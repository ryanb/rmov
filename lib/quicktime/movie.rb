module Quicktime
  # see rmov_ext for additional methods
  class Movie
    def duration
      raw_duration.to_f/time_scale
    end
    
    def width
      bounds[:right] - bounds[:left]
    end
    
    def height
      bounds[:bottom] - bounds[:top]
    end
  end
end
