#include "rmov_ext.h"

VALUE cTrack;

static void track_free(struct RTrack *rTrack)
{
}

static void track_mark(struct RTrack *rTrack)
{
}

/*
  call-seq: new() -> track
  
  Creates a new track instance. Generally you will do this through 
  movie.tracks to fetch the Track instances for a given movie.
*/
static VALUE track_new(VALUE klass)
{
  struct RTrack *rTrack;
  return Data_Make_Struct(klass, struct RTrack, track_mark, track_free, rTrack);
}

/*
  call-seq: load(movie, index)
  
  Loads a QuickTime track from a given movie. This is done automatically 
  when calling movie.tracks.
*/
static VALUE track_load(VALUE obj, VALUE movie_obj, VALUE index_obj)
{
  RTRACK(obj)->track = GetMovieIndTrack(MOVIE(movie_obj), NUM2INT(index_obj));
  if (!RTRACK(obj)->track)
    rb_raise(eQuicktime, "Unable to fetch track for movie at index %d", NUM2INT(index_obj));
  
  return obj;
}

/*
  call-seq: raw_duration() -> duration_int
  
  Returns the raw duration of the track. Combine this with time_scale to 
  reach the duration in seconds.
*/
static VALUE track_raw_duration(VALUE obj)
{
  return INT2NUM(GetMediaDuration(TRACK_MEDIA(obj)));
}

/*
  call-seq: time_scale() -> scale_int
  
  Returns the time scale of the track. Usually only needed when working 
  with raw_duration.
*/
static VALUE track_time_scale(VALUE obj)
{
  return INT2NUM(GetMediaTimeScale(TRACK_MEDIA(obj)));
}

/*
  call-seq: frame_count() -> count
  
  Returns the number of frames in the track.
*/
static VALUE track_frame_count(VALUE obj)
{
  return INT2NUM(GetMediaSampleCount(TRACK_MEDIA(obj)));
}

/*
  call-seq: media_type() -> media_type_sym
  
  Returns either :audio or :video depending on the type of track this is.
*/
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

/*
  call-seq: id() -> quicktime_track_id_int
  
  Returns either id number QuickTime uses to reference this track. 
  Usually only used internally.
*/
static VALUE track_id(VALUE obj)
{
  return INT2NUM(GetTrackID(TRACK(obj)));
}

/*
  call-seq: delete()
  
  Removes the track from its movie and deletes it from memory.
*/
static VALUE track_delete(VALUE obj)
{
  DisposeMovieTrack(TRACK(obj));
  return Qnil;
}

/*
  call-seq: disable()
  
  Disables the track. See enabled? to determine if it's disabled already.
*/
static VALUE track_disable(VALUE obj, VALUE boolean)
{
  SetTrackEnabled(TRACK(obj), FALSE);
  return obj;
}

/*
  call-seq: enable()
  
  Enables the track. See enabled? to determine if it's enabled already.
*/
static VALUE track_enable(VALUE obj, VALUE boolean)
{
  SetTrackEnabled(TRACK(obj), TRUE);
  return obj;
}

/*
  call-seq: enabled?() -> bool
  
  Returns true/false depending on if the track is enabled.
*/
static VALUE track_enabled(VALUE obj, VALUE boolean)
{
  if (GetTrackEnabled(TRACK(obj)) == TRUE) {
    return Qtrue;
  } else {
    return Qfalse;
  }
}

/*
  call-seq: volume() -> volume_float
  
  Returns the volume of the audio from 0.0 to 1.0.
*/
static VALUE track_get_volume(VALUE obj)
{
  return rb_float_new((double)GetTrackVolume(TRACK(obj))/0x0100);
}

/*
  call-seq: volume=(volume_float)
  
  Sets the volume to the given value (0.0 to 1.0)
*/
static VALUE track_set_volume(VALUE obj, VALUE volume_obj)
{
  SetTrackVolume(TRACK(obj), (short)(0x0100*NUM2DBL(volume_obj)));
  return Qnil;
}


void Init_quicktime_track()
{
  VALUE mQuicktime;
  mQuicktime = rb_define_module("Quicktime");
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
  rb_define_method(cTrack, "volume", track_get_volume, 0);
  rb_define_method(cTrack, "volume=", track_set_volume, 1);
}
