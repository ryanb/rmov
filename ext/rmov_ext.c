#include "rmov_ext.h"

void Init_rmov_ext()
{
  EnterMovies(); // Enables the QuickTime framework
  mQuicktime = rb_define_module("Quicktime");
  Init_quicktime_movie();
  Init_quicktime_track();
}
