// $Id$
#include "tsubomi_indexer.h"
#include <iostream>
#include <string>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>

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
  // class indexer
  void indexer::mkary(const char *seps, bool is_progress) {
    // read index from file
    vector<sa_index> sa;
    if (seps[0] == '\0') {
      for (sa_index offset = 0; offset < this->mr_file_.size(); offset++) {
        sa.push_back(offset);
      }
    } else {
      bool flg = true;
      for (sa_index offset = 0; offset < this->mr_file_.size(); offset++) {
        if (flg) { sa.push_back(offset); flg = false; }
        for (int i = 0; seps[i] != '\0'; i++) {
          if (this->mr_file_[offset] == seps[i]) { flg = true; }
        }
      }
    }

    // sort index & make suffix array
    srand((unsigned)time(NULL));
    int N = sa.size();
    if (is_progress) {
      progress_bar prg(int(N * log(N) / log(2) / 40));
      cerr << "+--------------------------------------+" << endl;
      this->sort(sa, 0, N - 1, 0, &prg);
      cerr << endl << "done!" << endl;
    } else {
      this->sort(sa, 0, N - 1, 0);
    }

    // write suffix array as "filename.ary"
    string aryname(this->filename_);
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

  void indexer::sort(vector<sa_index> &sa, sa_index begin, sa_index end,
                     sa_index depth, progress_bar *pprg) {
    sa_index a = begin;
    sa_index b = begin;
    sa_index c = end;
    sa_index d = end;
    sa_index size = end - begin + 1;
    if (size <= 1) { return; }

    sa_index offset = sa[begin + rand() % size] + depth;
    int      pivot  = -1000; // smaller than any char(-127...128)
    if (offset < this->mr_file_.size()) { pivot = this->mr_file_[offset]; }

    while (1) {
      sa_index b_offset = sa[b] + depth;
      int      b_ch     = -1000; // smaller than any char(-127...128)
      if (b_offset < this->mr_file_.size()) { b_ch = this->mr_file_[b_offset]; }
      sa_index c_offset = sa[c] + depth;
      int      c_ch     = -1000; // smaller than any char(-127...128)
      if (c_offset < this->mr_file_.size()) { c_ch = this->mr_file_[c_offset]; }

      while (b <= c && b_ch <= pivot) {
        if (pprg) { pprg->progress(1); }
        if (b_ch == pivot) { swap(sa[a], sa[b]); a++; }
        b++;
        b_offset = sa[b] + depth;
        b_ch     = -1000; // smaller than any char(-127...128)
        if (b_offset < this->mr_file_.size()) { b_ch = this->mr_file_[b_offset]; }
      }
      while (b <= c && c_ch >= pivot) {
        if (pprg) { pprg->progress(1); }
        if (c_ch == pivot) { swap(sa[c], sa[d]); d--; }
        c--;
        c_offset = sa[c] + depth;
        c_ch     = -1000; // smaller than any char(-127...128)
        if (c_offset < this->mr_file_.size()) { c_ch = this->mr_file_[c_offset]; }
      }
      if (b > c) { break; }
      swap(sa[b], sa[c]); b++; c--;
    }

    sa_index eq_size = 0;
    eq_size = (((a - begin) < (b - a)) ? (a - begin) : (b - a));
    for (sa_index i = 0; i < eq_size; i++) { swap(sa[begin + i], sa[b - eq_size + i]); }
    eq_size = (((d - c) < (end - d)) ? (d - c) : (end - d));
    for (sa_index i = 0; i < eq_size; i++) { swap(sa[b + i], sa[end - eq_size + i + 1]); }

    this->sort(sa, begin,             begin + b - a - 1,   depth,     pprg);
    this->sort(sa, begin + b - a,     end   - d + c,       depth + 1, pprg);
    this->sort(sa, end   - d + c + 1, end,                 depth,     pprg);
    return;
  }
}
