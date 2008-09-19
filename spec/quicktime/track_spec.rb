require File.dirname(__FILE__) + '/../spec_helper.rb'

describe Quicktime::Track do
  describe "example.mov" do
    before(:each) do
      @movie = Quicktime::Movie.new(File.dirname(__FILE__) + '/../fixtures/example.mov')
      @track = @movie.tracks.first
    end
    
    it "duration should be 3.1 seconds" do
      @track.duration.should == 3.1
    end
  end
end
