module Selective
  def <<(item)
    super(item) if selected?(item.name)
  end

  def selected?(name)
    selection.empty? || selection.include?(name)
  end

  def selection
    return @selection if defined?(@selection)
    @selection = (ENV[self.class.name.upcase] || '').split(',')
  end
end
