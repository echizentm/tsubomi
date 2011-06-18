// $Id$
#ifndef ERIKA_LOUDS_TRIE
#define ERIKA_LOUDS_TRIE
#include <queue>
#include "erika_basic_trie.h"

namespace erika {
  class louds_trie : public trie {
    tsubomi::vertical_code vc_;
    std::vector<node>      nodes_;

    louds_trie(const louds_trie &);
    const louds_trie operator=(const louds_trie &);

    void retrieve(int pos, const std::string &key, trie_results &values,
                  int depth) {
      int ea = this->vc_.get(pos) + 1;
      int ia = ea - this->vc_.diff(pos);
      while (ia < ea) {
        node &n = this->nodes_[ia];
        if(n.is_value_) {
          values.push_back(trie_result(key, n.label_));
        } else if (depth > 0) {
          this->retrieve(ia, key + n.label_, values, depth - 1);
        } else if (depth == -1) {
          this->retrieve(ia, key + n.label_, values, depth);
        }
        ia++;
      }
      return;
    }
  public:
    louds_trie() {}
    ~louds_trie() {}
    void build(const basic_trie &bt) {
      std::queue<int> q;
      q.push(0);
      while (q.size() > 0) {
        int pos = q.front();
        q.pop();
        element e = bt.g_[pos];
        this->vc_.push(e.a_.size());
        this->nodes_.push_back(e.n_);
        std::vector<int>::const_iterator ia = e.a_.begin();
        std::vector<int>::const_iterator ea = e.a_.end();
        while (ia != ea) {
          q.push(*ia);
          ia++;
        }
      }
      return;
    }
    void dump() {
      std::vector<node>::const_iterator i = this->nodes_.begin();
      std::vector<node>::const_iterator e = this->nodes_.end();
      int pos = 0;
      while (i != e) {
        std::cout << i->label_ << "(" << i->is_value_ << "): ";
        std::cout << this->vc_.diff(pos) << std::endl;
        i++;
        pos++;
      }
      return;
    }
    void search(const std::string &key, trie_results &values,
                int depth = -1) {
      values.clear();
      int pos  = 0;
      char d   = 0;
      const char *key_c = key.c_str();
      for (int i = 0; key_c[i] != '\0'; i += d) {
        d = utf8_char_size[(unsigned char)(key_c[i])];
        std::string label = key.substr(i, d);
        int ea = this->vc_.get(pos) + 1;
        int ia = ea - this->vc_.diff(pos);
        while (ia < ea) {
          node &n = this->nodes_[ia];
          if(n.is_value_ == false && n.label_ == label) { break; }
          ia++;
        }
        if (ia >= ea) { return; }
        pos = ia;
      }
      this->retrieve(pos, key, values, depth);
      return;
    }

    bool read(std::ifstream &ifs) {
      this->nodes_.clear();
      int n_size = 0;
      ifs.read((char *)&n_size, sizeof(int));
      if (ifs.eof()) { return false; }

      for (int i = 0; i < n_size; i++) {
        int len = 0;
        ifs.read((char *)&len, sizeof(int));
        if (ifs.eof()) { return false; }
        char *buf = new char[len];
        ifs.read((char *)buf, sizeof(char) * len);
        if (ifs.eof()) { return false; }
        node n = node(std::string(buf, len));
        delete [] buf;
        ifs.read((char *)&(n.is_value_), sizeof(bool));
        if (ifs.eof()) { return false; }
        this->nodes_.push_back(n);
      }
      this->vc_.read(ifs);
      return true;
    }
    bool read(const char *filename) {
      std::ifstream ifs;
      ifs.open(filename, std::ios::in | std::ios::binary);
      if (!ifs) {
        throw "error at louds_trie::read(). file cannot open for read.";
      }
      bool ret = this->read(ifs);
      ifs.close();
      return ret;
    }

    void write(std::ofstream &ofs) {
      int n_size = this->nodes_.size();
      ofs.write((char *)&n_size, sizeof(int));
      std::vector<node>::iterator i = this->nodes_.begin();
      std::vector<node>::iterator e = this->nodes_.end();
      while (i != e) {
        const char *buf = i->label_.c_str();
        int len = i->label_.length();
        ofs.write((char *)&len, sizeof(int));
        ofs.write((char *)buf, sizeof(char) * len);
        ofs.write((char *)&(i->is_value_), sizeof(bool));
        i++;
      }
      this->vc_.write(ofs);
      return;
    }
    void write(const char *filename, bool is_append = false) {
      std::ofstream ofs;
      if (is_append) {
        ofs.open(filename, std::ios::out | std::ios::binary | std::ios::app);
      } else {
        ofs.open(filename, std::ios::out | std::ios::binary | std::ios::trunc);
      }
      if (!ofs) {
        throw "error at louds_trie::write(). file cannot open for write.";
      }
      this->write(ofs);
      ofs.close();
      return;
    }
  };
}

#endif

