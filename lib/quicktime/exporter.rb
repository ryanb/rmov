module Quicktime
  # see ext/exporter.c for additional methods
  class Exporter
    attr_reader :movie
    
    def initialize(movie)
      @movie = movie
    end
  end
end
