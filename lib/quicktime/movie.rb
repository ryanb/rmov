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
      type = case File.extname(filepath).downcase
        when '.pct', '.pict' then 'PICT'
        when '.tif', '.tiff' then 'TIFF'
        when '.jpg', '.jpeg' then 'JPEG'
        when '.png'          then 'PNGf'
        when '.tga'          then 'TPIC'
        when '.bmp'          then 'BMPf'
        when '.psd'          then '8BPS'
        else raise QuickTime::Error, "Unable to guess ostype from file extension of #{filepath}"
      end
      export_image_type(filepath, seconds, type)
    end
    
    # Reset selection to beginning
    def deselect
      select(0, 0)
    end
    
    # Adds the tracks of given movie into called movie. Position will default to
    # beginning of movie. Duration will default to length of given movie.
    # 
    # You can track the progress of this operation by passing a block to this 
    # method. It will be called regularly during the process and pass the 
    # percentage complete (0.0 to 1.0) as an argument to the block.
    def composite_movie(movie, position = 0, duration = 0, &block)
      select(position, duration)
      add_into_selection(movie, &block)
      deselect
    end
    
    # Adds given movie to the end of movie which this method is called on.
    # 
    # You can track the progress of this operation by passing a block to this 
    # method. It will be called regularly during the process and pass the 
    # percentage complete (0.0 to 1.0) as an argument to the block.
    def append_movie(movie, &block)
      select(duration, 0)
      insert_into_selection(movie, &block)
      deselect
    end
    
    # Inserts given movie into called movie. The position defaults to the beginning
    # of the movie. If a duration is passed, that amount of the movie will be replaced.
    # 
    # You can track the progress of this operation by passing a block to this 
    # method. It will be called regularly during the process and pass the 
    # percentage complete (0.0 to 1.0) as an argument to the block.
    def insert_movie(movie, position = 0, duration = 0, &block)
      select(position, duration)
      insert_into_selection(movie, &block)
      deselect
    end
    
    # Returns a new movie from the specified portion of called movie.
    # 
    # You can track the progress of this operation by passing a block to this 
    # method. It will be called regularly during the process and pass the 
    # percentage complete (0.0 to 1.0) as an argument to the block.
    def clone_section(position = 0, duration = 0, &block)
      select(position, duration)
      movie = clone_selection(&block)
      deselect
      movie
    end
    
    # Deletes the specified section on movie and returns a new movie
    # with that content.
    # 
    # You can track the progress of this operation by passing a block to this 
    # method. It will be called regularly during the process and pass the 
    # percentage complete (0.0 to 1.0) as an argument to the block.
    def clip_section(position = 0, duration = 0, &block)
      select(position, duration)
      movie = clip_selection(&block)
      deselect
      movie
    end
    
    # Deletes the specified section on movie.
    def delete_section(position = 0, duration = 0)
      select(position, duration)
      delete_selection
      deselect
    end
  end
end
