#include "rmov_ext.h"

VALUE mQuicktime;
VALUE eQuicktime;
VALUE eMovieLoaded;
VALUE eMovieNotLoaded;

void Init_rmov_ext()
{
  EnterMovies(); // Enables the QuickTime framework
  mQuicktime = rb_define_module("Quicktime");
  eQuicktime = rb_define_class_under(mQuicktime, "Error", rb_eStandardError);
  eMovieLoaded = rb_define_class_under(mQuicktime, "MovieLoaded", eQuicktime);
  Init_quicktime_movie();
  Init_quicktime_track();
}
