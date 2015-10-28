task :default => :all

task :all => [
  'lib/Effects/Effects.h',
  'lib/Transitions/Transitions.h',
  'lib/Layout/Layout.h',
  'lib/Effects/Effects.cpp',
  'lib/Transitions/Transitions.cpp',
  'lib/Layout/Layout.cpp',
]
effects     = Rake::FileList.glob('effects/*.effect')
transitions = Rake::FileList.glob('transitions/*.transition')
animations  = effects + transitions

sourcefiles = Rake::FileList.glob('{lib,src}/**/*.{cpp,h}') + animations

generator = 'bin/generate_code.rb'


effects_deps = effects + Rake::FileList.new('lib/Effects/*dna', 'gen/lib/*.rb', 'lib/Animations/*dna')
['lib/Effects/Effects.h', 'lib/Effects/Effects.cpp'].each do |f|
  file f => effects_deps do
    sh "bin/ribosome #{f}.rb.dna > #{f}"
  end
  if ENV['EFFECTS']
    Rake::Task[f].execute
  end
end

transitions_deps = transitions + Rake::FileList.new('lib/Transitions/*dna', 'gen/lib/*.rb', 'lib/Animations/*dna')
['lib/Transitions/Transitions.h', 'lib/Transitions/Transitions.cpp'].each do |f|
  file f => transitions_deps do
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
task :firmware => firmware

tmps = {
  firmware:      'tmp/firmware.log',
  analyze_sram:  'tmp/analyze_sram.log',
  analyze_flash: 'tmp/analyze_flash.log',
}

file firmware => effects_deps + transitions_deps + sourcefiles + [tmps[:firmware]] do
  sh %Q~platformio run | tee #{tmps[:firmware]}~
end

max_prog_size = 14336
namespace :analyze do
  task :sram => firmware do
    sh %Q~nm #{firmware} -C -l -S -td --size-sort | egrep -v ' [tTvVwWaA] ' | column -t~
  end
  task :flash => firmware do
    sh %Q~nm #{firmware} -C -l -S -td --size-sort | egrep -v ' [bBdDgG] ' | tee #{tmps[:analyze_flash]}~
    if `grep ^Program: #{tmps[:firmware]}` =~ /(\d+) bytes/
      prgsize = $1.to_i

      if prgsize > max_prog_size
        $stderr.puts
        $stderr.puts "WARNING: Program is too big: #{prgsize} bytes (#{prgsize - max_prog_size} too much)!"
        $stderr.puts
      end
    end
  end
end
