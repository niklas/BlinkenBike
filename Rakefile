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


effects = Rake::FileList.new('effects/*.effect', 'lib/Effects/*dna', 'gen/lib/*.rb', 'lib/Animations/*dna')
['lib/Effects/Effects.h', 'lib/Effects/Effects.cpp'].each do |f|
  file f => effects do
    sh "bin/ribosome #{f}.rb.dna > #{f}"
  end
  if ENV['EFFECTS']
    Rake::Task[f].execute
  end
end

transitions = Rake::FileList.new('transitions/*.transition', 'lib/Transitions/*dna', 'gen/lib/*.rb', 'lib/Animations/*dna')
['lib/Transitions/Transitions.h', 'lib/Transitions/Transitions.cpp'].each do |f|
  file f => transitions do
    sh "bin/ribosome #{f}.rb.dna > #{f}"
  end
  if ENV['TRANSITIONS']
    Rake::Task[f].execute
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
    sh %Q~nm #{firmware} -C -l -S -td --size-sort | egrep -v ' [tTvVwWaA] ' | column -t~
  end
  task :flash => firmware do
    sh %Q~nm #{firmware} -C -l -S -td --size-sort | egrep -v ' [bBdDgG] '~
  end
end
