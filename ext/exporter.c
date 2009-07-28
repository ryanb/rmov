#include "rmov_ext.h"

VALUE cExporter;

static void exporter_free(struct RExporter *rExporter)
{
  if (rExporter->settings) {
    QTDisposeAtomContainer(rExporter->settings);
  }
}

static void exporter_mark(struct RExporter *rExporter)
{
}

/*
  call-seq: new(movie) -> exporter
  
  Creates a new exporter instance. Usually this is done through movie.exporter.
*/
static VALUE exporter_new(VALUE klass)
{
  struct RExporter *rExporter;
  return Data_Make_Struct(klass, struct RExporter, exporter_mark, exporter_free, rExporter);
}

static ComponentInstance exporter_component(VALUE obj)
{
  ComponentInstance component = OpenDefaultComponent('spit', 'MooV');
  if (REXPORTER(obj)->settings) {
    MovieExportSetSettingsFromAtomContainer(component, REXPORTER(obj)->settings);
  }
  return component;
}

/*
  call-seq: export_to_file(filepath)
  
  Exports a movie to the given filepath. This will use either the 
  settings you set beforehand, or QuickTime's defaults.

  You can track the progress of this operation by passing a block to this 
  method. It will be called regularly during the process and pass the 
  percentage complete (0.0 to 1.0) as an argument to the block.
*/
static VALUE exporter_export_to_file(VALUE obj, VALUE filepath)
{
  OSErr err;
  FSSpec fs;
  Movie movie = MOVIE(rb_iv_get(obj, "@movie"));
  ComponentInstance component = exporter_component(obj);
  
  if (rb_block_given_p())
    SetMovieProgressProc(movie, (MovieProgressUPP)movie_progress_proc, rb_block_proc());
  
  // Activate so QuickTime doesn't export a white frame
  SetMovieActive(movie, TRUE);
  
  err = NativePathNameToFSSpec(RSTRING(filepath)->ptr, &fs, 0);
  if (err != fnfErr)
    rb_raise(eQuickTime, "Error %d occurred while opening file for export at %s.", err, RSTRING(filepath)->ptr);
  
  // TODO use exporter settings when converting movie
  err = ConvertMovieToFile(movie, 0, &fs, 'MooV', 'TVOD', 0, 0, 0, component);
  if (err != noErr)
    rb_raise(eQuickTime, "Error %d occurred while attempting to export movie to file %s.", err, RSTRING(filepath)->ptr);
  
  if (rb_block_given_p())
    SetMovieProgressProc(movie, 0, 0);
  
  CloseComponent(component);
  
  return Qnil;
}

/*
  call-seq: open_settings_dialog()
  
  Opens the offical QuickTime GUI settings dialog. The process will be 
  suspended until the user closes the dialogue. If the user clicks Okay 
  the settings will be applied to this Exporter. You can then use 
  save_settings to save them to a file, and load_settings to load them 
  back again.
*/
static VALUE exporter_open_settings_dialog(VALUE obj)
{
  Boolean canceled;
  OSErr err;
  ProcessSerialNumber current_process = {0, kCurrentProcess};
  Movie movie = MOVIE(rb_iv_get(obj, "@movie"));
  ComponentInstance component = exporter_component(obj);
  
  // Bring this process to the front
  err = TransformProcessType(&current_process, kProcessTransformToForegroundApplication);
  if (err != noErr) {
    rb_raise(eQuickTime, "Error %d occurred while bringing this application to the forground.", err);
  }
  SetFrontProcess(&current_process);
  
  // Show export dialog and save settings
  err = MovieExportDoUserDialog(component, movie, 0, 0, GetMovieDuration(movie), &canceled);
  if (err != noErr) {
    rb_raise(eQuickTime, "Error %d occurred while opening export dialog.", err);
  }
  
  if (!canceled) {
    // Clear existing settings if there are any
    if (REXPORTER(obj)->settings) {
      QTDisposeAtomContainer(REXPORTER(obj)->settings);
    }
    MovieExportGetSettingsAsAtomContainer(component, &REXPORTER(obj)->settings);
  }
  
  CloseComponent(component);
  
  if (canceled) {
    return Qfalse;
  } else {
    return Qtrue;
  }
}

/*
  call-seq: load_settings(filepath)
  
  Loads the settings at the given filepath. See save_settings.
*/
static VALUE exporter_load_settings(VALUE obj, VALUE filepath)
{
  FILE *file;
  long length, read_length;
  
  file = fopen(RSTRING(filepath)->ptr, "r+b");
  if (!file) {
    rb_raise(eQuickTime, "Unable to open file for loading at %s.", RSTRING(filepath)->ptr);
  }
  
  // obtain file size:
  fseek(file , 0, SEEK_END);
  length = ftell(file);
  rewind(file);
  
  // clear existing settings if there are any
  if (REXPORTER(obj)->settings) {
    QTDisposeAtomContainer(REXPORTER(obj)->settings);
  }
  
  // load the file into settings
  REXPORTER(obj)->settings = (QTAtomContainer)NewHandleClear(length);
  read_length = fread(*(Handle)REXPORTER(obj)->settings, 1, length, file);
  if (read_length != length) {
    rb_raise(eQuickTime, "Unable to read entire file at %s.", RSTRING(filepath)->ptr);
  }
  
  fclose(file);
  
  return Qnil;
}

/*
  call-seq: save_settings(filepath)
  
  Saves the settings to the given filepath (usually with .st extension). 
  See open_settings_dialog and load_settings.
*/
static VALUE exporter_save_settings(VALUE obj, VALUE filepath)
{
  FILE *file;
  QTAtomContainer settings = REXPORTER(obj)->settings;
  
  if (!settings) {
    rb_raise(eQuickTime, "Unable to save settings because no settings are specified.");
  }
  
  file = fopen(RSTRING(filepath)->ptr, "wb");
  if (!file) {
    rb_raise(eQuickTime, "Unable to open file for saving at %s.", RSTRING(filepath)->ptr);
  }
  fwrite(*settings, GetHandleSize((Handle)settings), 1, file);
  fclose(file);
  
  return Qnil;
}

void Init_quicktime_exporter()
{
  VALUE mQuickTime;
  mQuickTime = rb_define_module("QuickTime");
  cExporter = rb_define_class_under(mQuickTime, "Exporter", rb_cObject);
  rb_define_alloc_func(cExporter, exporter_new);
  rb_define_method(cExporter, "export", exporter_export_to_file, 1);
  rb_define_method(cExporter, "open_settings_dialog", exporter_open_settings_dialog, 0);
  rb_define_method(cExporter, "load_settings", exporter_load_settings, 1);
  rb_define_method(cExporter, "save_settings", exporter_save_settings, 1);
}
