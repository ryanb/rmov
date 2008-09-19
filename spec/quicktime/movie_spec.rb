require File.dirname(__FILE__) + '/../spec_helper.rb'

describe Quicktime::Movie do
  describe "example.mov" do
    before(:each) do
      @movie = Quicktime::Movie.new(File.dirname(__FILE__) + '/../fixtures/example.mov')
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
  end
end
