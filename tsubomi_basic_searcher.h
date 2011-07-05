// $Id$
#ifndef TSUBOMI_BASIC_SEARCHER
#define TSUBOMI_BASIC_SEARCHER
#include "tsubomi_defs.h"
#include "tsubomi_mmap.h"
#include "tsubomi_searcher.h"

namespace tsubomi {
  class basic_searcher : public searcher {

  public:
    basic_searcher(const char *filename);
    ~basic_searcher();

    sa_range search(const char *key);
    sa_range search(const char *key, sa_index begin, sa_index end);
    bool     get_line(sa_index pos, char *buf, sa_index size,
                      sa_index *pkeypos = NULL);

  protected:
    mmap_reader<char>     mr_file_;
    mmap_reader<sa_index> mr_sa_;

    basic_searcher(const basic_searcher &);
    const basic_searcher &operator=(const basic_searcher &);

    int compare2key(sa_index offset, const char *key);
    sa_index binary_search(const char *key, sa_index begin, sa_index end);
  };
}

#endif

