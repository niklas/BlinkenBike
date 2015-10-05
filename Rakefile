task :default => :all

task :all => [
  'src/effects.h',
  'src/transitions.h',
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
