// $Id$
#include "tsubomi_indexer.h"
#include "tsubomi_defs.h"
#include "tsubomi_mmap.h"
#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include <algorithm>

namespace tsubomi {
  using namespace std;

  ////////////////////////////////////////////////////////////////
  class writer {
    FILE *fout_;
  public:
    writer(FILE *fout) : fout_(fout) {}
    ~writer() {}
    void write(vector<sa_index> *psa);
  private:
    writer(const writer &);
    const writer &operator=(const writer &);
  };

  void writer::write(vector<sa_index> *psa) {
    vector<sa_index>::iterator i = psa->begin();
    while (i != psa->end()) {
      sa_index index = *i;
      if (fwrite(&index, sizeof(sa_index), 1, this->fout_) < 1) {
        throw "error at writer::write(). fwrite() failure.";
      }
      i++;
    }
    return;
  }

  ////////////////////////////////////////////////////////////////
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
        if (star_ < 40) { cerr << "*"; this->star_++; }
        this->cur_ = 0;
      }
      return;
    }
  private:
    progress_bar(const progress_bar &);
    const progress_bar &operator=(const progress_bar &);
  };

  ////////////////////////////////////////////////////////////////
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

  ////////////////////////////////////////////////////////////////
  // class indexer
  void indexer::mkary(const char *filename, const char *seps, bool is_progress) {
    // make mmap to file
    mmap_reader<char> mr(filename);

    // read index from file
    vector<sa_index> sa;
    if (seps[0] == '\0') {
      for (sa_index offset = 0; offset < mr.size(); offset++) {
        sa.push_back(offset);
      }
    } else {
      bool flg = true;
      for (sa_index offset = 0; offset < mr.size(); offset++) {
        if (flg) { sa.push_back(offset); flg = false; }
        for (int i = 0; seps[i] != '\0'; i++) {
          if (mr[offset] == seps[i]) { flg = true; }
        }
      }
    }

    // sort index & make suffix array
    if (is_progress) {
      int N = sa.size();
      progress_bar prg(int(N * log(N) / log(2) / 40));
      cerr << "+--------------------------------------+" << endl;
      sort(sa.begin(), sa.end(), comparer(mr, &prg));
      cerr << endl << "done!" << endl;
    } else {
      sort(sa.begin(), sa.end(), comparer(mr));
    }

    // write suffix array as "filename.ary"
    string aryname(filename);
    aryname += ".ary";
    FILE *fout = fopen(aryname.c_str(), "wb");
    if (fout == NULL) {
      throw "error at indexer::mkary(). filename.ary cannot open.";
    }
    writer w(fout);
    w.write(&sa);
    fclose(fout);
    return;
  }

}
