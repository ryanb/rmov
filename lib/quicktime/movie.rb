module Quicktime
  # see rmov_ext for additional methods
  class Movie
    def self.open(filepath)
      new.load_from_file(filepath)
    end
    
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
    
    def audio_tracks
      tracks.select { |t| t.audio? }
    end
    
    def video_tracks
      tracks.select { |t| t.video? }
    end
    
    def export(path)
      convert_to_file(path)
    end
  end
end
