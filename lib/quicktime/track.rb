module Quicktime
  # see rmov_ext for additional methods
  class Track
    def duration
      raw_duration.to_f/time_scale
    end
  end
end
