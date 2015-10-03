#!/usr/bin/env ruby

$:.unshift File.join(__FILE__, '../../gen/lib')

require 'effects'
require 'transitions'

if $0 == __FILE__
  effects = Effects.glob('effects/*.effect')
  File.open 'src/effects.h', 'w' do |h|
    h.puts effects.header
  end
  File.open 'src/effects.cpp', 'w' do |cpp|
    cpp.puts effects.implementation
  end

  alphas = Transitions.glob('transitions/*.transition')
  File.open 'src/transitions.h', 'w' do |h|
    h.puts alphas.header
  end
  File.open 'src/transitions.cpp', 'w' do |cpp|
    cpp.puts alphas.implementation
  end
else
  raise "run as #{$0}"
end
