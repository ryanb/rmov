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
    rb_raise(eQuicktime, "Error while attempting to open file for export at %s.", RSTRING(filepath)->ptr);
  
  // TODO use exporter settings when converting movie
  err = ConvertMovieToFile(movie, 0, &fs, 'MooV', 'TVOD', 0, 0, 0, component);
  if (err != noErr)
    rb_raise(eQuicktime, "Error while attempting to export movie to file %s.", RSTRING(filepath)->ptr);
  
  if (rb_block_given_p())
    SetMovieProgressProc(movie, 0, 0);
  
  CloseComponent(component);
  
  return Qnil;
}

static VALUE exporter_open_settings_dialog(VALUE obj)
{
  Boolean canceled;
  OSErr err;
  ProcessSerialNumber current_process = {0, kCurrentProcess};
  Movie movie = MOVIE(rb_iv_get(obj, "@movie"));
  ComponentInstance component = exporter_component(obj);
  
  // Bring this process to the front
  if (!IsProcessVisible(&current_process)) {
    err = TransformProcessType(&current_process, kProcessTransformToForegroundApplication);
    if (err != noErr) {
      rb_raise(eQuicktime, "Error while attempting to put make this a forground application.");
    }
  }
  SetFrontProcess(&current_process);
  
  // Show export dialog and save settings
  err = MovieExportDoUserDialog(component, movie, 0, 0, GetMovieDuration(movie), &canceled);
  if (err != noErr) {
    rb_raise(eQuicktime, "Error while attempting to open export dialog");
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

static VALUE exporter_load_settings(VALUE obj, VALUE filepath)
{
  FILE *file;
  long length, read_length;
  
  file = fopen(RSTRING(filepath)->ptr, "r+b");
  if (!file) {
    rb_raise(eQuicktime, "Unable to open file for loading at %s.", RSTRING(filepath)->ptr);
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
    rb_raise(eQuicktime, "Unable to read entire file at %s.", RSTRING(filepath)->ptr);
  }
  
  fclose(file);
  
  return Qnil;
}

static VALUE exporter_save_settings(VALUE obj, VALUE filepath)
{
  FILE *file;
  QTAtomContainer settings = REXPORTER(obj)->settings;
  
  if (!settings) {
    rb_raise(eQuicktime, "Unable to save settings because no settings are specified.");
  }
  
  file = fopen(RSTRING(filepath)->ptr, "wb");
  if (!file) {
    rb_raise(eQuicktime, "Unable to open file for saving at %s.", RSTRING(filepath)->ptr);
  }
  fwrite(&settings, GetHandleSize((Handle)settings), 1, file);
  fclose(file);
  
  return Qnil;
}

void Init_quicktime_exporter()
{
  cExporter = rb_define_class_under(mQuicktime, "Exporter", rb_cObject);
  rb_define_alloc_func(cExporter, exporter_new);
  rb_define_method(cExporter, "export", exporter_export_to_file, 1);
  rb_define_method(cExporter, "open_settings_dialog", exporter_open_settings_dialog, 0);
  rb_define_method(cExporter, "load_settings", exporter_load_settings, 1);
  rb_define_method(cExporter, "save_settings", exporter_save_settings, 1);
}
