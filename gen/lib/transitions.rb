require 'animations'
require 'transition'

class Transitions < Animations
  def self.functions
    {
      'init'  => 'int * meta',
      'pixel' => 'int * meta, int * pixel, int i',
    }
  end
end

