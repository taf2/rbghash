# measure hash memory usage
require 'ghash_core'
require 'rubygems'
require 'rmem' # sudo gem install taf2-rmem # from github.com

def sparse_test
  # measure memory usage from sparse set
  sparse_set = SparseSet.new
  1000000.times {|i| sparse_set[i.to_s] = nil }
  printf("memory: %.4f MB\n", RMem::Report.memory/1024.0/1024.0)
  sparse_set = nil
  GC.start
end

printf("memory: %.4f MB\n", (RMem::Report.memory/1024.0/1024.0))
sparse_test
printf("memory: %.4f MB\n", (RMem::Report.memory/1024.0/1024.0))
