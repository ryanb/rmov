#include <ruby.h>
#include <QuickTime/QuickTime.h>

extern VALUE eQuickTime, cMovie, cTrack, cExporter;


#define OSTYPE(str) ((str[0] << 24) | (str[1] << 16) | (str[2] << 8) | str[3])

/*** MOVIE ***/

void Init_quicktime_movie();
OSErr movie_progress_proc(Movie movie, short message, short operation, Fixed percent, VALUE proc);

#define RMOVIE(obj) (Check_Type(obj, T_DATA), (struct RMovie*)DATA_PTR(obj))
#define MOVIE(obj) (RMOVIE(obj)->movie)
#define MOVIE_TIME(obj, seconds) (floor(NUM2DBL(seconds)*GetMovieTimeScale(MOVIE(obj))))

struct RMovie {
  Movie movie;
  short resId;
  char *filepath;
};


/*** TRACK ***/

void Init_quicktime_track();

#define RTRACK(obj) (Check_Type(obj, T_DATA), (struct RTrack*)DATA_PTR(obj))
#define TRACK(obj) (RTRACK(obj)->track)
#define TRACK_MEDIA(obj) (GetTrackMedia(TRACK(obj)))
#define TRACK_TIME(obj, seconds) (floor(NUM2DBL(seconds)*GetMediaTimeScale(TRACK_MEDIA(obj))))

struct RTrack {
  Track track;
};


/*** EXPORTER ***/

void Init_quicktime_exporter();

#define REXPORTER(obj) (Check_Type(obj, T_DATA), (struct RExporter*)DATA_PTR(obj))

struct RExporter {
  QTAtomContainer settings;
};
