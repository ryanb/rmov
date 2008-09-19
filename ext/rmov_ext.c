#include <ruby.h>
#include <QuickTime/QuickTime.h>

VALUE mQuicktime;
VALUE cMovie;
VALUE cTrack;

#define RMOVIE(obj) (Check_Type(obj, T_DATA), (struct RMovie*)DATA_PTR(obj))
#define MOVIE_PTR(obj) (RMOVIE(obj)->movie)
#define MOVIE(obj) (*MOVIE_PTR(obj))
struct RMovie {
  Movie *movie;
};

#define RTRACK(obj) (Check_Type(obj, T_DATA), (struct RTrack*)DATA_PTR(obj))
#define TRACK_PTR(obj) (RTRACK(obj)->track)
#define TRACK(obj) (*TRACK_PTR(obj))
#define TRACK_MEDIA(obj) (GetTrackMedia(TRACK(obj)))
struct RTrack {
  Track *track;
};

static void movie_free(struct RMovie *rMovie)
{
  if (rMovie->movie)
    DisposeMovie(*rMovie->movie);
}

static void movie_mark(struct RMovie *rMovie)
{
  
}

static VALUE movie_new(VALUE klass)
{
  struct RMovie *rMovie;
  return Data_Make_Struct(klass, struct RMovie, movie_mark, movie_free, rMovie);
}

static VALUE movie_load(VALUE obj, VALUE filepath)
{
  OSErr err;
  FSSpec fs;
  short frefnum = -1;
  short movie_resid = 0;
  Movie *movie = ALLOC(Movie);
  
  // TODO add error handling
  err = NativePathNameToFSSpec(RSTRING(filepath)->ptr, &fs, 0);
  err = OpenMovieFile(&fs, &frefnum, 0);
  err = NewMovieFromFile(movie, frefnum, &movie_resid, 0, newMovieActive, 0);
  RMOVIE(obj)->movie = movie;
  
  return obj;
}

static VALUE movie_raw_duration(VALUE obj)
{
  return INT2NUM(GetMovieDuration(MOVIE(obj)));
}

static VALUE movie_time_scale(VALUE obj)
{
  return INT2NUM(GetMovieTimeScale(MOVIE(obj)));
}

static VALUE movie_bounds(VALUE obj)
{
  VALUE bounds_hash = rb_hash_new();
  Rect bounds;
  GetMovieBox(MOVIE(obj), &bounds);
  rb_hash_aset(bounds_hash, ID2SYM(rb_intern("left")), INT2NUM(bounds.left));
  rb_hash_aset(bounds_hash, ID2SYM(rb_intern("top")), INT2NUM(bounds.top));
  rb_hash_aset(bounds_hash, ID2SYM(rb_intern("right")), INT2NUM(bounds.right));
  rb_hash_aset(bounds_hash, ID2SYM(rb_intern("bottom")), INT2NUM(bounds.bottom));
  return bounds_hash;
}

static VALUE movie_track_count(VALUE obj)
{
  return INT2NUM(GetMovieTrackCount(MOVIE(obj)));
}

static void track_free(struct RMovie *rMovie)
{
}

static void track_mark(struct RMovie *rMovie)
{
}

static VALUE track_new(VALUE klass)
{
  struct RTrack *rTrack;
  return Data_Make_Struct(klass, struct RTrack, track_mark, track_free, rTrack);
}

static VALUE track_load(VALUE obj, VALUE movie_obj, VALUE index_obj)
{
  Track track;
  
  // TODO add error handling
  track = GetMovieIndTrack(MOVIE(movie_obj), NUM2INT(index_obj));
  RTRACK(obj)->track = &track;
  
  return obj;
}

static VALUE track_raw_duration(VALUE obj)
{
  return INT2NUM(GetMediaDuration(TRACK_MEDIA(obj)));
}

static VALUE track_time_scale(VALUE obj)
{
  return INT2NUM(GetMediaTimeScale(TRACK_MEDIA(obj)));
}

void Init_rmov_ext()
{
  EnterMovies(); // Enables the QuickTime framework
  mQuicktime = rb_define_module("Quicktime");
  cMovie = rb_define_class_under(mQuicktime, "Movie", rb_cObject);
  rb_define_alloc_func(cMovie, movie_new);
  rb_define_method(cMovie, "load_from_file", movie_load, 1);
  rb_define_method(cMovie, "raw_duration", movie_raw_duration, 0);
  rb_define_method(cMovie, "time_scale", movie_time_scale, 0);
  rb_define_method(cMovie, "bounds", movie_bounds, 0);
  rb_define_method(cMovie, "track_count", movie_track_count, 0);
  cTrack = rb_define_class_under(mQuicktime, "Track", rb_cObject);
  rb_define_alloc_func(cTrack, track_new);
  rb_define_method(cTrack, "load_from_movie", track_load, 2);
  rb_define_method(cTrack, "raw_duration", track_raw_duration, 0);
  rb_define_method(cTrack, "time_scale", track_time_scale, 0);
}
