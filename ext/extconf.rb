require 'mkmf'

$LDFLAGS = '-framework Quicktime'
create_makefile('rmov')
