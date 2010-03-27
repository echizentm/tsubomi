// $Id$
#include "tsubomi.h"
#include <algorithm>

namespace tsubomi {
  using namespace std;

  //////////////////////////
  // CONSTRUCT & DESTRUCT //
  //////////////////////////
  indexer::indexer(FILE *fin) : fin_(fin), is_close_(false) {
    return;
  }

  indexer::indexer(const char *filename) {
    this->is_close_ = true;
    this->fin_ = fopen(filename, "r");
    if (this->fin_ == NULL) {
      throw "error at indexer::indexer(). filename cannot open.";
    }

    string aryname = string(filename);
    aryname += ".ary";
    FILE *fin_ary = fopen(aryname.c_str(), "rb");
    if (fin_ary == NULL) {
      fclose(this->fin_);
      throw "error at indexer::indexer(). filename.ary cannot open.";
    }
    this->read(fin_ary);
    fclose(fin_ary);
    return;
  }

  indexer::~indexer() {
    if (this->is_close_ && this->fin_) { fclose(this->fin_); }
    return;
  }

  //////////////////////
  // PUBLIC FUNCTIONS //
  //////////////////////
  void indexer::mkary(const char *seps) {
    long offset = 0;
    int  ch     = '\0';
    this->sa_.clear();
    if (seps[0] == '\0') {
      while ((ch = fgetc(this->fin_)) != EOF) {
        this->sa_.push_back(offset);
        offset++;
      }
    } else {
      bool flg = true;
      while ((ch = fgetc(this->fin_)) != EOF) {
        if (flg) { this->sa_.push_back(offset); flg = false; }
        for (int i = 0; seps[i] != '\0'; i++) { if (ch == seps[i]) { flg = true; } }
        offset++;
      }
    }
    sort(this->sa_.begin(), this->sa_.end(), comparer(this->fin_));
    return;
  }

  pair<long, long> indexer::search(const char *key) {
    long size  = this->sa_.size();
    long index = this->binary_search(key, 0, size);
    if (index == -1) { return pair<long, long>(-1, -1); }

    long begin = index - 1;
    long end   = index + 1;
    while (begin >= 0 && this->compare2key(this->sa_[begin], key) == 0) { begin--; }
    while (end < size && this->compare2key(this->sa_[end]  , key) == 0) { end++; }
    return pair<long, long>(begin + 1, end - 1);
  }

  long indexer::get_offset(long index) {
    if (index < 0 || this->sa_.size() <= index) {
      throw "error at indexer::get_offset(). index is out if range.";
    }
    return this->sa_[index];
  }

  void indexer::get_string(long index, char *buf, long size, const char *seps) {
    if (index < 0 || this->sa_.size() <= index) {
      throw "error at indexer::get_string(). index is out if range.";
    }
    if (fseek(this->fin_, this->sa_[index], SEEK_SET) != 0) {
      throw "error at indexer::get_string(). fseek() failure.";
    }
    size--;
    int i = 0;
    while (i < size) {
      buf[i] = fgetc(this->fin_);
      if (buf[i] == EOF) { goto LOOP_END; }
      for (int j = 0; seps[j] != '\0'; j++) { if (seps[j] == buf[i]) { goto LOOP_END; } }
      i++;
    }
LOOP_END:
    buf[i] = '\0';
    return;
  }

  void indexer::read(FILE *fin) {
    reader r(fin);
    this->sa_.clear();
    r.read(&(this->sa_));
    return;
  }

  void indexer::write(FILE *fout) {
    writer w(fout);
    w.write(&(this->sa_));
    return;
  }

  ///////////////////////
  // PRIVATE FUNCTIONS //
  ///////////////////////
  int indexer::compare2key(long offset, const char *key) {
    if (fseek(this->fin_, offset, SEEK_SET) != 0) {
      throw "error at indexer::compare2key(). fseek() failure.";
    }
    int ret = 0;
    while (ret == 0) {
      ret = fgetc(this->fin_);
      if (ret == EOF) { return 1; } // key is greater
      ret = *key - ret;
      key++;
      if (*key == '\0') { break; }
    }
    return ret;
  }

  long indexer::binary_search(const char *key, long begin, long end) {
    if (begin >= end) { return -1; }
    long pivot = (begin + end) / 2;
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
  void reader::read(vector<long> *psa) {
    if (is_little_endian()) {
      long size = this->read_little();
      for (int i = 0; i < size; i++) { psa->push_back(this->read_little()); }
    } else {
      long size = this->read_big();
      for (int i = 0; i < size; i++) { psa->push_back(this->read_big()); }
    }
    return;
  }

  ///////////////////////
  // PRIVATE FUNCTIONS //
  ///////////////////////
  long reader::read_little() {
    long index;
    if (fread(&index, sizeof(long), 1, this->fin_) < 1) {
      throw "error at reader::read_little(). fread() failure.";
    }
    return index;
  }

  long reader::read_big() {
    long index     = this->read_little();
    char *p        = (char *)(&index);
    long rev_index;
    char *rp       = (char *)(&rev_index);
    int  n         = sizeof(long);
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
  void writer::write(vector<long> *psa) {
    vector<long>::iterator i = psa->begin();
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
  void writer::write_little(long index) {
    if (fwrite(&index, sizeof(long), 1, this->fout_) < 1) {
      throw "error at writer::write_little(). fwrite() failure.";
    }
    return;
  }

  void writer::write_big(long index) {
    char *p        = (char *)(&index);
    long rev_index;
    char *rp       = (char *)(&rev_index);
    int  n         = sizeof(long);
    for (int i = 0; i < n; i++) { rp[i] = p[i]; }
    this->write_little(rev_index);
    return;
  }
}

