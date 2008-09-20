#include "rmov_ext.h"

VALUE mQuicktime;

void Init_rmov_ext()
{
  EnterMovies(); // Enables the QuickTime framework
  mQuicktime = rb_define_module("Quicktime");
  Init_quicktime_movie(mQuicktime);
  Init_quicktime_track(mQuicktime);
}
