#!/usr/bin/env ruby

require 'ostruct'
require 'fileutils'

require 'rubygems'
require 'rmagick'

class Frame
  def initialize(image_path)
    @image_path = image_path
  end

  def tubes
    @tubes ||= []
  end

  def write(target_path)
    image = Magick::Image.read(@image_path)[0]
    draw.draw(image)
    FileUtils.mkdir_p File.dirname(target_path)
    image.write(target_path)
    $stderr.puts "written #{target_path}"
  end

  private

  def draw
    Magick::Draw.new.tap do |pen|
      tubes.each do |tube|
        # the frame
        pen.stroke('#76BA0B')
        pen.stroke_width 10
        pen.stroke_opacity 0.7
        pen.line *tube.from, *tube.to
      end
    end
  end
end

class Tube < OpenStruct
end

if $0 == __FILE__

  frame = Frame.new 'assets/kalkhoff.jpeg'

  # order of adding is the order of the LED strip
  frame.tubes << Tube.new(name: 'down tube'    , from: [ 924, 604], to: [1667,1515], leds: 20)
  frame.tubes << Tube.new(name: 'chain left'   , from: [1790,1546], to: [2600,1474], leds: 14)
  frame.tubes << Tube.new(name: 'seat left'    , from: [2606,1377], to: [2066, 717], leds: 16)
  frame.tubes << Tube.new(name: 'seat tube'    , from: [2006, 692], to: [1776,1384], leds: 14)
  frame.tubes << Tube.new(name: 'chain right'  , from: [1790,1546], to: [2600,1474], leds: 14)
  frame.tubes << Tube.new(name: 'seat right'   , from: [2606,1377], to: [2066, 717], leds: 16)
  frame.tubes << Tube.new(name: 'top tube'     , from: [1996, 594], to: [1011, 360], leds: 16)

  frame.write 'doc/kalkhoff.jpeg'

end
