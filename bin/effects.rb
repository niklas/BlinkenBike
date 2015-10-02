#!/usr/bin/env ruby

class Effect < Struct.new(:name, :source)
  MethodSections = %w(init pixel step)

  def implementation
    sections.map do |sec, code|
      case sec
      when 'setup'
        code.gsub(/^\s{2}/, '') # just outdent
      when *MethodSections
        c_func(sec, code)
      else
        raise "unknown section: #{sec}"
      end
    end.join("\n")
  end

  def header
    sections.map do |sec, _code|
      case sec
      when 'setup'
        # nuffin
      when *MethodSections
        c_header(sec) + ';'
      end
    end.join("\n")
  end

  private

  def init_name
    "effect_init_#{name}"
  end

  def init_sign
    "meta"
  end

  def pixel_name
    "effect_pixel_#{name}"
  end

  def pixel_sign
    "meta, pixel"
  end

  def step_name
    "effect_step_#{name}"
  end

  def step_sign
    "meta"
  end

  def c_header(sec)
    name = send("#{sec}_name")
    sign = send("#{sec}_sign")
    %Q~void #{name}(#{sign})~
  end

  def c_func(sec, code)
    header = c_header(sec)
    %Q~#{header} {\n#{code}}\n~
  end

  def sections
    {}.tap do |sections|
      section = nil

      source.each_line do |line|
        case line
        when /^(setup|init|pixel|step):\s*$/
          section = $1
          sections[section] = ''
        when /^\s*#/,%r~^\s*//~
          # ignore comments
        when /^\s*$/
        else
          raise "must start with a 'section:': #{fn}" unless section

          sections[section] << line
        end
      end
    end
  end
end

class Effects < Array
  def implementation
    map(&:implementation).join("\n\n")
  end

  def header
    map(&:header).join("\n\n")
  end
end

if $0 == __FILE__
  effects = Dir['effects/*.effect'].map do |fn|
    source = File.read(fn)
    name   = File.basename(fn, '.*')

    Effect.new name, source
  end
  effects = Effects.new(effects)

  File.open 'src/effects.h', 'w' do |h|
    h.puts effects.header
  end

  File.open 'src/effects.cpp', 'w' do |cpp|
    cpp.puts effects.implementation
  end
else
  raise "run as #{$0}"
end
