#include "rmov_ext.h"

VALUE cMovie;

static void movie_free(struct RMovie *rMovie)
{
  if (rMovie->movie)
    DisposeMovie(rMovie->movie);
}

static void movie_mark(struct RMovie *rMovie)
{
  
}

static VALUE movie_new(VALUE klass)
{
  struct RMovie *rMovie;
  return Data_Make_Struct(klass, struct RMovie, movie_mark, movie_free, rMovie);
}

static VALUE movie_load_from_file(VALUE obj, VALUE filepath)
{
  if (MOVIE(obj)) {
    rb_raise(eMovieLoaded, "Movie has already been loaded.");
  } else {
    OSErr err;
    FSSpec fs;
    short frefnum = -1;
    short movie_resid = 0;
    Movie *movie = ALLOC(Movie);
    
    err = NativePathNameToFSSpec(RSTRING(filepath)->ptr, &fs, 0);
    if (err != 0)
      rb_raise(eInvalidArgument, "No readable file found at %s", RSTRING(filepath)->ptr);
    
    err = OpenMovieFile(&fs, &frefnum, 0);
    if (err != 0)
      rb_raise(eInvalidArgument, "Unable to open movie at %s", RSTRING(filepath)->ptr);
    
    err = NewMovieFromFile(movie, frefnum, &movie_resid, 0, newMovieActive, 0);
    if (err != 0)
      rb_raise(eInvalidArgument, "Unable to load movie at %s", RSTRING(filepath)->ptr);
    
    RMOVIE(obj)->movie = *movie;
    
    return obj;
  }
}

static VALUE movie_load_empty(VALUE obj)
{
  if (MOVIE(obj)) {
    rb_raise(eMovieLoaded, "Movie has already been loaded.");
  } else {
    RMOVIE(obj)->movie = NewMovie(0);
    return obj; 
  }
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

static VALUE movie_convert_to_file(VALUE obj, VALUE filepath)
{
  OSErr err;
  FSSpec fs;
  
  // TODO add error handling
  err = NativePathNameToFSSpec(RSTRING(filepath)->ptr, &fs, 0);
  err = ConvertMovieToFile(MOVIE(obj), 0, &fs, 'MooV', 'TVOD', 0, 0, 0, 0);
  
  return obj;
}

static VALUE movie_composite_movie(VALUE obj, VALUE src, VALUE position)
{
  SetMovieSelection(MOVIE(obj), MOVIE_TIME(obj, position), 0);
  AddMovieSelection(MOVIE(obj), MOVIE(src));
  return obj;
}

static VALUE movie_insert_movie(VALUE obj, VALUE src, VALUE position)
{
  SetMovieSelection(MOVIE(obj), MOVIE_TIME(obj, position), 0);
  PasteMovieSelection(MOVIE(obj), MOVIE(src));
  return obj;
}

static VALUE movie_append_movie(VALUE obj, VALUE src)
{
  SetMovieSelection(MOVIE(obj), GetMovieDuration(MOVIE(obj)), 0);
  PasteMovieSelection(MOVIE(obj), MOVIE(src));
  return obj;
}

static VALUE movie_delete_section(VALUE obj, VALUE start, VALUE duration)
{
  SetMovieSelection(MOVIE(obj), MOVIE_TIME(obj, start), MOVIE_TIME(obj, duration));
  ClearMovieSelection(MOVIE(obj));
  return obj;
}

static VALUE movie_clone_section(VALUE obj, VALUE start, VALUE duration)
{
  VALUE new_movie_obj = rb_obj_alloc(cMovie);
  SetMovieSelection(MOVIE(obj), MOVIE_TIME(obj, start), MOVIE_TIME(obj, duration));
  RMOVIE(new_movie_obj)->movie = CopyMovieSelection(MOVIE(obj));
  return new_movie_obj;
}

static VALUE movie_clip_section(VALUE obj, VALUE start, VALUE duration)
{
  VALUE new_movie_obj = rb_obj_alloc(cMovie);
  SetMovieSelection(MOVIE(obj), MOVIE_TIME(obj, start), MOVIE_TIME(obj, duration));
  RMOVIE(new_movie_obj)->movie = CutMovieSelection(MOVIE(obj));
  return new_movie_obj;
}

void Init_quicktime_movie()
{
  cMovie = rb_define_class_under(mQuicktime, "Movie", rb_cObject);
  rb_define_alloc_func(cMovie, movie_new);
  rb_define_method(cMovie, "load_from_file", movie_load_from_file, 1);
  rb_define_method(cMovie, "load_empty", movie_load_empty, 0);
  rb_define_method(cMovie, "raw_duration", movie_raw_duration, 0);
  rb_define_method(cMovie, "time_scale", movie_time_scale, 0);
  rb_define_method(cMovie, "bounds", movie_bounds, 0);
  rb_define_method(cMovie, "track_count", movie_track_count, 0);
  rb_define_method(cMovie, "convert_to_file", movie_convert_to_file, 1);
  rb_define_method(cMovie, "composite_movie", movie_composite_movie, 2);
  rb_define_method(cMovie, "insert_movie", movie_insert_movie, 2);
  rb_define_method(cMovie, "append_movie", movie_append_movie, 1);
  rb_define_method(cMovie, "delete_section", movie_delete_section, 2);
  rb_define_method(cMovie, "clone_section", movie_clone_section, 2);
  rb_define_method(cMovie, "clip_section", movie_clip_section, 2);
}
