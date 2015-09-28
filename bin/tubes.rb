#!/usr/bin/env ruby

require 'rubygems'
require 'ostruct'

class Frame
  def tubes
    @tubes ||= []
  end
end

class Tube < OpenStruct
end

if $0 == __FILE__

  frame = Frame.new

  # order of adding is the order of the LED strip
  frame.tubes << Tube.new(name: 'top tube'     , from: [  20,  20], to: [  50,  50], leds: 16)
  frame.tubes << Tube.new(name: 'down tube'    , from: [  20,  20], to: [  50,  50], leds: 16)
  frame.tubes << Tube.new(name: 'seat tube'    , from: [  20,  20], to: [  50,  50], leds: 16)
  frame.tubes << Tube.new(name: 'chain left'   , from: [  20,  20], to: [  50,  50], leds: 16)
  frame.tubes << Tube.new(name: 'chain right'  , from: [  20,  20], to: [  50,  50], leds: 16)
  frame.tubes << Tube.new(name: 'seat left'    , from: [  20,  20], to: [  50,  50], leds: 16)
  frame.tubes << Tube.new(name: 'seat right'   , from: [  20,  20], to: [  50,  50], leds: 16)

end
