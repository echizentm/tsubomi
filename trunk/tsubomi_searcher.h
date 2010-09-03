// $Id$
#ifndef TSUBOMI_SEARCHER
#define TSUBOMI_SEARCHER
#include "tsubomi_defs.h"

namespace tsubomi {
  class searcher {
  public:
    virtual ~searcher(){};
    virtual sa_range search(const char *key) = 0;
    virtual bool get_line(sa_index pos, char *buf, sa_index size,
                          sa_index *pkeypos = NULL) = 0;
  };
}

#endif

