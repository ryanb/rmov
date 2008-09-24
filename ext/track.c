#include "rmov_ext.h"

VALUE cTrack;

static void track_free(struct RTrack *rTrack)
{
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
  RTRACK(obj)->track = GetMovieIndTrack(MOVIE(movie_obj), NUM2INT(index_obj));
  if (!RTRACK(obj)->track)
    rb_raise(eQuicktime, "Unable to fetch track for movie at index %d", NUM2INT(index_obj));
  
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
  if (media_type == SoundMediaType) {
    return ID2SYM(rb_intern("audio"));
  } else if (media_type == VideoMediaType) {
    return ID2SYM(rb_intern("video"));
  } else {
    return Qnil;
  }
}

static VALUE track_id(VALUE obj)
{
  return INT2NUM(GetTrackID(TRACK(obj)));
}

static VALUE track_delete(VALUE obj)
{
  DisposeMovieTrack(TRACK(obj));
  return Qnil;
}

static VALUE track_disable(VALUE obj, VALUE boolean)
{
  SetTrackEnabled(TRACK(obj), FALSE);
  return obj;
}

static VALUE track_enable(VALUE obj, VALUE boolean)
{
  SetTrackEnabled(TRACK(obj), TRUE);
  return obj;
}

static VALUE track_enabled(VALUE obj, VALUE boolean)
{
  if (GetTrackEnabled(TRACK(obj)) == TRUE) {
    return Qtrue;
  } else {
    return Qfalse;
  }
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
  rb_define_method(cTrack, "delete", track_delete, 0);
  rb_define_method(cTrack, "enabled?", track_enabled, 0);
  rb_define_method(cTrack, "enable", track_enable, 0);
  rb_define_method(cTrack, "disable", track_disable, 0);
}
