require 'animation'
class Effect < Animation
  def self.method_sections
    super + %w(step)
  end

  def self.attribute_sections
    super + %w(orientation)
  end

  def orientation_attribute
    v = @attributes['orientation']
    if !v || v.empty?
      'ORIENTATION_NIL'
    else
      %Q~ORIENTATION_#{v.upcase}~
    end
  end
end
