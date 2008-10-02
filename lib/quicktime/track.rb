module Quicktime
  # see ext/track.c for additional methods
  class Track
    # Returns the length of this track in seconds
    # using raw_duration and time_scale.
    def duration
      raw_duration.to_f/time_scale
    end
    
    # The average frame_rate for this track. May not be exact.
    def frame_rate # what about odd frame rates such as 29.97?
      frame_count/duration
    end
    
    # Returns true/false depending on if track is an audio track.
    def audio?
      media_type == :audio
    end
    
    # Returns true/false depending on if track is a video track.
    def video?
      media_type == :video
    end
  end
end
