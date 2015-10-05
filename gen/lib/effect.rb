require 'animation'
class Effect < Animation
  def self.method_sections
    super + %w(step)
  end
end
