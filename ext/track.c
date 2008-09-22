#include "rmov_ext.h"

VALUE cTrack;

static void track_free(struct RTrack *rTrack)
{
  // TODO ensure I don't need to dispose the track or media here
}

static void track_mark(struct RTrack *rTrack)
{
}

static VALUE track_new(VALUE klass)
{
  struct RTrack *rTrack;
  return Data_Make_Struct(klass, struct RTrack, track_mark, track_free, rTrack);
}

static VALUE track_load(VALUE obj, VALUE movie_obj, VALUE index_obj)
{
  // TODO add error handling
  RTRACK(obj)->track = GetMovieIndTrack(MOVIE(movie_obj), NUM2INT(index_obj));
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

static VALUE track_frame_count(VALUE obj)
{
  return INT2NUM(GetMediaSampleCount(TRACK_MEDIA(obj)));
}

static VALUE track_media_type(VALUE obj)
{
  OSType media_type;
  
  GetMediaHandlerDescription(TRACK_MEDIA(obj), &media_type, 0, 0);
  if (media_type == 'soun') {
    return ID2SYM(rb_intern("audio"));
  } else if (media_type == 'vide') {
    return ID2SYM(rb_intern("video"));
  } else {
    return Qnil;
  }
}

static VALUE track_id(VALUE obj)
{
  return INT2NUM(GetTrackID(TRACK(obj)));
}

void Init_quicktime_track()
{
  cTrack = rb_define_class_under(mQuicktime, "Track", rb_cObject);
  rb_define_alloc_func(cTrack, track_new);
  rb_define_method(cTrack, "load_from_movie", track_load, 2);
  rb_define_method(cTrack, "raw_duration", track_raw_duration, 0);
  rb_define_method(cTrack, "time_scale", track_time_scale, 0);
  rb_define_method(cTrack, "frame_count", track_frame_count, 0);
  rb_define_method(cTrack, "media_type", track_media_type, 0);
  rb_define_method(cTrack, "id", track_id, 0);
}
