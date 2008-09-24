#include "rmov_ext.h"

VALUE cExporter;

static void exporter_free(struct RExporter *rExporter)
{
}

static void exporter_mark(struct RExporter *rExporter)
{
}

static VALUE exporter_new(VALUE klass)
{
  struct RExporter *rExporter;
  return Data_Make_Struct(klass, struct RExporter, exporter_mark, exporter_free, rExporter);
}

static VALUE exporter_export_to_file(VALUE obj, VALUE filepath)
{
  OSErr err;
  FSSpec fs;
  Movie movie = MOVIE(rb_iv_get(obj, "@movie"));
  
  if (rb_block_given_p())
    SetMovieProgressProc(movie, (MovieProgressUPP)movie_progress_proc, rb_block_proc());
  
  // Activate so QuickTime doesn't export a white frame
  SetMovieActive(movie, TRUE);
  
  // TODO add error handling
  err = NativePathNameToFSSpec(RSTRING(filepath)->ptr, &fs, 0);
  err = ConvertMovieToFile(movie, 0, &fs, 'MooV', 'TVOD', 0, 0, 0, 0);
  
  if (rb_block_given_p())
    SetMovieProgressProc(movie, 0, 0);
  
  return Qnil;
}

static VALUE exporter_open_settings_dialog(VALUE obj)
{
  Boolean canceled;
  OSErr err;
  ProcessSerialNumber psn = {0, kCurrentProcess};
  Movie movie = MOVIE(rb_iv_get(obj, "@movie"));
  ComponentInstance component = OpenDefaultComponent('spit', 'MooV');
  
  // Attempt to bring this process to the front
  // TODO is it okay if this process is transformed multiple times?
  err = TransformProcessType(&psn, kProcessTransformToForegroundApplication);
  if (err == noErr) {
    (void)SetFrontProcess(&psn);
  }
  
  // TODO add error handling
  err = MovieExportDoUserDialog(component, movie, 0, 0, GetMovieDuration(movie), &canceled);
  if (canceled) {
    return Qfalse;
  } else {
    MovieExportGetSettingsAsAtomContainer(component, &REXPORTER(obj)->settings);
    return Qtrue;
  }
  
  // TODO we need to dispose of this atom container and close the component
}

static VALUE exporter_load_settings(VALUE obj, VALUE filepath)
{
  FILE *file;
  long length;
  QTAtomContainer atom;
  
  // TODO add error handling
  file = fopen(RSTRING(filepath)->ptr, "r+b");
  
  // obtain file size:
  fseek(file , 0, SEEK_END);
  length = ftell(file);
  rewind(file);
  
  // load the file
  // TODO dispose of old settings if it's already set
  REXPORTER(obj)->settings = (QTAtomContainer)NewHandleClear(length);
  fread(*(Handle)atom, length, 1, file);
  fclose(file);
  
  return Qnil;
}

static VALUE exporter_save_settings(VALUE obj, VALUE filepath)
{
  FILE *file;
  QTAtomContainer settings = REXPORTER(obj)->settings;
  
  // TODO add error handling
  file = fopen(RSTRING(filepath)->ptr, "wb");
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
