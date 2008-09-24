require File.dirname(__FILE__) + '/../spec_helper.rb'

describe Quicktime::Exporter do
  it "should be able to load and save settings" do
    in_path = File.dirname(__FILE__) + '/../fixtures/settings.st'
    out_path = File.dirname(__FILE__) + '/../output/saved_settings.st'
    
    exporter = Quicktime::Exporter.new(nil)
    exporter.load_settings(in_path)
    exporter.save_settings(out_path)
    
    File.size(out_path).should == File.size(in_path)
  end
end
