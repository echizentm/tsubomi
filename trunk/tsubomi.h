// $Id$
#ifndef TSUBOMI
#define TSUBOMI
#include <iostream>
#include <map>
#include <vector>
#include "tsubomi_defs.h"
#include "tsubomi_mmap.h"

namespace tsubomi {
  using namespace std;

  class indexer {
    bool              is_close_;
    vector<sa_index>  sa_;
    mmap_reader<char> mr_;
  public:
    indexer(const char *filename);
    ~indexer();
    void mkary(const char *seps = "", bool is_progress = false);
    pair<sa_index, sa_index> search(const char *key);
    sa_index get_offset(sa_index index);
    void get_string(sa_index index, char *buf, sa_index size, const char *seps = "");
    void read(const char *filename);
    void write(const char *filename);
  private:
    indexer(const indexer &);
    const indexer &operator=(const indexer &);
    int compare2key(sa_index offset, const char *key);
    sa_index binary_search(const char *key, sa_index begin, sa_index end);
  };

  class reader {
    FILE *fin_;
  public:
    reader(FILE *fin);
    ~reader();
    void read(vector<sa_index> *psa);
  private:
    reader(const reader &);
    const reader &operator=(const reader &);
    sa_index read_little();
    sa_index read_big();
  };

  class writer {
    FILE *fout_;
  public:
    writer(FILE *fout);
    ~writer();
    void write(vector<sa_index> *psa);
  private:
    writer(const writer &);
    const writer &operator=(const writer &);
    void write_little(sa_index index);
    void write_big(sa_index index);
  };

  class progress_bar {
    int cur_;
    int max_;
    int star_;
  public:
    progress_bar(int max) : cur_(0), max_(max), star_(0) {}
    ~progress_bar() {};
    void progress(int num) {
      this->cur_ += num;
      if (this->cur_ >= this->max_) {
        if (star_ < 40) { cout << "*"; this->star_++; }
        this->cur_ = 0;
      }
      return;
    }
  };

  class comparer {
    mmap_reader<char> &mr_;
    progress_bar      *pprg_;
  public:
    comparer(mmap_reader<char> &mr, progress_bar *pprg = NULL)
     : mr_(mr), pprg_(pprg) {}
    ~comparer() {}

    // if S[offset2] > S[offset1], then return true
    bool operator()(sa_index offset1, sa_index offset2) {
      if (this->pprg_) { this->pprg_->progress(1); }

      int ret = 0;
      while (ret == 0) {
        if (offset2 >= this->mr_.size()) { return false; }
        ret = this->mr_[offset2++];
        if (offset1 >= this->mr_.size()) { return false; }
        ret -= this->mr_[offset1++];
      }
      return ret > 0;
    }
  };
}

#endif

