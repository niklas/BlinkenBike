require 'animations'
require 'effect'

class Effects < Animations
  def self.functions
    {
      'init'  => 'int * meta',
      'pixel' => 'int * meta, byte * pixel, int i',
      'step'  => 'int * meta',
    }
  end
end

