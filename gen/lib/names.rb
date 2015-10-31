module Names
  def name
    @name ||= self.class.name
  end

  def singular_name
    @singular_name ||= name.sub(/s$/,'')
  end

  def method_sections
    item_class.method_sections
  end

  def function_type_name(section)
    [
      singular_name,
      section.capitalize,
      'Function'
    ].join
  end

  def type_list_name(section)
    [
      singular_name,
      section.capitalize,
      'FunctionList'
    ].join
  end

  def func_array_accessor_name(section)
    [
      singular_name.downcase,
      section.capitalize,
    ].join
  end

  def func_array_name(section)
    [
      singular_name.downcase,
      section.capitalize,
      's'
    ].join
  end

  def func_array_type_and_name(section, count='')
    type    = type_list_name(section)
    %Q~const #{function_type_name(section)} #{func_array_name(section)}~
  end

  def attribute_array_names
    first.attribute_sections.map(&method(:attribute_array_name)).join("\n")
  end

  def attribute_array_name(section)
    name    = func_array_name(section)
    typ     = self.class.attributes[section]
    %Q~extern const #{typ} #{name}[#{length}];~
  end
end
