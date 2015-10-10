module Names
  def name
    @name ||= self.class.name
  end

  def method_sections
    item_class.method_sections
  end

  def function_type_name(section)
    [
      name,
      section.capitalize,
      'Function'
    ].join
  end

  def type_list_name(section)
    [
      name,
      section.capitalize,
      'FunctionList'
    ].join
  end

  def func_array_name(section, count='')
    type    = type_list_name(section)
    name    = first.array_name(section)
    %Q~extern #{type} #{name}~
  end

  def function_array_names
    method_sections.map do |section|
      func_array_name(section, count_const) + ';'
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
