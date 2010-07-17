// $Id$
#ifndef TSUBOMI_INDEXER
#define TSUBOMI_INDEXER
#include "tsubomi_defs.h"
#include "tsubomi_mmap.h"
#include <vector>

namespace tsubomi {
  using namespace std;

  class progress_bar;

  class indexer {
    const char        *filename_;
    mmap_reader<char> mr_file_;

  public:
    indexer(const char *filename) : filename_(filename), mr_file_(filename) {}
    ~indexer() {}
    void mkary(const char *seps = "", bool is_utf8 = false, bool is_progress = false,
               bool is_make = true, bool is_sort = true);

  private:
    indexer(const indexer &);
    const indexer &operator=(const indexer &);

    void mkary_make(vector<sa_index> &sa, const char *seps, bool is_utf8);
    void mkary_sort(vector<sa_index> &sa, bool is_progress);
    void sort(vector<sa_index> &sa, sa_index begin, sa_index end,
              sa_index depth, progress_bar *pprg = NULL);
  };
}

#endif

