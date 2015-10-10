require 'ostruct'
require 'fileutils'

require 'rubygems'
require 'rmagick'
require 'pry'
require 'yaml'
require 'erb'

class Frame < OpenStruct
  TubeWidth = 8
  LedSize = 14

  def tubes
    @tubes ||= []
  end

  def write_image(target_path)
    calculate_floor_connections!
    image = Magick::Image.read(image_path)[0]
    annotations.draw(image)
    FileUtils.mkdir_p File.dirname(target_path)
    image.write(target_path)
    $stderr.puts "written #{target_path}"
  end

  def header
    calculate_floor_connections!

    ERB.new(File.read(header_template)).result(binding)
  end

  def header_template
    implementation_template.sub(/\.cpp\./, '.h.')
  end

  def implementation
    calculate_floor_connections!

    ERB.new(File.read(implementation_template)).result(binding)
  end

  private

  def annotations
    Magick::Draw.new.tap do |pen|
      pen.stroke_width 4
      pen.stroke('yellow')
      pen.stroke_opacity 0.3
      all_leds.each do |led|
        pen.line led.x, led.y, led.floor.x, led.floor.y
      end

      tubes.each do |tube|
        # the frame
        pen.stroke('#76BA0B')
        pen.stroke_width TubeWidth
        pen.stroke_opacity 0.7
        pen.line *tube.from, *tube.to
      end

      pen.fill('none')
      all_leds.each_cons(2) do |a,b|
        ab = [a,b]
        mx, my = ab.map(&:x).min, ab.map(&:y).max
        pen.stroke(Magick::Pixel.from_hsla((5*a.index) % 360, 255, 127).to_color)
        pen.bezier a.x, a.y,
          a.x + rand(-25..25), b.y + rand(-25..25),
          b.x + rand(-25..25), a.y + rand(-25..25),
          b.x, b.y
      end

      all_leds.each do |led|
        annotate_led led, 'white', pen
      end

      pen.stroke('blue')
      floor_tube.leds.each do |led|
        col = led.connections.empty?? 'red' : 'white'
        annotate_led led, col, pen
      end
    end
  end

  def annotate_led(led, color, pen)
    pen.stroke_width TubeWidth / 3
    pen.stroke('black')
    pen.fill(color)
    pen.circle led.x, led.y, led.x, led.y - LedSize

    pen.pointsize 23
    pen.stroke_width 1
    pen.text_antialias true
    pen.text led.x, led.y + (LedSize * 1.8), led.index.to_s
  end

  def all_leds
    tubes.map(&:leds).flatten
  end

  def floor_tube
    return @floor_tube if defined?(@floor_tube)
    xs = all_leds.map(&:x)
    @floor_tube = Tube.new(
      name: 'floor',
      from: [ xs.min, floor ],
      to:   [ xs.max, floor ],
      virtual: true,
      led_count: 20
    )
  end

  def calculate_floor_connections!
    return if @_calculated_floor_connections
    all_leds.each_with_index do |led, i|
      led.index = i
      led.floor = floor_tube.leds.min_by do |fl|
        (fl.x - led.x).abs
      end
      led.floor.connections << led
    end

    floor_tube.leds.each_with_index do |led, i|
      led.index = i
    end

    unass = floor_tube.leds.reject { |l| !l.connections.empty? }.count
    $stderr.puts "unassigned floor leds: #{unass}" if unass > 0
    slutty = 4
    sluts = floor_tube.leds.reject { |l| l.connections.count <= slutty }.count
    if sluts > 0
      $stderr.puts "slutty floor leds: #{sluts} (more than #{slutty} connections)"
      floor_tube.leds.group_by { |l| l.connections.count }.sort_by { |k,v| k }.each do |connection_count, leds|
        if connection_count >= slutty
          $stderr.puts %Q~   % 3i: %s~ % [connection_count, '#' * leds.length]
        end
      end
    end
    @_calculated_floor_connections = true
  end
end

class Tube < OpenStruct
  class Led < Struct.new(:x, :y)
    attr_accessor :floor
    attr_accessor :index
  end

  class VirtualLed < Led
    def connections
      @connections ||= []
    end
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
      led_class.new x, y
    end
  end

  def led_class
    virtual ? VirtualLed : Led
  end

  def from_x; from[0]; end
  def from_y; from[1]; end
  def to_x; to[0]; end
  def to_y; to[1]; end
end


