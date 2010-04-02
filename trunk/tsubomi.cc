// $Id$
#include "tsubomi.h"
#include <algorithm>
#include <cmath>

namespace tsubomi {
  using namespace std;

  //////////////////////////
  // CONSTRUCT & DESTRUCT //
  //////////////////////////
  indexer::indexer(const char *filename) : mr_(filename) {
    return;
  }
  indexer::~indexer() {
    return;
  }

  //////////////////////
  // PUBLIC FUNCTIONS //
  //////////////////////
  void indexer::mkary(const char *seps, bool is_progress) {
    sa_index offset = 0;
    int      ch     = '\0';
    this->sa_.clear();
    if (seps[0] == '\0') {
      for (sa_index offset = 0; offset < this->mr_.size(); offset++) {
        this->sa_.push_back(offset);
      }
    } else {
      bool flg = true;
      for (sa_index offset = 0; offset < this->mr_.size(); offset++) {
        if (flg) { this->sa_.push_back(offset); flg = false; }
        for (int i = 0; seps[i] != '\0'; i++) {
          if (this->mr_[offset] == seps[i]) { flg = true; }
        }
      }
    }

    if (is_progress) {
      int N = this->sa_.size();
      progress_bar prg(int(N * log(N) / log(2) / 40));
      cout << "+--------------------------------------+" << endl;
      sort(this->sa_.begin(), this->sa_.end(), comparer(this->mr_, &prg));
      cout << endl << "done!" << endl;
    } else {
      sort(this->sa_.begin(), this->sa_.end(), comparer(this->mr_));
    }
    return;
  }

  pair<sa_index, sa_index> indexer::search(const char *key) {
    sa_index size  = this->sa_.size();
    sa_index index = this->binary_search(key, 0, size);
    if (index == -1) { return pair<sa_index, sa_index>(-1, -1); }

    sa_index begin = index - 1;
    sa_index end   = index + 1;
    while (begin >= 0 && this->compare2key(this->sa_[begin], key) == 0) { begin--; }
    while (end < size && this->compare2key(this->sa_[end]  , key) == 0) { end++; }
    return pair<sa_index, sa_index>(begin + 1, end - 1);
  }

  sa_index indexer::get_offset(sa_index index) {
    if (index < 0 || this->sa_.size() <= index) {
      throw "error at indexer::get_offset(). index is out if range.";
    }
    return this->sa_[index];
  }

  void indexer::get_string(sa_index index, char *buf, sa_index size, const char *seps) {
    if (index < 0 || this->sa_.size() <= index) {
      throw "error at indexer::get_string(). index is out if range.";
    }
    sa_index offset = this->sa_[index];

    size--;
    int i = 0;
    while (i < size) {
      if (offset >= this->mr_.size()) { goto LOOP_END; }
      buf[i] = this->mr_[offset++];
      for (int j = 0; seps[j] != '\0'; j++) { if (seps[j] == buf[i]) { goto LOOP_END; } }
      i++;
    }
LOOP_END:
    buf[i] = '\0';
    return;
  }

  void indexer::read(const char *filename) {
    FILE *fin = fopen(filename, "rb");
    if (fin == NULL) {
      throw "error at indexer::read(). filename cannot open.";
    }
    reader r(fin);
    this->sa_.clear();
    r.read(&(this->sa_));
    fclose(fin);
    return;
  }

  void indexer::write(const char *filename) {
    FILE *fout = fopen(filename, "wb");
    if (fout == NULL) {
      throw "error at indexer::write(). filename cannot open.";
    }
    writer w(fout);
    w.write(&(this->sa_));
    fclose(fout);
    return;
  }

  ///////////////////////
  // PRIVATE FUNCTIONS //
  ///////////////////////
  int indexer::compare2key(sa_index offset, const char *key) {
    int ret = 0;
    while (ret == 0) {
      if (offset >= this->mr_.size()) { return 1; } // key is greater
      ret = this->mr_[offset++];
      ret = *key - ret;
      key++;
      if (*key == '\0') { break; }
    }
    return ret;
  }

  sa_index indexer::binary_search(const char *key, sa_index begin, sa_index end) {
    if (begin >= end) { return -1; }
    sa_index pivot = (begin + end) / 2;
    int ret = this->compare2key(this->sa_[pivot], key);

    if      (ret < 0) { return this->binary_search(key, begin, pivot); }
    else if (ret > 0) { return this->binary_search(key, pivot + 1, end); }
    return pivot;
  }

  ////////////////////////////////////////////////////////////////

  //////////////////////////
  // CONSTRUCT & DESTRUCT //
  //////////////////////////
  reader::reader(FILE *fin) : fin_(fin) {
    return;
  }

  reader::~reader() {
    return;
  }

  //////////////////////
  // PUBLIC FUNCTIONS //
  //////////////////////
  void reader::read(vector<sa_index> *psa) {
    if (is_little_endian()) {
      sa_index size = this->read_little();
      for (int i = 0; i < size; i++) { psa->push_back(this->read_little()); }
    } else {
      sa_index size = this->read_big();
      for (int i = 0; i < size; i++) { psa->push_back(this->read_big()); }
    }
    return;
  }

  ///////////////////////
  // PRIVATE FUNCTIONS //
  ///////////////////////
  sa_index reader::read_little() {
    sa_index index;
    if (fread(&index, sizeof(sa_index), 1, this->fin_) < 1) {
      throw "error at reader::read_little(). fread() failure.";
    }
    return index;
  }

  sa_index reader::read_big() {
    sa_index index = this->read_little();
    sa_index rev_index;
    char *p  = (char *)(&index);
    char *rp = (char *)(&rev_index);
    int  n   = sizeof(sa_index);
    for (int i = 0; i < n; i++) { rp[i] = p[i]; }
    return rev_index;
  }

  ////////////////////////////////////////////////////////////////

  //////////////////////////
  // CONSTRUCT & DESTRUCT //
  //////////////////////////
  writer::writer(FILE *fout) : fout_(fout) {
    return;
  }

  writer::~writer() {
    return;
  }

  //////////////////////
  // PUBLIC FUNCTIONS //
  //////////////////////
  void writer::write(vector<sa_index> *psa) {
    vector<sa_index>::iterator i = psa->begin();
    if (is_little_endian()) {
      this->write_little(psa->size());
      while (i != psa->end()) { this->write_little(*i); i++; }
    } else {
      this->write_big(psa->size());
      while (i != psa->end()) { this->write_big(*i); i++; }
    }
    return;
  }

  ///////////////////////
  // PRIVATE FUNCTIONS //
  ///////////////////////
  void writer::write_little(sa_index index) {
    if (fwrite(&index, sizeof(sa_index), 1, this->fout_) < 1) {
      throw "error at writer::write_little(). fwrite() failure.";
    }
    return;
  }

  void writer::write_big(sa_index index) {
    sa_index rev_index;
    char *p  = (char *)(&index);
    char *rp = (char *)(&rev_index);
    int  n   = sizeof(sa_index);
    for (int i = 0; i < n; i++) { rp[i] = p[i]; }
    this->write_little(rev_index);
    return;
  }
}

