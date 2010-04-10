// $Id$
#include "tsubomi_indexer.h"
#include <iostream>
#include <string>
#include <cmath>
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
        if (offset1 >= this->mr_.size()) { return true; }
        ret -= this->mr_[offset1++];
      }
      return ret > 0;
    }
  };

  ////////////////////////////////////////////////////////////////
  class comparer2 {
    mmap_reader<char> &mr_;
    vector<sa_index>  &sa_;

  public:
    comparer2(mmap_reader<char> &mr, vector<sa_index> &sa)
     : mr_(mr), sa_(sa) {}
    ~comparer2() {}

    // if S[sa[i2]] > S[sa[i1]], then return true
    bool operator()(sa_index i1, sa_index i2) {
      sa_index offset1 = this->sa_[i1];
      sa_index offset2 = this->sa_[i2];

      int ret = 0;
      while (ret == 0) {
        if (offset2 >= this->mr_.size()) { return false; }
        ret = this->mr_[offset2++];
        if (offset1 >= this->mr_.size()) { return true; }
        ret -= this->mr_[offset1++];
      }
      return ret > 0;
    }
  };

  ////////////////////////////////////////////////////////////////
  class bit_vector {
    vector<char> v_;
  public:
    bit_vector(sa_index size) : v_((size >> 0x3) + 0x1, 0) {}
    ~bit_vector() {}

    bool set(sa_index i, bool bit) {
      char mask = 0x1 << (0x7 & i);
      if (bit) { return (this->v_[i >> 0x3] |= mask); }
      return (this->v_[i >> 0x3] &= ~mask);
    }
    bool get(sa_index i) {
      char mask = 0x1 << (0x7 & i);
      return (this->v_[i >> 0x3] & mask);
    }
  private:
    bit_vector(const bit_vector &);
    const bit_vector &operator=(const bit_vector &);
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
    if (is_progress) {
      int N = sa.size();
      progress_bar prg(int(5 * N / 40));
      cerr << "+--------------------------------------+" << endl;
      this->sais(sa, &prg);
      cerr << endl << "done!" << endl;
    } else {
      this->sais(sa);
    }

/*
    if (is_progress) {
      int N = sa.size();
      progress_bar prg(int(N * log(N) / log(2) / 40));
      cerr << "+--------------------------------------+" << endl;
      sort(sa.begin(), sa.end(), comparer(this->mr_file_, &prg));
      cerr << endl << "done!" << endl;
    } else {
      sort(sa.begin(), sa.end(), comparer(this->mr_file_));
    }
*/

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

  void indexer::sais(vector<sa_index> &sa, progress_bar *pprg) {
    // find LMS-string
    sa_index         size = sa.size();
    bit_vector       type(size); // true:S, false:L ,default:L
    vector<sa_index> sa_sub;     // contain index of sa
    vector<sa_index> count(sa_byte_size, 0);
    count[this->mr_file_[sa[size - 1]]]++;
    for (sa_index i = size - 2; i >= 0; i--) {
      if(pprg) { pprg->progress(1); }

      count[this->mr_file_[sa[i]]]++;
      bool pre_type = type.get(i + 1);
      int r = this->compare(i, i + 1, sa);
      if (r > 0 || (r == 0 && pre_type == 1)) { type.set(i, 1); }
      else if (pre_type == 1)                 { sa_sub.push_back(i + 1); }
    }
    sa_index size_sub = sa_sub.size();

    // sort LMS-string
    sort(sa_sub.begin(), sa_sub.end(), comparer2(this->mr_file_, sa));

    // set backet
    sa_index         sum = 0;
    vector<sa_index> pos(sa_byte_size);
    pos[0] = 0;
    for (int i = 1; i < sa_byte_size; i++) { pos[i] = sum; sum += count[i]; }

    // induced sort: step1
    vector<sa_index> sa_new(size, -1); // contain index of sa
    for (sa_index i = size_sub - 1; i >= 0; i--) {
      sa_byte  ch = this->mr_file_[sa[sa_sub[i]]];
      sa_index p  = size;
      if (ch < sa_byte_size - 1) { p = pos[ch + 1] - 1; }
      while (sa_new[p] != -1) { p--; }
      sa_new[p] = sa_sub[i];
    }
/*cout << "sa" << endl;
for (sa_index i = 0; i < size; i++) {
  cout << sa_new[i] << endl;
}*/

    // induced sort: step2
    if (type.get(size - 1) == 0) {
      sa_byte ch = this->mr_file_[sa[size - 1]];
      sa_new[pos[ch]] = size - 1;
      pos[ch]++;
    }
    for (sa_index i = 0; i < size; i++) {
      if(pprg) { pprg->progress(1); }

      sa_index sa_i = sa_new[i] - 1;
      if (sa_i < 0) { continue; }
      if (type.get(sa_i) == 0) {
        sa_byte ch = this->mr_file_[sa[sa_i]];
        sa_new[pos[ch]] = sa_i;
        pos[ch]++;
      }
    }
/*cout << "sa" << endl;
for (sa_index i = 0; i < size; i++) {
  cout << sa_new[i] << endl;
}*/

    // induced sort: step3
    sum = 0;
    for (int i = 0; i < sa_byte_size; i++) {
      sum += count[i];
      pos[i] = sum;
      if (pos[i] > 0) { pos[i]--; }
    }
    for (sa_index i = size - 1; i >= 0; i--) {
      if(pprg) { pprg->progress(1); }

      sa_index sa_i = sa_new[i] - 1;
      if (sa_i < 0) { continue; }
      if (type.get(sa_i) == 1) {
        sa_byte ch = this->mr_file_[sa[sa_i]];
        sa_new[pos[ch]] = sa_i;
        pos[ch]--;
      }
    }
/*cout << "sa" << endl;
for (sa_index i = 0; i < size; i++) {
  cout << sa_new[i] << endl;
}*/

    for (sa_index i = 0; i < size; i++) { if(pprg) { pprg->progress(1); } sa_new[i] = sa[sa_new[i]]; }
    for (sa_index i = 0; i < size; i++) { if(pprg) { pprg->progress(1); } sa[i]     = sa_new[i]; }
    return;
  }

  // return mr[sa[i2]] - mr[sa[i1]]
  inline int indexer::compare(sa_index i1, sa_index i2, vector<sa_index> &sa) {
    sa_index size    = sa.size();
    sa_index offset1 = sa[i1++];
    sa_index offset2 = sa[i2++];
    sa_index max1    = this->mr_file_.size();
    sa_index max2    = this->mr_file_.size();
    if (i1 < size) { max1 = sa[i1]; }
    if (i2 < size) { max2 = sa[i2]; }

    int ret = 0;
    while (ret == 0) {
      if (offset2 >= max2) { return ((offset1 >= max1) ? 0 : -1); }
      ret = this->mr_file_[offset2++];
      if (offset1 >= max1) { return 1; }
      ret -= this->mr_file_[offset1++];
    }
    return ret;
  }
}
