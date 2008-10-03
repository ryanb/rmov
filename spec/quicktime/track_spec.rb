require File.dirname(__FILE__) + '/../spec_helper.rb'

describe Quicktime::Track do
  describe "example.mov" do
    before(:each) do
      @movie = Quicktime::Movie.open(File.dirname(__FILE__) + '/../fixtures/example.mov')
    end
  
    describe "example.mov video track" do
      before(:each) do
        @track = @movie.video_tracks.first
      end
    
      it "duration should be 3.1 seconds" do
        @track.duration.should == 3.1
      end
    
      it "frame count should be 31" do
        @track.frame_count.should == 31
      end
    
      it "frame rate should be 10" do
        @track.frame_rate.should == 10
      end
    
      it "should be able to delete a track" do
        @track.delete
        @movie.video_tracks.should == []
      end
    
      it "should be able to add a track" do
        @track.delete
        @movie.video_tracks.should == []
      end
    
      it "should be able to disable and enable a track" do
        @track.should be_enabled
        @track.disable
        @track.should_not be_enabled
        @track.enable
        @track.should be_enabled
      end
    end
  
    describe "example.mov audio track" do
      before(:each) do
        @track = @movie.audio_tracks.first
      end
    
      it "should have a volume of 1.0" do
        @track.volume.should == 1.0
      end
    
      it "should be able to set volume to 0.5" do
        @track.volume = 0.5
        @track.volume.should == 0.5
      end
    end
  end
end
