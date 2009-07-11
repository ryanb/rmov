require File.dirname(__FILE__) + '/../spec_helper.rb'

describe QuickTime::Movie do
  it "should raise an exception when attempting to open a nonexisting file" do
    lambda { QuickTime::Movie.open('foo.mov') }.should raise_error(QuickTime::Error)
  end
  
  it "should raise an exception when attempting to open a non movie file" do
    lambda { QuickTime::Movie.open(__FILE__) }.should raise_error(QuickTime::Error)
  end
  
  describe "example.mov" do
    before(:each) do
      @movie = QuickTime::Movie.open(File.dirname(__FILE__) + '/../fixtures/example.mov')
    end
    
    it "duration should be 3.1 seconds" do
      @movie.duration.should == 3.1
    end
    
    it "bounds should be a hash of top, left, bottom, and right points" do
      @movie.bounds.should == { :top => 0, :left => 0, :bottom => 50, :right => 60 }
    end
    
    it "width should be 60" do
      @movie.width.should == 60
    end
    
    it "height should be 50" do
      @movie.height.should == 50
    end
    
    it "should have 2 tracks" do
      @movie.tracks.map { |t| t.class }.should == [QuickTime::Track, QuickTime::Track]
      @movie.tracks.map { |t| t.id }.should == [1, 2]
    end
    
    it "should be able to export into separate file" do
      path = File.dirname(__FILE__) + '/../output/exported_example.mov'
      File.delete(path) rescue nil
      
      progress = 0
      @movie.export(path) { |p| progress = p }
      progress.should == 1.0
      
      exported_movie = QuickTime::Movie.open(path)
      exported_movie.duration.should == @movie.duration
      exported_movie.tracks.size == @movie.tracks.size
    end
    
    it "should have one audio track" do
      @movie.audio_tracks.should have(1).record
    end
    
    it "should have one video track" do
      @movie.video_tracks.should have(1).record
    end
    
    it "composite should add another movie's tracks at a given location" do
      m2 = QuickTime::Movie.open(File.dirname(__FILE__) + '/../fixtures/example.mov')
      @movie.composite_movie(m2, 2)
      @movie.duration.should == 5.1
    end
    
    it "insert should insert another movie's tracks at a given location" do
      m2 = QuickTime::Movie.open(File.dirname(__FILE__) + '/../fixtures/example.mov')
      @movie.insert_movie(m2, 2)
      @movie.duration.should == 6.2
    end
    
    it "append_movie should insert movie at the end" do
      m2 = QuickTime::Movie.open(File.dirname(__FILE__) + '/../fixtures/example.mov')
      @movie.append_movie(m2)
      @movie.duration.should == 6.2
    end
    
    it "delete_section should remove a section from a movie" do
      @movie.delete_section(1, 0.6)
      @movie.duration.should == 2.5
    end
    
    it "clone_section should make a new movie from given section and leave existing movie intact" do
      mov = @movie.clone_section(1, 0.6)
      mov.duration.should == 0.6
      @movie.duration.should == 3.1
    end
    
    it "clip_section should make a new movie from given section and remove it form existing movie" do
      mov = @movie.clip_section(1, 0.6)
      mov.duration.should == 0.6
      @movie.duration.should == 2.5
    end
    
    it "should have an exporter with this movie" do
      exporter = @movie.exporter
      exporter.should be_kind_of(QuickTime::Exporter)
      exporter.movie.should == @movie
    end
    
    it "should say when movie has changed" do
      @movie.should_not be_changed
      @movie.delete_section(1, 0.6)
      @movie.should be_changed
    end
    
    it "should be able to clear changed status" do
      @movie.delete_section(1, 0.6)
      @movie.clear_changed_status
      @movie.should_not be_changed
    end
    
    it "flatten should save movie into file" do
      path = File.dirname(__FILE__) + '/../output/flattened_example.mov'
      File.delete(path) if File.exist?(path)
      @movie.flatten(path)
      mov = QuickTime::Movie.open(path)
      mov.duration.should == 3.1
    end
    
    it "save should update movie in current file" do
      path = File.dirname(__FILE__) + '/../output/saved_example.mov'
      File.delete(path) if File.exist?(path)
      @movie.flatten(path)
      mov = QuickTime::Movie.open(path)
      mov.audio_tracks.each { |t| t.delete } # delete track to demonstrate change
      mov.save
      mov2 = QuickTime::Movie.open(path)
      mov2.audio_tracks.should be_empty
    end
    
    it "save should raise exception when saving new movie without filepath" do
      mov = QuickTime::Movie.empty
      lambda { mov.save }.should raise_error
    end
    
    it "export_pict should output a pict file at a given duration" do
      path = File.dirname(__FILE__) + '/../output/example.pct'
      File.delete(path) rescue nil
      @movie.export_image(path, 1.2)
    end
    
    it "export_png should output a png file at a given duration" do
      path = File.dirname(__FILE__) + '/../output/example.png'
      File.delete(path) rescue nil
      @movie.export_image(path, 1.2)
    end
    
    it "should default poster time to 0" do
      @movie.poster_time.should == 0
    end
    
    it "should be able to set poster time  to 2.1 seconds in" do
      @movie.poster_time = 2.1
      @movie.poster_time.should == 2.1
    end
    
    it "should overlay 2nd movie with transparency" do
      m2 = QuickTime::Movie.open(File.dirname(__FILE__) + '/../fixtures/dot.png')
      @movie.composite_movie(m2, 0)
      @movie.video_tracks.last.enable_alpha
      File.delete(File.dirname(__FILE__) + '/../output/transparent.mov') rescue nil
      @movie.flatten(File.dirname(__FILE__) + '/../output/transparent.mov')
      # this test needs to be checked manually by looking at the output movie
    end
  end
  
  describe "empty movie" do
    before(:each) do
      @movie = QuickTime::Movie.empty
    end
    
    it "should have 0 duration" do
      @movie.duration.should == 0
    end
    
    it "should be able to append an existing movie" do
      m2 = QuickTime::Movie.open(File.dirname(__FILE__) + '/../fixtures/example.mov')
      @movie.append_movie(m2)
      @movie.duration.should == 3.1
    end
    
    it "should raise MovieAlreadyLoaded exception when attempting to load it again" do
      lambda { @movie.load_empty }.should raise_error(QuickTime::Error)
      lambda { @movie.load_from_file('example.mov') }.should raise_error(QuickTime::Error)
    end
    
    it "should be able to create a new track" do
      track = @movie.new_track(300, 500)
      track.should be_kind_of(QuickTime::Track)
      @movie.tracks.should have(1).record
    end
    
    it "should be able to create a new video track" do
      track = @movie.new_video_track(300, 500)
      track.should be_kind_of(QuickTime::Track)
      track.should be_video
      @movie.video_tracks.should have(1).record
    end
    
    it "should be able to create a new audio track" do
      track = @movie.new_audio_track(300, 500)
      track.should be_kind_of(QuickTime::Track)
      track.should be_audio
      @movie.audio_tracks.should have(1).record
    end
    
    it "should be able to create a new text track" do
      track = @movie.new_text_track(300, 500)
      track.should be_kind_of(QuickTime::Track)
      track.should be_text
      @movie.text_tracks.should have(1).record
    end
  end
end
