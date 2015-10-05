task :default => :all

task :all => [
  'src/effects.h',
  'src/transitions.h',
  'src/layout.h',
]
generator = 'bin/generate_code.rb'

effects = Rake::FileList.new('effects/*.effect', 'bin/generate_effects')
file 'src/effects.h' => effects do
  sh "bin/generate_effects"
end

transitions = Rake::FileList.new('transitions/*.transition', 'bin/generate_transitions')
file 'src/transitions.h' => transitions do
  sh "bin/generate_transitions"
end

['src/layout.h', 'src/layout.cpp'].each do |layout|
  file layout => ['bicycle.yaml', 'bin/generate_layout', 'gen/lib/tubes.rb', 'src/layout.cpp.erb'] do
    sh 'bin/generate_layout'
  end
end
