require 'animations'
require 'effect'

class Effects < Animations
  def self.functions
    {
      'init'  => 'int * meta, byte numPixels',
      'pixel' => 'int * meta, CRGB * pixel, int i, byte numPixels',
      'step'  => 'int * meta, byte numPixels',
    }
  end
end

