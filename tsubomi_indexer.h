// $Id$
#ifndef TSUBOMI_INDEXER
#define TSUBOMI_INDEXER

namespace tsubomi {
  using namespace std;

  class indexer {
  public:
    indexer() {}
    ~indexer() {}
    void mkary(const char *filename, const char *seps = "", bool is_progress = false);
  private:
    indexer(const indexer &);
    const indexer &operator=(const indexer &);
  };
}

#endif

