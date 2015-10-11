require 'containing'
require 'selective'
require 'names'
class Animations < Array
  include Containing
  include Selective
  include Names

  def count_const
    first.class.name.upcase + '_NUM'
  end

end
