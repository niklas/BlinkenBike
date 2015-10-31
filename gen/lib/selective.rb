module Selective
  Always = %w(stvzo67 usa_police)
  def <<(item)
    super(item) if selected?(item.name)
  end

  def selected?(name)
    selection.empty? || selection.include?(name) || Always.include?(name)
  end

  def selection
    return @selection if defined?(@selection)
    @selection = (ENV[self.class.name.upcase] || '').split(',')
  end
end
