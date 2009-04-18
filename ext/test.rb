require 'ghash_core'
require 'test/unit'
require 'benchmark'

class TestGSet < Test::Unit::TestCase
  def setup
    @sparse_set = SparseSet.new
    @dense_set = DenseSet.new
    @rset = {}
  end

  def test_serialize
    n = 1000
    n.times do|i|
      @sparse_set.insert(i.to_s)
    end
    n.times do|i|
      assert @sparse_set.lookup(i.to_s)
    end
    @sparse_set.save "test.hash"

    tset = SparseSet.new
    tset.load "test.hash"
    n.times do|i|
      assert @sparse_set.lookup(i.to_s)
      assert tset.lookup(i.to_s)
    end
  ensure
    File.unlink("test.hash") if File.exist?("test.hash")
  end

  def test_set_get
    n = 10000

    Benchmark.bmbm do |x|
      x.report("Sparse Set") { sample_set(@sparse_set, n) }
      x.report("Dense Set") { sample_set(@dense_set, n) }
      x.report("Ruby Hash") { sample_set(@rset, n) }
    end

  end

  def sample_set(set, n)
    n.times do|i|
      set[i.to_s] = nil
    end
    n.times do|i|
      assert set.key?(i.to_s)
    end
  end

end
