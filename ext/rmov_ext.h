#include <ruby.h>
#include <QuickTime/QuickTime.h>

extern VALUE mQuicktime, cMovie, cTrack;

/*** MOVIE ***/

void Init_quicktime_movie();

#define RMOVIE(obj) (Check_Type(obj, T_DATA), (struct RMovie*)DATA_PTR(obj))
#define MOVIE_PTR(obj) (RMOVIE(obj)->movie)
#define MOVIE(obj) (*MOVIE_PTR(obj))

struct RMovie {
  Movie *movie;
};


/*** TRACK ***/

void Init_quicktime_track();

#define RTRACK(obj) (Check_Type(obj, T_DATA), (struct RTrack*)DATA_PTR(obj))
#define TRACK_PTR(obj) (RTRACK(obj)->track)
#define TRACK(obj) (*TRACK_PTR(obj))
#define TRACK_MEDIA(obj) (GetTrackMedia(TRACK(obj)))

struct RTrack {
  Track *track;
};
