#include "rmov_ext.h"

VALUE eQuickTime;

void Init_rmov_ext()
{
  VALUE mQuickTime;
  
  EnterMovies(); // Enables the QuickTime framework
  
  mQuickTime = rb_define_module("QuickTime");
  eQuickTime = rb_define_class_under(mQuickTime, "Error", rb_eStandardError);
  Init_quicktime_movie();
  Init_quicktime_track();
  Init_quicktime_exporter();
}
