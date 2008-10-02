#include "rmov_ext.h"

VALUE eQuicktime;

void Init_rmov_ext()
{
  VALUE mQuicktime;
  
  EnterMovies(); // Enables the QuickTime framework
  
  mQuicktime = rb_define_module("Quicktime");
  eQuicktime = rb_define_class_under(mQuicktime, "Error", rb_eStandardError);
  Init_quicktime_movie();
  Init_quicktime_track();
  Init_quicktime_exporter();
}
