$:.unshift(File.dirname(__FILE__))

require File.dirname(__FILE__) + '/../ext/rmov_ext'

require 'quicktime/movie'
require 'quicktime/track'
require 'quicktime/exporter'


# RMov is made up of several parts. To start, see Quicktime::Movie.
module Quicktime
end
