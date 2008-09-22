#include <ruby.h>
#include <QuickTime/QuickTime.h>

extern VALUE mQuicktime, cMovie, cTrack;

/*** MOVIE ***/

void Init_quicktime_movie();

#define RMOVIE(obj) (Check_Type(obj, T_DATA), (struct RMovie*)DATA_PTR(obj))
#define MOVIE(obj) (RMOVIE(obj)->movie)
#define MOVIE_TIME(obj, seconds) (floor(NUM2DBL(seconds)*GetMovieTimeScale(MOVIE(obj))))

struct RMovie {
  Movie movie;
};


/*** TRACK ***/

void Init_quicktime_track();

#define RTRACK(obj) (Check_Type(obj, T_DATA), (struct RTrack*)DATA_PTR(obj))
#define TRACK(obj) (RTRACK(obj)->track)
#define TRACK_MEDIA(obj) (GetTrackMedia(TRACK(obj)))

struct RTrack {
  Track track;
};
