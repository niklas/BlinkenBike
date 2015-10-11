class Animation < Struct.new(:name, :source)
  def self.method_sections
    %w(init pixel)
  end

  def method_sections
    self.class.method_sections
  end

  def self.attribute_sections
    %w()
  end

  def attribute_sections
    self.class.attribute_sections
  end

  def initialize(*)
    super
    @attributes = {}
  end

  def signature_const(section)
    [
      self.class.name.upcase,
      section.upcase,
      'SIGNATURE'
    ].join('_')
  end

  def func_name(section)
    [
      self.class.name.downcase,
      name.downcase,
      section.downcase,
    ].join('_')
  end

  def sections
    return @sections if defined?(@sections)
    @sections = {}.tap do |sections|
      section = nil

      source.each_line do |line|
        case line
        when /^(#{(method_sections + ['setup']).join('|')}):\s*$/
          section = $1
          sections[section] = ''
        when /^(#{attribute_sections.join('|')}):\s*(\w+)\s*$/
          @attributes[$1] = $2
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
      method_sections.each do |sec|
        sections[sec] ||= ''
      end
    end
  end
end
