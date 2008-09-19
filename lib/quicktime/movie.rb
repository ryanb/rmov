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
    
    def tracks
      (1..track_count).map do |i|
        Track.new.load_from_movie(self, i)
      end
    end
  end
end
