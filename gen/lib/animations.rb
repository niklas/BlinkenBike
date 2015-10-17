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

  def by_auto
    sort_by { |e| e.auto ? 0 : 1 }
  end

end
