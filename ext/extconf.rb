ENV["ARCHFLAGS"] = "-arch i386"
require "mkmf"

$LDFLAGS = "-framework QuickTime"
create_makefile("rmov_ext")
