module Quicktime
  # see rmov_ext for additional methods
  class Track
    def duration
      raw_duration.to_f/time_scale
    end
    
    def frame_rate # what about odd frame rates such as 29.97?
      frame_count/duration
    end
  end
end
