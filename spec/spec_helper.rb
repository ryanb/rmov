require 'rubygems'
require 'spec'
require File.dirname(__FILE__) + '/../ext/rmov'

Spec::Runner.configure do |config|
  config.mock_with :mocha
end
