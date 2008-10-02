module Quicktime
  # see ext/movie.c for additional methods
  class Movie
    # Opens a movie at filepath.
    def self.open(filepath)
      new.load_from_file(filepath)
    end
    
    # Returns a new, empty movie.
    def self.empty
      new.load_empty
    end
    
    # Returns the length of this movie in seconds
    # using raw_duration and time_scale.
    def duration
      raw_duration.to_f/time_scale
    end
    
    # Returns the bounding width of this movie in number of pixels.
    def width
      bounds[:right] - bounds[:left]
    end
    
    # Returns the bounding height of this movie in number of pixels.
    def height
      bounds[:bottom] - bounds[:top]
    end
    
    # Returns an array of tracks in this movie.
    def tracks
      (1..track_count).map do |i|
        Track.new.load_from_movie(self, i)
      end
    end
    
    # Returns an array of audio tracks in this movie.
    def audio_tracks
      tracks.select { |t| t.audio? }
    end
    
    # Returns an array of video tracks in this movie.
    def video_tracks
      tracks.select { |t| t.video? }
    end
    
    # Returns an Exporter instance for this movie.
    def exporter
      Exporter.new(self)
    end
    
    # Convenience method for exporting the movie. See Exporter::export.
    def export(*args, &block)
      exporter.export(*args, &block)
    end
  end
end
