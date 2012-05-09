// $Id$
#include "tsubomi_basic_searcher.h"

namespace tsubomi {
  using namespace std;

  ////////////////////////////////////////////////////////////////
  // class basic_searcher
  basic_searcher::basic_searcher(const char *filename)
   : mr_file_(filename), mr_sa_(string(filename) + ".ary") {
    return;
  }
  basic_searcher::~basic_searcher() {
    return;
  }

  sa_range basic_searcher::search(const char *key) {
    return this->search(key, 0, this->mr_sa_.size() - 1);
  }

  sa_range basic_searcher::search(const char *key, sa_index begin, sa_index end) {
    sa_index index = this->binary_search(key, begin, end);
    if (index == -1) { return pair<sa_index, sa_index>(-1, -1); }

    sa_index begin2 = index - 1;
    sa_index end2   = index + 1;
    while (begin2 >= begin && this->compare2key(this->mr_sa_[begin2], key) == 0) { begin2--; }
    while (end2   <= end   && this->compare2key(this->mr_sa_[end2]  , key) == 0) { end2++; }
    return pair<sa_index, sa_index>(begin2 + 1, end2 - 1);
  }

  bool basic_searcher::get_line(sa_index pos, char *buf, sa_index size,
                                sa_index *pkeypos) {
    if (pos < 0 || this->mr_sa_.size() <= pos) {
      throw "error at searcher::get_line(). pos is out of range.";
    }

    sa_index sa = this->mr_sa_[pos];
    sa_index tmp_sa = sa;
    while (sa > 0 && this->mr_file_[sa - 1] != '\n') { sa--; }
    if (pkeypos) { *pkeypos = (tmp_sa - sa); }

    bool ret = false;
    sa_index i = 0;
    while (i < (size - 1)) {
      buf[i] = this->mr_file_[sa++];
      if (buf[i] == '\n' || buf[i] == '\0') { ret = true; break; }
      i++;
    }
    buf[i] = '\0';
    return ret;
  }

  bool basic_searcher::get_line(sa_index pos, string &buf,
                                sa_index *pkeypos) {
    if (pos < 0 || this->mr_sa_.size() <= pos) {
      throw "error at searcher::get_line(). pos is out of range.";
    }

    sa_index sa = this->mr_sa_[pos];
    sa_index tmp_sa = sa;
    while (sa > 0 && this->mr_file_[sa - 1] != '\n') { sa--; }
    if (pkeypos) { *pkeypos = (tmp_sa - sa); }

    bool ret = false;
    buf = "";
    sa_index i = 0;
    while (1) {
      buf += this->mr_file_[sa++];
      if (buf[i] == '\n' || buf[i] == '\0') { ret = true; break; }
      i++;
    }
    buf[i] = '\0';
    return ret;
  }

  int basic_searcher::compare2key(sa_index offset, const char *key) {
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

  sa_index basic_searcher::binary_search(const char *key, sa_index begin, sa_index end) {
    if (begin > end) { return -1; }
    sa_index pivot = (begin + end) / 2;
    int ret = this->compare2key(this->mr_sa_[pivot], key);

    if      (ret < 0) { return this->binary_search(key, begin, pivot - 1); }
    else if (ret > 0) { return this->binary_search(key, pivot + 1, end); }
    return pivot;
  }
}

