desc "Builds the rmov_ext extension"
task :setup do
  Dir.chdir('ext') do
    ruby 'extconf.rb'
    system 'make'
  end
end
