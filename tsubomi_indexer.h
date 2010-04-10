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
    void mkary(const char *seps = "", bool is_progress = false);

  private:
    indexer(const indexer &);
    const indexer &operator=(const indexer &);

    void sais(vector<sa_index> &sa, progress_bar *pprg = NULL);
    int compare(sa_index i1, sa_index i2, vector<sa_index> &sa);
  };
}

#endif

