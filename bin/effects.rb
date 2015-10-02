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

  def init_name
    "effect_init_#{name}"
  end

  def init_sign
    "EFFECT_INIT_SIGNATURE"
  end

  def pixel_name
    "effect_pixel_#{name}"
  end

  def pixel_sign
    "EFFECT_PIXEL_SIGNATURE"
  end

  def step_name
    "effect_step_#{name}"
  end

  def step_sign
    "EFFECT_STEP_SIGNATURE"
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
    return @sections if defined?(@sections)
    @sections = {}.tap do |sections|
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

      # make sure all sections are present, maybe even empty
      Effect::MethodSections.each do |sec|
        sections[sec] ||= ''
      end
    end
  end
end

class Effects < Array
  def implementation
    map(&:implementation).join("\n\n")
  end

  def header
    [
      function_signs,
      function_headers,
      inits,
      pixels,
      steps,
      count_const,
    ].join("\n\n\n")
  end

  def function_headers
    map(&:header).join("\n\n")
  end

  def inits
    x = map(&:init_name).join(",\n")

    %Q~void (*effectInit[])(EFFECT_INIT_SIGNATURE) = {\n#{x}\n};~
  end

  def pixels
    x = map(&:pixel_name).join(",\n")

    %Q~void (*effectPixel[])(EFFECT_PIXEL_SIGNATURE) = {\n#{x}\n};~
  end

  def steps
    x = map(&:step_name).join(",\n")

    %Q~void (*effectStep[])(EFFECT_STEP_SIGNATURE) = {\n#{x}\n};~
  end

  def function_signs
    [].tap do |s|
      s << %Q~#define EFFECT_INIT_SIGNATURE int * meta, byte numPixels~
      s << %Q~#define EFFECT_PIXEL_SIGNATURE int * meta, byte * pixel, int i, byte numPixels~
      s << %Q~#define EFFECT_STEP_SIGNATURE int * meta, byte numPixels~
    end.join("\n")
  end

  def count_const
    %Q~#define EFFECT_NUM #{size}~
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
    h.puts %Q~#ifndef __EFFECTS_H__~
    h.puts %Q~#define __EFFECTS_H__~
    h.puts
    h.puts effects.header
    h.puts
    h.puts %Q~#endif~
  end

  File.open 'src/effects.cpp', 'w' do |cpp|
    cpp.puts %Q~#include <Arduino.h>~
    cpp.puts %Q~#include "colors.h"~
    cpp.puts %Q~#include "trigometry.h"~
    cpp.puts %Q~#include "effects.h"~
    cpp.puts effects.implementation
  end
else
  raise "run as #{$0}"
end
