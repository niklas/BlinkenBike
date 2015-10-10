module Containing
  def item_class
    self.class.item_class
  end

  def self.included(base)
    base.class_eval do
      extend Containing::ClassMethods
    end
  end

  module ClassMethods
    def testing?
      ENV['BIKE_ENV'] == 'test'
    end

    def glob(glb)
      klass = item_class
      items = new
      Dir[glb].map do |fn|
        source = File.read(fn)
        name   = File.basename(fn, '.*')
        unless testing?
          if name =~ /^test_/
            next
          end
        end

        items << klass.new(name, source)
      end
      items
    end

    def item_class
      Object.const_get( name.sub(/s$/,'') )
    end

  end
end
