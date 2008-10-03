require 'mkmf'

$LDFLAGS = '-framework QuickTime'
create_makefile('rmov_ext')
