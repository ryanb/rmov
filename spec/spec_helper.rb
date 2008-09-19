require 'rubygems'
require 'spec'
require File.dirname(__FILE__) + '/../lib/rmov'

Spec::Runner.configure do |config|
  config.mock_with :mocha
end
