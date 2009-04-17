#ifndef GHASH_RUBY_H
#define GHASH_RUBY_H

#include "ghash_config.h"
#include <string.h>
#include <stdlib.h>
#include <google/sparse_hash_set>
#include <google/dense_hash_set>

/* hash namespace */
#ifdef HAVE_GS_HASH_EXT
  #define hns ext
#elif HAVE_GS_HASH_GNU_CXX
  #define hns __gnu_cxx
#elif HAVE_GS_HASH_TR1
  #define hns tr1
#else
#error "No hash function available ext::hash, __gnu_cxx::hash, or tr1::hash"
#endif

unsigned int MurmurHash2( const void * key, int len, unsigned int seed );

namespace GRB {
  struct eqstr {
    inline bool operator()(const char *s1, const char *s2) const {
      return (s1 == s2) || (s1 && s2 && strcmp(s1,s2) == 0);
    }
  };

  struct hashstr {
    inline unsigned int operator()(const char *key) const {
      unsigned int len = (unsigned int)strlen((const char*)key);
      return MurmurHash2(key,len,128);
    }
  };

  typedef google::sparse_hash_set<const char*, hashstr, eqstr> SparseSet;
  typedef google::dense_hash_set<const char*, hashstr, eqstr> DenseSet;
  //typedef google::sparse_hash_set<const char*, hns::hash<const char*>, eqstr> SparseSet;
  //typedef google::dense_hash_set<const char*, hns::hash<const char*>, eqstr> DenseSet;
}

#endif
