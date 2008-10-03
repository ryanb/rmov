require File.dirname(__FILE__) + '/../spec_helper.rb'

describe Quicktime::Movie do
  it "should raise an exception when attempting to open a nonexisting file" do
    lambda { Quicktime::Movie.open('foo.mov') }.should raise_error(Quicktime::Error)
  end
  
  it "should raise an exception when attempting to open a non movie file" do
    lambda { Quicktime::Movie.open(__FILE__) }.should raise_error(Quicktime::Error)
  end
  
  describe "example.mov" do
    before(:each) do
      @movie = Quicktime::Movie.open(File.dirname(__FILE__) + '/../fixtures/example.mov')
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
      @movie.tracks.map { |t| t.class }.should == [Quicktime::Track, Quicktime::Track]
      @movie.tracks.map { |t| t.id }.should == [1, 2]
    end
    
    it "should be able to export into separate file" do
      path = File.dirname(__FILE__) + '/../output/exported_example.mov'
      File.delete(path) rescue nil
      
      progress = 0
      @movie.export(path) { |p| progress = p }
      progress.should == 1.0
      
      exported_movie = Quicktime::Movie.open(path)
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
      m2 = Quicktime::Movie.open(File.dirname(__FILE__) + '/../fixtures/example.mov')
      @movie.composite_movie(m2, 2)
      @movie.duration.should == 5.1
    end
    
    it "insert should insert another movie's tracks at a given location" do
      m2 = Quicktime::Movie.open(File.dirname(__FILE__) + '/../fixtures/example.mov')
      @movie.insert_movie(m2, 2)
      @movie.duration.should == 6.2
    end
    
    it "append_movie should insert movie at the end" do
      m2 = Quicktime::Movie.open(File.dirname(__FILE__) + '/../fixtures/example.mov')
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
      exporter.should be_kind_of(Quicktime::Exporter)
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
      File.delete(path) rescue nil
      @movie.flatten(path)
      mov = Quicktime::Movie.open(path)
      mov.duration.should == 3.1
    end
    
    it "export_pict should output a pict file at a given duration" do
      path = File.dirname(__FILE__) + '/../output/example.pct'
      File.delete(path) rescue nil
      @movie.export_pict(path, 1.2)
    end
    
    it "should default poster time to 0" do
      @movie.poster_time.should == 0
    end
    
    it "should be able to set poster time  to 2.1 seconds in" do
      @movie.poster_time = 2.1
      @movie.poster_time.should == 2.1
    end
  end
  
  describe "empty movie" do
    before(:each) do
      @movie = Quicktime::Movie.empty
    end
    
    it "should have 0 duration" do
      @movie.duration.should == 0
    end
    
    it "should be able to append an existing movie" do
      m2 = Quicktime::Movie.open(File.dirname(__FILE__) + '/../fixtures/example.mov')
      @movie.append_movie(m2)
      @movie.duration.should == 3.1
    end
    
    it "should raise MovieAlreadyLoaded exception when attempting to load it again" do
      lambda { @movie.load_empty }.should raise_error(Quicktime::Error)
      lambda { @movie.load_from_file('example.mov') }.should raise_error(Quicktime::Error)
    end
  end
end
