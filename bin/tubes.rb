#!/usr/bin/env ruby

require 'ostruct'
require 'fileutils'

require 'rubygems'
require 'rmagick'
require 'pry'

class Frame < OpenStruct
  TubeWidth = 8
  LedSize = 14

  def tubes
    @tubes ||= []
  end

  def write(target_path)
    image = Magick::Image.read(image_path)[0]
    annotations.draw(image)
    FileUtils.mkdir_p File.dirname(target_path)
    image.write(target_path)
    $stderr.puts "written #{target_path}"
  end

  private

  def annotations
    Magick::Draw.new.tap do |pen|
      tubes.each do |tube|
        # the frame
        pen.stroke('#76BA0B')
        pen.stroke_width TubeWidth
        pen.stroke_opacity 0.7
        pen.line *tube.from, *tube.to

        pen.stroke_width TubeWidth / 3
        pen.stroke('white')
        tube.leds.each do |led|
          annotate_led led, pen
        end
      end

      pen.stroke('blue')
      floor_tube.leds.each do |led|
        annotate_led led, pen
      end
    end
  end

  def annotate_led(led, pen)
    pen.circle led.x, led.y, led.x, led.y - LedSize
  end

  def all_leds
    tubes.map(&:leds).flatten
  end

  def floor_tube
    xs = all_leds.map(&:x)
    @floor_tube ||= Tube.new(
      name: 'floor',
      from: [ xs.min, floor ],
      to:   [ xs.max, floor ],
      led_count: 50
    )
  end
end

class Tube < OpenStruct
  class Led < Struct.new(:x, :y)
  end

  def leds
    return @leds if defined?(@leds)
    length = (to_x - from_x).to_f
    height = (to_y - from_y).to_f
    m = height / length
    n = from_y - (m*from_x)
    @leds = (1..led_count).to_a.map do |i|
      x = from_x + (i - 0.5) * (length / led_count)
      y = m * x + n
      Led.new x, y
    end
  end

  def from_x; from[0]; end
  def from_y; from[1]; end
  def to_x; to[0]; end
  def to_y; to[1]; end
end

if $0 == __FILE__

  frame = Frame.new image_path: 'assets/kalkhoff.jpeg', floor: 1981

  # order of adding is the order of the LED strip
  frame.tubes << Tube.new(name: 'down tube'    , from: [ 924, 604], to: [1667,1515], led_count: 20)
  frame.tubes << Tube.new(name: 'chain left'   , from: [1790,1546], to: [2600,1474], led_count: 14)
  frame.tubes << Tube.new(name: 'seat left'    , from: [2606,1377], to: [2066, 717], led_count: 16)
  frame.tubes << Tube.new(name: 'seat tube'    , from: [2006, 692], to: [1776,1384], led_count: 14)
  frame.tubes << Tube.new(name: 'chain right'  , from: [1790,1546], to: [2600,1474], led_count: 14)
  frame.tubes << Tube.new(name: 'seat right'   , from: [2606,1377], to: [2066, 717], led_count: 16)
  frame.tubes << Tube.new(name: 'top tube'     , from: [1996, 594], to: [1011, 360], led_count: 16)

  frame.write 'doc/kalkhoff.jpeg'

end
