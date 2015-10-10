require 'containing'
require 'selective'
require 'names'
class Animations < Array
  include Containing
  include Selective
  include Names

  def implementation
    [
      %Q~#include "#{name}.h"~,
      attribute_arrays,
      map(&:implementation).join("\n\n"),
      *method_sections.map(&method(:func_array))
    ].join("\n")
  end

  def header
    guard = "__#{name}_H__"
    [
      %Q~#ifndef #{guard}~,
      %Q~#define #{guard}~,
      '',
      %Q~#include <Arduino.h>~,
      %Q~#include "FastLED.h"~,
      %Q~#include "Settings.h"~,
      %Q~#include "Layout.h"~,
      '// function_signs',
      function_signs,
      '// function_type_defs',
      function_type_defs,
      '// function_headers',
      function_headers,
      '// count_const_definition',
      count_const_definition,
      '',
      %Q~#endif~
    ].join("\n")
  end

  def function_type_defs
    method_sections.map do |section|
      %~typedef void (*#{function_type_name(section)})(#{first.signature_const(section)});~
    end.join("\n")
  end


  def type_defs
    method_sections.map do |section|
      %~typedef void (*#{type_list_name(section)}[])(#{first.signature_const(section)});~
    end.join("\n")
  end

  def function_headers
    map(&:header).join("\n\n")
  end

  def func_array(section)
    content = map { |a| a.func_name(section) }.join(",\n")

    %Q~#{func_array_name(section)} = {\n#{content}\n};~
  end

  def function_signs
    self.class.functions.map do |section, sign|
      sign_name = first.signature_const(section)
      %Q~#define #{sign_name} #{sign}~
    end.join("\n")
  end

  def count_const_definition
    %Q~#define #{count_const} #{size}~
  end

  def count_const
    first.class.name.upcase + '_NUM'
  end

  def attribute_arrays
    first.attribute_sections.map do |section|
      name    = first.array_name(section)
      content = map(&:"#{section}_attribute").join(',')
      typ     = self.class.attributes[section]
      %Q~PROGMEM const #{typ} #{name}[] = {#{content}};~
    end.join("\n")
  end

end
