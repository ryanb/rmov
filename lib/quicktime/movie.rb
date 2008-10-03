module QuickTime
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
    
    # Returns an array of text tracks in this movie.
    def text_tracks
      tracks.select { |t| t.text? }
    end
    
    # Returns an Exporter instance for this movie.
    def exporter
      Exporter.new(self)
    end
    
    # Convenience method for exporting the movie. See Exporter::export.
    def export(*args, &block)
      exporter.export(*args, &block)
    end
    
    # Creates a new video track with given width/height on movie and returns it.
    def new_video_track(width, height)
      track = new_track(width, height)
      track.new_video_media
      track
    end
    
    # Creates a new audio track with given width/height on movie and returns it.
    def new_audio_track(width, height)
      track = new_track(width, height)
      track.new_audio_media
      track
    end
    
    # Creates a new text track with given width/height on movie and returns it.
    def new_text_track(width, height)
      track = new_track(width, height)
      track.new_text_media
      track
    end
    
    # Exports a frame of the movie at the given time (in seconds) to the given file. 
    # The image format is automatically determined from the file extension. If this
    # cannot be determined from the extension then you can use export_image_type to
    # specify the ostype manually.
    def export_image(filepath, seconds)
      # TODO support more file types
      type = case File.extname(filepath)
        when '.pct' then 'PICT'
        when '.png' then 'PNGf'
        else raise QuickTime::Error, "Unable to guess ostype from file extension of #{filepath}"
      end
      export_image_type(filepath, seconds, type)
    end
  end
end
