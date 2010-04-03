// $Id$
#include "tsubomi_searcher.h"

namespace tsubomi {
  using namespace std;

  ////////////////////////////////////////////////////////////////
  // class searcher
  sa_range searcher::search(const char *key) {
    sa_index size  = this->mr_sa_.size();
    sa_index index = this->binary_search(key, 0, size);
    if (index == -1) { return pair<sa_index, sa_index>(-1, -1); }

    sa_index begin = index - 1;
    sa_index end   = index + 1;
    while (begin >= 0 && this->compare2key(this->mr_sa_[begin], key) == 0) { begin--; }
    while (end < size && this->compare2key(this->mr_sa_[end]  , key) == 0) { end++; }
    return pair<sa_index, sa_index>(begin + 1, end - 1);
  }

  sa_index searcher::get_offset(sa_index index) {
    if (index < 0 || this->mr_sa_.size() <= index) {
      throw "error at searcher::get_offset(). index is out of range.";
    }
    return this->mr_sa_[index];
  }

  void searcher::get_string(sa_index index, char *buf, sa_index size, const char *seps) {
    if (index < 0 || this->mr_sa_.size() <= index) {
      throw "error at searcher::get_string(). index is out of range.";
    }
    sa_index offset = this->mr_sa_[index];

    size--;
    int i = 0;
    while (i < size) {
      if (offset >= this->mr_file_.size()) { goto LOOP_END; }
      buf[i] = this->mr_file_[offset++];
      for (int j = 0; seps[j] != '\0'; j++) { if (seps[j] == buf[i]) { goto LOOP_END; } }
      i++;
    }
LOOP_END:
    buf[i] = '\0';
    return;
  }

  int searcher::compare2key(sa_index offset, const char *key) {
    int ret = 0;
    while (ret == 0) {
      if (offset >= this->mr_file_.size()) { return 1; } // key is greater
      ret = this->mr_file_[offset++];
      ret = *key - ret;
      key++;
      if (*key == '\0') { break; }
    }
    return ret;
  }

  sa_index searcher::binary_search(const char *key, sa_index begin, sa_index end) {
    if (begin >= end) { return -1; }
    sa_index pivot = (begin + end) / 2;
    int ret = this->compare2key(this->mr_sa_[pivot], key);

    if      (ret < 0) { return this->binary_search(key, begin, pivot); }
    else if (ret > 0) { return this->binary_search(key, pivot + 1, end); }
    return pivot;
  }
}

