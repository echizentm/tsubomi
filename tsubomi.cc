// $Id$
#include "tsubomi.h"

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
    if (this->fin_ == NULL) { throw "error at indexer::indexer(). filename cannot open."; }
    string aryname = string(filename);
    aryname += ".ary";
    FILE *fin_ary = fopen(aryname.c_str(), "rb");
    if (fin_ary == NULL) { throw "error at indexer::indexer(). filename.ary cannot open."; }
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
  void indexer::mkary(const char *seps = "");
  pair<long, long> indexer::search(const char *key);
  long indexer::get_offset(long index);
  void indexer::get_string(long index, char *buf, long size, const char *seps = "");
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
  int indexer::compare2key(long offset, const char *key);
  long indexer::binary_search(const char *key, long begin, long end);

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
    else {
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
    else {
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

