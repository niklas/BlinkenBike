require 'animations'
require 'transition'

class Transitions < Animations
  def self.functions
    {
      'init'  => 'int * meta, byte numPixels',
      'pixel' => 'int * meta, int * pixel, int i, byte numPixels',
    }
  end
end

