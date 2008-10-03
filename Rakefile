require 'rubygems'
require 'rake'
require 'echoe'

Echoe.new('rmov', '0.1.4') do |p|
  p.summary        = "Ruby wrapper for the QuickTime C API."
  p.description    = "Ruby wrapper for the QuickTime C API."
  p.url            = "http://github.com/ryanb/rmov"
  p.author         = 'Ryan Bates'
  p.email          = "ryan (at) railscasts (dot) com"
  p.ignore_pattern = ["script/*", "tmp/*", "spec/output/*", "**/*.o", "**/*.bundle", "**/*.mov"]
  p.extensions     = ["ext/extconf.rb"]
  p.development_dependencies = []
end

Dir["#{File.dirname(__FILE__)}/tasks/*.rake"].sort.each { |ext| load ext }
