require File.dirname(__FILE__) + '/../spec_helper.rb'

describe Quicktime::Exporter do
  it "should raise error when saving with no settings" do
    lambda { Quicktime::Exporter.new(nil).save_settings('foo') }.should raise_error(Quicktime::Error)
  end
  
  it "should raise error when atempting to load no file" do
    lambda { Quicktime::Exporter.new(nil).load_settings('foo/bar/baz') }.should raise_error(Quicktime::Error)
  end
  
  describe "loaded settings.st" do
    before(:each) do
      @load_path = File.dirname(__FILE__) + '/../fixtures/settings.st'
      @exporter = Quicktime::Exporter.new(nil)
      @exporter.load_settings(@load_path)
    end
    
    it "should be able to save settings to file" do
      save_path = File.dirname(__FILE__) + '/../output/saved_settings.st'
      @exporter.save_settings(save_path)
      File.size(save_path).should == File.size(@load_path)
    end
    
    it "should complain when attempting to save to an invalid file" do
      lambda { @exporter.save_settings('foo/bar/baz') }.should raise_error(Quicktime::Error)
    end
  end
end
