require 'mkmf'
require 'erb'
require 'rbconfig'

dir_config('ghash')

def define(s)
  $defs.push( format("-D HAVE_%s", s.to_s.upcase) )
end


def ext_try_cc(src,opt="",&b)
  cmd = "g++ -c#$INCFLAGS -I#{$hdrdir} #$CPPFLAGS #$CFLAGS #$ARCH_FLAG #{opt} #{CONFTEST_C}"
  try_do(src,cmd, &b)
ensure
  rm_f "conftest.c"
end

# check source for presence
def test_for(name, const, src)
  checking_for name do
    if ext_try_cc(src,"#{$CFLAGS} #{$LIBS}")
      define const
      true
    else
      false
    end
  end
end

sample_test=%{
#include <iostream>
#include <google/sparse_hash_map>

using google::sparse_hash_map;      // namespace where class lives by default
using std::cout;
using std::endl;
<% if ext_check %>
  using ext::hash;  // or __gnu_cxx::hash, or maybe tr1::hash, depending on your OS
<% elsif gnu_check %>
  using __gnu_cxx::hash;  // or __gnu_cxx::hash, or maybe tr1::hash, depending on your OS
<% else %>
  using tr1::hash;  // or __gnu_cxx::hash, or maybe tr1::hash, depending on your OS
<% end %>

struct eqstr
{
  bool operator()(const char* s1, const char* s2) const
  {
    return (s1 == s2) || (s1 && s2 && strcmp(s1, s2) == 0);
  }
};

int main()
{
  sparse_hash_map<const char*, int, hash<const char*>, eqstr> months;
  
  months["january"] = 31;
  months["february"] = 28;
  months["march"] = 31;
  months["april"] = 30;
  months["may"] = 31;
  months["june"] = 30;
  months["july"] = 31;
  months["august"] = 31;
  months["september"] = 30;
  months["october"] = 31;
  months["november"] = 30;
  months["december"] = 31;
  
  cout << "september -> " << months["september"] << endl;
  cout << "april     -> " << months["april"] << endl;
  cout << "june      -> " << months["june"] << endl;
  cout << "november  -> " << months["november"] << endl;
}
}

ext_check = true
test_for("google sparsehash library with ext::hash", "GS_HASH_EXT", ERB.new(sample_test, 0, "%<>").result() )
ext_check = false
gnu_check = true
test_for("google sparsehash library with __gnu_cxx::hash", "GS_HASH_GNU_CXX", ERB.new(sample_test, 0, "%<>").result() )
ext_check = false
gnu_check = false
test_for("google sparsehash library with tr1::hash", "GS_HASH_TR1", ERB.new(sample_test, 0, "%<>").result() )

create_header('ghash_config.h')
create_makefile('ghash_core')
