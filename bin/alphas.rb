#!/usr/bin/env ruby

class Alpha < Struct.new(:name, :source)
  MethodSections = %w(init pixel)

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
    "alpha_init_#{name}"
  end

  def init_sign
    "ALPHA_INIT_SIGNATURE"
  end

  def pixel_name
    "alpha_pixel_#{name}"
  end

  def pixel_sign
    "ALPHA_PIXEL_SIGNATURE"
  end

  def c_header(sec)
    name = send("#{sec}_name")
    sign = send("#{sec}_sign")
    %Q~void #{name}(#{sign})~
  end

  def c_func(sec, code)
    header = c_header(sec)
    extra = sec == 'init' ? '  meta[0] = 1;' : ''
    %Q~#{header} {\n#{code}#{extra}\n}\n~
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
          # ignore comments before first section
          if section
            sections[section] << line
          end
        when /^\s*$/
        else
          raise "must start with a 'section:': #{fn}" unless section

          sections[section] << line
        end
      end

      # make sure all sections are present, maybe even empty
      Alpha::MethodSections.each do |sec|
        sections[sec] ||= ''
      end
    end
  end
end

class Alphas < Array
  def implementation
    [
      map(&:implementation).join("\n\n"),
      func_array('init'),
      func_array('pixel'),
    ].join("\n\n\n")
  end

  def header
    [
      function_signs,
      function_headers,
      count_const,
      function_array_names,
    ].join("\n\n\n")
  end

  def function_headers
    map(&:header).join("\n\n")
  end

  def func_array_name(section, count='')
    name    = 'alpha' + section.capitalize
    sign    = "ALPHA_#{section.upcase}_SIGNATURE"
    %Q~extern void (*#{name}[#{count}])(#{sign})~
  end

  def function_array_names
    Alpha::MethodSections.map do |section|
      func_array_name(section, 'ALPHA_NUM') + ';'
    end.join("\n")
  end

  def func_array(section)
    content = map(&:"#{section}_name").join(",\n")

    %Q~#{func_array_name(section)} = {\n#{content}\n};~
  end

  def function_signs
    [].tap do |s|
      s << %Q~#define ALPHA_INIT_SIGNATURE int * meta, byte numPixels~
      s << %Q~#define ALPHA_PIXEL_SIGNATURE int * meta, byte * pixel, int i, byte numPixels~
    end.join("\n")
  end

  def count_const
    %Q~#define ALPHA_NUM #{size}~
  end
end

if $0 == __FILE__
  alphas = Dir['transitions/*.transition'].map do |fn|
    source = File.read(fn)
    name   = File.basename(fn, '.*')

    Alpha.new name, source
  end
  alphas = Alphas.new(alphas)

  File.open 'src/transitions.h', 'w' do |h|
    h.puts %Q~#ifndef __ALPHAS_H__~
    h.puts %Q~#define __ALPHAS_H__~
    h.puts
    h.puts alphas.header
    h.puts
    h.puts %Q~#endif~
  end

  File.open 'src/transitions.cpp', 'w' do |cpp|
    cpp.puts %Q~#include <Arduino.h>~
    cpp.puts %Q~#include "colors.h"~
    cpp.puts %Q~#include "trigometry.h"~
    cpp.puts %Q~#include "alphas.h"~
    cpp.puts alphas.implementation
  end
else
  raise "run as #{$0}"
end

