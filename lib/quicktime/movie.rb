module Quicktime
  # see ext/movie.c for additional methods
  class Movie
    def self.open(filepath)
      new.load_from_file(filepath)
    end
    
    def self.empty
      new.load_empty
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
    
    def exporter
      Exporter.new(self)
    end
    
    def export(*args, &block)
      exporter.export(*args, &block)
    end
  end
end
