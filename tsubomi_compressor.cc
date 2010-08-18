// $Id$
#include "tsubomi_compressor.h"
#include "tsubomi_mmap.h"
#include <string>
#include <iostream>

namespace tsubomi {
  using namespace std;

  void compressor::mkcsa(const char *filename, bool is_progress) {
    mmap_reader<char>     mr_file(filename);
    mmap_reader<sa_index> mr_sa(string(filename) + ".ary");
    this->size_ = mr_sa.size();

    progress_bar prg(this->size() / 40);
    if (is_progress) {
      cerr << "+--------------------------------------+" << endl;
    }

    int      ch_prev    = 0;
    sa_index index_prev = 0;
    sa_index index      = 0;
    for (sa_index i = 0; i < this->size(); i++) {
      sa_index sa      = mr_sa[i];
      int      ch      = mr_file[sa] + 128;
      int      ch_diff = ch - ch_prev;
      if (ch_diff < 0) { throw "error at compressor::mkcsa(). ch_diff < 0."; }
      if (ch_diff > 0) { index = 0; index_prev = 0; }
      this->pos2ch_.push(ch_diff);

      if (sa == (this->size() - 1)) {
        index = 0;
      } else {
        while (mr_sa[index] != (sa + 1)) {
          index++;
          if (index >= this->size()) {
            throw "error at compressor::mkcsa(). index is out of range.";
          }
        }
      }
      index++;
      this->PSI_[ch].push(index - index_prev);
      index_prev = index;
      ch_prev    = ch;
      if (is_progress) { prg.progress(1); }
    }

    this->PSIhead_.push(0);
    for (int ch = 0; ch < 256; ch++) {
      this->PSIhead_.push(this->PSI_[ch].size());
    }
    if (is_progress) { cerr << endl << "done!" << endl; }
    return;
  }

  inline sa_index compressor::size() {
    return this->size_;
  }

  sa_range compressor::search(const char *key) {
    int      ch    = key[0] + 128;
    sa_index begin = this->PSIhead_.get(ch);
    sa_index end   = this->PSIhead_.get(ch + 1) - 1;

    sa_index index = this->binary_search(key, begin, end);
    if (index == -1) { return pair<sa_index, sa_index>(-1, -1); }

    sa_index begin2 = index - 1;
    sa_index end2   = index + 1;
    while (begin2 >= begin && this->compare2key(begin2, key) == 0) { begin2--; }
    while (end2   <= end   && this->compare2key(end2  , key) == 0) { end2++; }
    return pair<sa_index, sa_index>(begin2 + 1, end2 - 1);
  }

  sa_index compressor::get_suffix(sa_index pos, char *buf, sa_index size, 
                                  const char *seps) {
    sa_index i = 0;
    while (i < (size - 1)) {
      pos = get_next(pos, buf + i);
      for (int j = 0; seps[j] != '\0'; j++) {
        if (buf[i] == seps[j]) { goto END; }
      }
      i++;
      if (pos == -1) { break; }
    }
END:
    buf[i] = '\0';
    return pos;
  }

  void compressor::read(const char *filename) {
    string csaname = string(filename) + ".csa";
    ifstream ifs;
    ifs.open(csaname.c_str(), ios::in | ios::binary);
    if (!ifs) {
      throw "error at compressor::read(). file cannot open for read.";
    }

    ifs.read((char *)&(this->size_), sizeof(sa_index));
    if(!(this->pos2ch_.read(ifs))) { goto END; }
    if(!(this->PSIhead_.read(ifs))) { goto END; }
    for (int ch = 0; ch < 256; ch++) {
      if (!(this->PSI_[ch].read(ifs))) { goto END; }
    }
    ifs.close();
    return;
END:
    ifs.close();
    throw "error at compressor::read(). data cannot read.";
  }

  void compressor::write(const char *filename) {
    string csaname = string(filename) + ".csa";
    ofstream ofs;
    ofs.open(csaname.c_str(), ios::out | ios::binary | ios::trunc);
    if (!ofs) { 
      throw "error at compressor::write(). file cannot open for write."; 
    }

    ofs.write((char *)&(this->size_), sizeof(sa_index));
    this->pos2ch_.write(ofs);
    this->PSIhead_.write(ofs);
    for (int ch = 0; ch < 256; ch++) {
      this->PSI_[ch].write(ofs);
    }
    ofs.close();
    return;
  }

  sa_index compressor::get_next(sa_index pos, char *pch) {
    if (pos < 0 && this->size() <= pos) {
      throw "error at compressor::get_next(). pos is out if range.";
    }
    int ch = this->pos2ch_.get(pos);
    if (pch != NULL) { *pch = char(ch - 128); }
    return this->PSI_[ch].get(pos - this->PSIhead_.get(ch)) - 1;
  }

  int compressor::compare2key(sa_index pos, const char *key) {
    int  ret = 0;
    char ch;
    while (ret == 0) {
      pos = get_next(pos, &ch);
      if (pos == -1) { return 1; } // key is greater
      ret = *key - ch;
      key++;
      if (*key == '\0') { break; }
    }
    return ret;
  }

  sa_index compressor::binary_search(const char *key,
                                     sa_index begin, sa_index end) {
    if (begin > end) { return -1; }
    sa_index pivot = (begin + end) / 2;
    int ret = this->compare2key(pivot, key);

    if      (ret < 0) { return this->binary_search(key, begin, pivot - 1); }
    else if (ret > 0) { return this->binary_search(key, pivot + 1, end); }
    return pivot;
  }
}

