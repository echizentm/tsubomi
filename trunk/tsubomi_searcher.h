// $Id$
#ifndef TSUBOMI_SEARCHER
#define TSUBOMI_SEARCHER
#include "tsubomi_defs.h"
#include "tsubomi_mmap.h"

namespace tsubomi {
  using namespace std;

  class searcher {
    mmap_reader<char>     mr_file_;
    mmap_reader<sa_index> mr_sa_;

  public:
    searcher(const char *filename)
     : mr_file_(filename), mr_sa_(string(filename) + ".ary") {}
    ~searcher() {}

    sa_range search(const char *key);
    sa_range search(const char *key, sa_index begin, sa_index end);
    sa_index get_offset(sa_index index);
    void get_string(sa_index index, char *buf, sa_index size, const char *seps = "");
    void get_string(sa_index index, char *buf,
                    sa_index left, const char *seps_l, sa_index right, const char *seps_r);
    bool get_value(const char *key, char *buf, sa_index size, const char *seps = "");

  private:
    searcher(const searcher &);
    const searcher &operator=(const searcher &);

    int compare2key(sa_index offset, const char *key);
    sa_index binary_search(const char *key, sa_index begin, sa_index end);
  };
}

#endif

