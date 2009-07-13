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
    rb_raise(eQuickTime, "Unable to fetch track for movie at index %d", NUM2INT(index_obj));
  
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
  } else if (media_type == TextMediaType) {
    return ID2SYM(rb_intern("text"));
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

/*
  call-seq: offset() -> seconds
  
  Returns the offset of the track from the beginning of the movie (in seconds).
*/
static VALUE track_get_offset(VALUE obj)
{
  return rb_float_new((double)GetTrackOffset(TRACK(obj))/GetMediaTimeScale(TRACK_MEDIA(obj)));
}

/*
  call-seq: offset=(seconds)
  
  Sets the offset of the track from the start of the movie (in seconds).
*/
static VALUE track_set_offset(VALUE obj, VALUE seconds)
{
  SetTrackOffset(TRACK(obj), TRACK_TIME(obj, seconds));
  return Qnil;
}

/*
  call-seq: new_video_media
  
  Creates a new video media for this track.
  
  Generally this method is not called directly, instead you can make a 
  new video track using Movie#new_video_track.
*/
static VALUE track_new_video_media(VALUE obj)
{
  NewTrackMedia(TRACK(obj), VideoMediaType, 600, 0, 0);
  return obj;
}

/*
  call-seq: new_audio_media
  
  Creates a new audio media for this track.
  
  Generally this method is not called directly, instead you can make a 
  new audio track using Movie#new_audio_track.
*/
static VALUE track_new_audio_media(VALUE obj)
{
  NewTrackMedia(TRACK(obj), SoundMediaType, 44100, 0, 0);
  return obj;
}

/*
  call-seq: new_text_media
  
  Creates a new text media for this track.
  
  Generally this method is not called directly, instead you can make a 
  new text track using Movie#new_text_track.
*/
static VALUE track_new_text_media(VALUE obj)
{
  NewTrackMedia(TRACK(obj), TextMediaType, 600, 0, 0);
  return obj;
}

/*
  call-seq: enable_alpha
  
  Enable the straight alpha graphic mode for this track.
  
  This is best used on an overlayed video track which includes some
  alpha transparency (such as in a PNG image).
*/
static VALUE track_enable_alpha(VALUE obj)
{
  MediaSetGraphicsMode(GetMediaHandler(TRACK_MEDIA(obj)), graphicsModeStraightAlpha, 0);
  return obj;
}

/*
  call-seq: scale
  
  Scale the track's size by width and height respectively.
  
  The value passed is a relative float where "1" is the current size.
*/
static VALUE track_scale(VALUE obj, VALUE width, VALUE height)
{
  MatrixRecord matrix;
  GetTrackMatrix(TRACK(obj), &matrix);
  ScaleMatrix(&matrix, FloatToFixed(NUM2DBL(width)), FloatToFixed(NUM2DBL(height)), 0, 0);
  SetTrackMatrix(TRACK(obj), &matrix);
  return obj;
}

/*
  call-seq: bounds() -> bounds_hash
  
  Returns a hash of boundaries. The hash contains four keys: :left, :top, 
  :right, :bottom. Each holds an integer representing the pixel value.
*/
static VALUE track_bounds(VALUE obj)
{
  VALUE bounds_hash = rb_hash_new();
  RgnHandle region;
  Rect bounds;
  region = GetTrackDisplayBoundsRgn(TRACK(obj));
  GetRegionBounds(region, &bounds);
  DisposeRgn(region);
  rb_hash_aset(bounds_hash, ID2SYM(rb_intern("left")), INT2NUM(bounds.left));
  rb_hash_aset(bounds_hash, ID2SYM(rb_intern("top")), INT2NUM(bounds.top));
  rb_hash_aset(bounds_hash, ID2SYM(rb_intern("right")), INT2NUM(bounds.right));
  rb_hash_aset(bounds_hash, ID2SYM(rb_intern("bottom")), INT2NUM(bounds.bottom));
  return bounds_hash;
}

void Init_quicktime_track()
{
  VALUE mQuickTime;
  mQuickTime = rb_define_module("QuickTime");
  cTrack = rb_define_class_under(mQuickTime, "Track", rb_cObject);
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
  rb_define_method(cTrack, "offset", track_get_offset, 0);
  rb_define_method(cTrack, "offset=", track_set_offset, 1);
  rb_define_method(cTrack, "new_video_media", track_new_video_media, 0);
  rb_define_method(cTrack, "new_audio_media", track_new_audio_media, 0);
  rb_define_method(cTrack, "new_text_media", track_new_text_media, 0);
  rb_define_method(cTrack, "enable_alpha", track_enable_alpha, 0);
  rb_define_method(cTrack, "scale", track_scale, 2);
  rb_define_method(cTrack, "bounds", track_bounds, 0);
}
