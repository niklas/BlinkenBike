task :default => :all

task :all => [
  'lib/Effects/Effects.h',
  'lib/Transitions/Transitions.h',
  'lib/Layout/Layout.h',
  'lib/Effects/Effects.cpp',
  'lib/Transitions/Transitions.cpp',
  'lib/Layout/Layout.cpp',
]
generator = 'bin/generate_code.rb'


if ENV['EFFECTS']
  touch 'bin/generate_effects'
end
effects = Rake::FileList.new('effects/*.effect', 'bin/generate_effects', 'gen/lib/*.rb')
['lib/Effects/Effects.h', 'lib/Effects/Effects.cpp'].each do |f|
  file f => effects do
    mkdir_p 'lib/Effects'
    sh "bin/generate_effects"
  end
end

if ENV['TRANSITIONS']
  touch 'bin/generate_transitions'
end
transitions = Rake::FileList.new('transitions/*.transition', 'bin/generate_transitions', 'gen/lib/*.rb')
['lib/Transitions/Transitions.h', 'lib/Transitions/Transitions.cpp'].each do |f|
  file f => transitions do
    mkdir_p 'lib/Transitions'
    sh "bin/generate_transitions"
  end
end

['lib/Layout/Layout.h', 'lib/Layout/Layout.cpp'].each do |layout|
  file layout => ['bicycle.yaml', 'bin/generate_layout', 'gen/lib/tubes.rb', 'lib/Layout/Layout.cpp.erb', 'lib/Layout/Layout.h.erb'] do
    sh 'bin/generate_layout'
  end
end

task :simulate do
  sh 'processing-java --sketch=simulation/fire --run'
end

firmware = '.pioenvs/diecimilaatmega168/firmware.elf'
namespace :analyze do
  task :sram => firmware do
    sh %Q~nm #{firmware} -C -l -S -td --size-sort | egrep -v ' [tTvVwWaA] | column -t'~
  end
end
