// $Id$
#ifndef TSUBOMI_INDEXER
#define TSUBOMI_INDEXER
#include "tsubomi_defs.h"
#include "tsubomi_mmap.h"
#include <vector>

namespace tsubomi {
  class indexer {
  public:
    indexer(const char *filename);
    virtual ~indexer();
    virtual void mkary(const char *seps = "", bool is_utf8 = false,
                       bool is_progress = false,
                       bool is_make = true, bool is_sort = true);

  protected:
    const char        *filename_;
    mmap_reader<char> mr_file_;

    virtual void mkary_make(std::vector<sa_index> &sa,
                            const char *seps, bool is_utf8);
    virtual void mkary_sort(std::vector<sa_index> &sa, bool is_progress);
    virtual void sort(std::vector<sa_index> &sa, sa_index begin, sa_index end,
                      sa_index depth, progress_bar *pprg = NULL);

  private:
    indexer(const indexer &);
    const indexer &operator=(const indexer &);
  };

  class writer {
    FILE *fout_;
  public:
    writer(FILE *fout) : fout_(fout) {}
    ~writer() {}
    void write(std::vector<sa_index> &sa);
  private:
    writer(const writer &);
    const writer &operator=(const writer &);
  };

  class reader {
    FILE *fin_;
  public:
    reader(FILE *fin) : fin_(fin) {}
    ~reader() {}
    void read(std::vector<sa_index> &sa);
  private:
    reader(const reader &);
    const reader &operator=(const reader &);
  };
}

#endif

