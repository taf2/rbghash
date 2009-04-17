require 'ghash_core'
require 'test/unit'

class TestGSet < Test::Unit::TestCase
  def setup
    @sparse_set = SparseSet.new
    @dense_set = DenseSet.new
    @rset = {}
  end

  def test_set_get
    t = Time.now
    n = 10000
    n.times do|i|
      @sparse_set.insert(i.to_s)
      assert @sparse_set.lookup(i.to_s)
    end
    puts "Sparse Took #{Time.now - t} seconds to insert #{n} records"

    t = Time.now
    n.times do|i|
      @dense_set.insert(i.to_s)
      assert @dense_set.lookup(i.to_s)
    end
    puts "Dense Took #{Time.now - t} seconds to insert #{n} records"
    
    t = Time.now
    n.times do|i|
      @rset[i.to_s] = nil
      assert @rset.key?(i.to_s)
    end
    puts "Ruby Took #{Time.now - t} seconds to insert #{n} records"
  end

end
