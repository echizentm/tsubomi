// $Id$
#ifndef TSUBOMI_BASIC_SEARCHER
#define TSUBOMI_BASIC_SEARCHER
#include "tsubomi_defs.h"
#include "tsubomi_mmap.h"
#include "tsubomi_searcher.h"
#include <string>

namespace tsubomi {
  class basic_searcher : public searcher {

  public:
    basic_searcher(const char *filename);
    virtual ~basic_searcher();

    virtual sa_range search(const char *key);
    virtual sa_range search(const char *key, sa_index begin, sa_index end);
    virtual bool     get_line(sa_index pos, char *buf, sa_index size,
                              sa_index *pkeypos = NULL);
    virtual bool     get_line(sa_index pos, std::string &buf,
                              sa_index *pkeypos = NULL);

  protected:
    mmap_reader<char>     mr_file_;
    mmap_reader<sa_index> mr_sa_;

    virtual int compare2key(sa_index offset, const char *key);
    virtual sa_index binary_search(const char *key, sa_index begin, sa_index end);

  private:
    basic_searcher(const basic_searcher &);
    const basic_searcher &operator=(const basic_searcher &);
  };
}

#endif

