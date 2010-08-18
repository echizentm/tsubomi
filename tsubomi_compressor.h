// $Id$
#ifndef TSUBOMI_COMPRESSOR
#define TSUBOMI_COMPRESSOR
#include "tsubomi_defs.h"
#include "tsubomi_vertical_code.h"

namespace tsubomi {
  class compressor {
  public:
    compressor(){}
    ~compressor(){}
    void mkcsa(const char *filename, bool is_progress = false);
    sa_index size();
    sa_range search(const char *key);
    sa_index get_suffix(sa_index pos, char *buf,
                        sa_index size, const char *seps);
    void read(const char *filename);
    void write(const char *filename);

  private:
    vertical_code PSI_[256];
    vertical_code PSIhead_;
    vertical_code pos2ch_;
    sa_index      size_;

    compressor(const compressor &);
    const compressor &operator=(const compressor &);
    sa_index get_next(sa_index pos, char *pch = NULL);
    int compare2key(sa_index pos, const char *key);
    sa_index binary_search(const char *key, sa_index begin, sa_index end);
  };
}

#endif

