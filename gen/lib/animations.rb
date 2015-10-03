class Animations < Array
  def self.glob(glb)
    klass = item_class
    items = Dir[glb].map do |fn|
      source = File.read(fn)
      name   = File.basename(fn, '.*')

      klass.new name, source
    end
    new items
  end

  def self.item_class
    Object.const_get( name.sub(/s$/,'') )
  end

  def implementation
    [
      %Q~#include <Arduino.h>~,
      %Q~#include "colors.h"~,
      %Q~#include "trigometry.h"~,
      %Q~#include "settings.h"~,
      %Q~#include "#{self.class.name.downcase}.h"~,
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
      function_signs,
      function_headers,
      count_const_definition,
      function_array_names,
      '',
      %Q~#endif~
    ].join("\n")
  end

  def function_headers
    map(&:header).join("\n\n")
  end

  def func_array_name(section, count='')
    name    = first.array_name(section)
    sign    = first.signature_const(section)
    %Q~extern void (*#{name}[#{count}])(#{sign})~
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
end
