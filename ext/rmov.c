#include <ruby.h>
#include <QuickTime/QuickTime.h>

VALUE mQuicktime;
VALUE cMovie;

#define RMOVIE(obj) (Check_Type(obj, T_DATA), (struct RMovie*)DATA_PTR(obj))
#define MOVIE(obj)  (RMOVIE(obj)->movie)

struct RMovie {
  Movie *movie;
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

static VALUE movie_init(VALUE obj, VALUE filepath)
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
  
  return Qnil;
}

// The length of the movie in seconds
static VALUE movie_duration(VALUE obj)
{
  return rb_float_new((double)GetMovieDuration(*MOVIE(obj))/GetMovieTimeScale(*MOVIE(obj)));
}

void Init_rmov()
{
  EnterMovies(); // Enables the QuickTime framework
  mQuicktime = rb_define_module("Quicktime");
  cMovie = rb_define_class_under(mQuicktime, "Movie", rb_cObject);
  rb_define_alloc_func(cMovie, movie_new);
  rb_define_method(cMovie, "initialize", movie_init, 1);
  rb_define_method(cMovie, "duration", movie_duration, 0);
}
