class Animations < Array
  Testing = ENV['BIKE_ENV'] == 'test'

  def self.glob(glb)
    klass = item_class
    items = new
    Dir[glb].map do |fn|
      source = File.read(fn)
      name   = File.basename(fn, '.*')
      unless Testing
        if name =~ /^test_/
          next
        end
      end

      items << klass.new(name, source)
    end
    items
  end

  def self.item_class
    Object.const_get( name.sub(/s$/,'') )
  end

  def <<(item)
    super(item) if selected?(item.name)
  end

  def selected?(name)
    selection.empty? || selection.include?(name)
  end

  def selection
    return @selection if defined?(@selection)
    @selection = (ENV[self.class.name.upcase] || '').split(',')
  end

  def implementation
    [
      %Q~#include "#{self.class.name}.h"~,
      attribute_arrays,
      map(&:implementation).join("\n\n"),
      *first.method_sections.map(&method(:func_array))
    ].join("\n")
  end

  def header
    guard = "__#{self.class.name.upcase}_H__"
    [
      %Q~#ifndef #{guard}~,
      %Q~#define #{guard}~,
      '',
      %Q~#include <Arduino.h>~,
      %Q~#include "FastLED.h"~,
      %Q~#include "Settings.h"~,
      %Q~#include "Layout.h"~,
      function_signs,
      type_defs,
      function_headers,
      count_const_definition,
      function_array_names,
      '',
      %Q~#endif~
    ].join("\n")
  end

  def type_defs
    first.method_sections.map do |section|
      %~typedef void (*#{type_list_name(section)}[])(#{first.signature_const(section)});~
    end.join("\n")
  end

  def type_list_name(section)
    "Simple#{self.class.name}#{section.capitalize}List"
  end

  def function_headers
    map(&:header).join("\n\n")
  end

  def func_array_name(section, count='')
    type    = type_list_name(section)
    name    = first.array_name(section)
    %Q~extern #{type} #{name}~
  end

  def function_array_names
    first.method_sections.map do |section|
      func_array_name(section, count_const) + ';'
    end.join("\n")
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

  def attribute_array_names
    first.attribute_sections.map(&method(:attribute_array_name)).join("\n")
  end

  def attribute_array_name(section)
    name    = first.array_name(section)
    typ     = self.class.attributes[section]
    %Q~extern const #{typ} #{name}[#{length}];~
  end
end
