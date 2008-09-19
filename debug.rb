# run like this:
#   gdb ruby
#   r debug.rb
#   bt
require 'lib/rmov'
m = Quicktime::Movie.open('spec/fixtures/example.mov')
puts m.duration
puts m.width
puts m.height
puts m.tracks.size
