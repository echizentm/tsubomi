// $Id$
#ifndef TSUBOMI_COMPRESSOR
#define TSUBOMI_COMPRESSOR
#include "tsubomi_defs.h"
#include "tsubomi_searcher.h"
#include "tsubomi_vertical_code.h"

namespace tsubomi {
  class compressor : public searcher {
  public:
    compressor(){}
    compressor(const char *filename);
    ~compressor(){}
    void mkcsa(const char *filename, sa_index step = 100,
               bool is_progress = false);
    sa_index size();
    sa_range search(const char *key);
    bool get_line(sa_index pos, char *buf, sa_index size,
                  sa_index *pkeypos = NULL);
    void read(const char *filename);
    void write(const char *filename);

  private:
    vertical_code PSI_[256];
    vertical_code B_;
    vertical_code S_;
    vertical_code invB_;
    vertical_code invS_;
    vertical_code PSIhead_;
    vertical_code pos2ch_;
    sa_index      size_;
    sa_index      step_;

    compressor(const compressor &);
    const compressor &operator=(const compressor &);
    bool get_suffix(sa_index pos, char *buf, sa_index size);
    sa_index get_sa(sa_index pos);
    sa_index get_next(sa_index pos, char *pch = NULL);
    int compare2key(sa_index pos, const char *key);
    sa_index binary_search(const char *key, sa_index begin, sa_index end);
  };
}

#endif

