#ifndef rmov_ext_h
#define rmov_ext_h

#include <ruby.h>
#include <QuickTime/QuickTime.h>

VALUE mQuicktime;


/*** MOVIE ***/

VALUE cMovie;

void Init_quicktime_movie();

#define RMOVIE(obj) (Check_Type(obj, T_DATA), (struct RMovie*)DATA_PTR(obj))
#define MOVIE_PTR(obj) (RMOVIE(obj)->movie)
#define MOVIE(obj) (*MOVIE_PTR(obj))

struct RMovie {
  Movie *movie;
};


/*** TRACK ***/

VALUE cTrack;

#define RTRACK(obj) (Check_Type(obj, T_DATA), (struct RTrack*)DATA_PTR(obj))
#define TRACK_PTR(obj) (RTRACK(obj)->track)
#define TRACK(obj) (*TRACK_PTR(obj))
#define TRACK_MEDIA(obj) (GetTrackMedia(TRACK(obj)))

struct RTrack {
  Track *track;
};

#endif
