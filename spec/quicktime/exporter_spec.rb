require File.dirname(__FILE__) + '/../spec_helper.rb'

describe QuickTime::Exporter do
  it "should raise error when saving with no settings" do
    lambda { QuickTime::Exporter.new(nil).save_settings('foo') }.should raise_error(QuickTime::Error)
  end
  
  it "should raise error when atempting to load no file" do
    lambda { QuickTime::Exporter.new(nil).load_settings('foo/bar/baz') }.should raise_error(QuickTime::Error)
  end
  
  describe "loaded settings.st" do
    before(:each) do
      @load_path = File.dirname(__FILE__) + '/../fixtures/settings.st'
      @exporter = QuickTime::Exporter.new(nil)
      @exporter.load_settings(@load_path)
    end
    
    it "should be able to save settings to file" do
      save_path = File.dirname(__FILE__) + '/../output/saved_settings.st'
      @exporter.save_settings(save_path)
      File.size(save_path).should == File.size(@load_path)
    end
    
    it "should complain when attempting to save to an invalid file" do
      lambda { @exporter.save_settings('foo/bar/baz') }.should raise_error(QuickTime::Error)
    end
  end
  describe "example.mov" do
    before(:each) do
      @movie = QuickTime::Movie.open(File.dirname(__FILE__) + '/../fixtures/example.mov')
      @exporter = @movie.exporter
    end
    
    it "should be able to export from loaded settings.st" do
      load_path = File.dirname(__FILE__) + '/../fixtures/settings.st'
      path = File.dirname(__FILE__) + '/../output/loaded_exported_example.mov'
      File.delete(path) rescue nil
      
      @exporter.load_settings(load_path)
      @exporter.export(path)
      exported_movie = QuickTime::Movie.open(path)
      exported_movie.duration.should == @movie.duration
      exported_movie.tracks.size == @movie.tracks.size
    end
  end
end
