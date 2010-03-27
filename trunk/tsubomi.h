// $Id$
#ifndef TSUBOMI
#define TSUBOMI
#include <iostream>
#include <map>
#include <vector>

namespace tsubomi {
  using namespace std;

  class indexer {
    FILE         *fin_;
    bool         is_close_;
    vector<long> sa_;
  public:
    indexer(FILE *fin);
    indexer(const char *filename);
    ~indexer();
    void mkary(const char *seps = "");
    pair<long, long> search(const char *key);
    long get_offset(long index);
    void get_string(long index, char *buf, long size, const char *seps = "");
    void read(FILE *fin);
    void write(FILE *fout);
  private:
    indexer(const indexer &);
    const indexer &operator=(const indexer &);
    int compare2key(long offset, const char *key);
    long binary_search(const char *key, long begin, long end);
  };

  class reader {
    FILE *fin_;
  public:
    reader(FILE *fin);
    ~reader();
    void read(vector<long> *psa);
  private:
    reader(const reader &);
    const reader &operator=(const reader &);
    long read_little();
    long read_big();
  };

  class writer {
    FILE *fout_;
  public:
    writer(FILE *fout);
    ~writer();
    void write(vector<long> *psa);
  private:
    writer(const writer &);
    const writer &operator=(const writer &);
    void write_little(long index);
    void write_big(long index);
  };

  class comparer {
    FILE *fin_;
  public:
    comparer(FILE *fin) : fin_(fin) {}
    ~comparer() {}
    // if S[offset2] > S[offset1], then return true
    bool operator()(long offset1, long offset2) {
      int ret = 0;
      while (ret == 0) {
        if (fseek(this->fin_, offset2++, SEEK_SET) != 0) { return false; }
        if ((ret = fgetc(this->fin_)) == EOF) { return false; }
        int ch;
        if (fseek(this->fin_, offset1++, SEEK_SET) != 0) { return true; }
        if ((ch = fgetc(this->fin_)) == EOF) { return true; }
        ret -= ch;
      }
      return ret > 0;
    }
  };

  inline bool is_little_endian() {
    int i = 1;
    char *p = (char *)(&i);
    if (*p) { return true; }
    return false;
  }
}

#endif

