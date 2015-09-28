#!/usr/bin/env ruby

require 'ostruct'
require 'fileutils'

require 'rubygems'
require 'mini_magick'

class Frame
  def initialize(image_path)
    @image_path = image_path
  end

  def tubes
    @tubes ||= []
  end

  def write(target_path)
    image = MiniMagick::Image.open(@image_path)
    draw(image)
    FileUtils.mkdir_p File.dirname(target_path)
    image.write(target_path)
    $stderr.puts "written #{target_path}"
  end

  private

  def draw(image)
    # TODO
  end
end

class Tube < OpenStruct
end

if $0 == __FILE__

  frame = Frame.new 'assets/kalkhoff.jpeg'

  # order of adding is the order of the LED strip
  frame.tubes << Tube.new(name: 'top tube'     , from: [  20,  20], to: [  50,  50], leds: 16)
  frame.tubes << Tube.new(name: 'down tube'    , from: [  20,  20], to: [  50,  50], leds: 16)
  frame.tubes << Tube.new(name: 'seat tube'    , from: [  20,  20], to: [  50,  50], leds: 16)
  frame.tubes << Tube.new(name: 'chain left'   , from: [  20,  20], to: [  50,  50], leds: 16)
  frame.tubes << Tube.new(name: 'chain right'  , from: [  20,  20], to: [  50,  50], leds: 16)
  frame.tubes << Tube.new(name: 'seat left'    , from: [  20,  20], to: [  50,  50], leds: 16)
  frame.tubes << Tube.new(name: 'seat right'   , from: [  20,  20], to: [  50,  50], leds: 16)


  frame.write 'doc/kalkhoff.jpeg'

end
