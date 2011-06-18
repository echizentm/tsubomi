// $Id$
#ifndef ERIKA_BASIC_TRIE
#define ERIKA_BASIC_TRIE
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include "tsubomi_vertical_code.h"

namespace erika {
  const char utf8_char_size[] = {
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
    3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,5,5,5,5,6,6,1,1
  };

  class node {
  public:
    std::string label_;
    bool        is_value_;
    node(const std::string &label = "", bool is_value = false)
     : label_(label), is_value_(is_value) {}
    ~node() {}
  };

  class element {
  public:
    node             n_;
    std::vector<int> a_;
    element(const node &n)
     : n_(n) {}
    ~element() {}
  };

  typedef std::pair<std::string, std::string> trie_result;
  typedef std::vector<trie_result > trie_results;
  class trie {
  public:
    virtual ~trie() {}
    virtual void search(const std::string &key, trie_results &values,
                        int depth = -1) = 0;
    virtual bool read(std::ifstream &ifs) = 0;
    virtual bool read(const char *filename) = 0;
    virtual void write(std::ofstream &ofs) = 0;
    virtual void write(const char *filename, bool is_append = false) = 0;
  };

  class basic_trie : public trie {
    friend class louds_trie;
    std::vector<element> g_;

    basic_trie(const basic_trie &);
    const basic_trie operator=(const basic_trie &);

    void retrieve(int pos, const std::string &key, trie_results &values,
                  int depth) {
      std::vector<int>::iterator ia = this->g_[pos].a_.begin();
      std::vector<int>::iterator ea = this->g_[pos].a_.end();
      while (ia != ea) {
        node &n = this->g_[*ia].n_;
        if(n.is_value_) {
          values.push_back(trie_result(key, n.label_));
        } else if (depth > 0) {
          this->retrieve(*ia, key + n.label_, values, depth - 1);
        } else if (depth == -1) {
          this->retrieve(*ia, key + n.label_, values, depth);
        }
        ia++;
      }
      return;
    }
  public:
    basic_trie() {
      this->g_.push_back(element(node()));
    }
    ~basic_trie() {}
    void add(const std::string key, const std::string value) {
      int  pos = 0;
      char d   = 0;
      const char *key_c = key.c_str();
      for (int i = 0; key_c[i] != '\0'; i += d) {
        d = utf8_char_size[(unsigned char)(key_c[i])];
        std::string label = key.substr(i, d);
        std::vector<int>::iterator ia = this->g_[pos].a_.begin();
        std::vector<int>::iterator ea = this->g_[pos].a_.end();
        while (ia != ea) {
          node &n = this->g_[*ia].n_;
          if(n.is_value_ == false && n.label_ == label) { break; }
          ia++;
        }
        if (ia == ea) {
          this->g_.push_back(element(node(label)));
          int next_pos = this->g_.size() - 1;
          this->g_[pos].a_.push_back(next_pos);
          pos = next_pos;
        } else {
          pos = *ia;
        }
      }
      this->g_.push_back(element(node(value, true)));
      int next_pos = this->g_.size() - 1;
      this->g_[pos].a_.push_back(next_pos);
      return;
    }
    void dump() {
      std::vector<element>::iterator i = this->g_.begin();
      std::vector<element>::iterator e = this->g_.end();
      while (i != e) {
        std::cout << i->n_.label_ << "(" << i->n_.is_value_ << "): ";
        std::vector<int>::iterator ia = i->a_.begin();
        std::vector<int>::iterator ea = i->a_.end();
        while (ia != ea) {
          std::cout << *ia << ", ";
          ia++;
        }
        std::cout << std::endl;
        i++;
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
        std::vector<int>::iterator ia = this->g_[pos].a_.begin();
        std::vector<int>::iterator ea = this->g_[pos].a_.end();
        while (ia != ea) {
          node &n = this->g_[*ia].n_;
          if(n.is_value_ == false && n.label_ == label) { break; }
          ia++;
        }
        if (ia == ea) { return; }
        pos = *ia;
      }
      this->retrieve(pos, key, values, depth);
      return;
    }

    bool read(std::ifstream &ifs) {
      this->g_.clear();
      int g_size = 0;
      ifs.read((char *)&g_size, sizeof(int));
      if (ifs.eof()) { return false; }

      for (int i = 0; i < g_size; i++) {
        int len = 0;
        ifs.read((char *)&len, sizeof(int));
        if (ifs.eof()) { return false; }
        char *buf = new char[len];
        ifs.read((char *)buf, sizeof(char) * len);
        if (ifs.eof()) { return false; }
        element e = element(node(std::string(buf, len)));
        delete [] buf;
        ifs.read((char *)&(e.n_.is_value_), sizeof(bool));
        if (ifs.eof()) { return false; }
        int a_size = 0;
        ifs.read((char *)&a_size, sizeof(int));
        if (ifs.eof()) { return false; }
        for (int j = 0; j < a_size; j++) {
          int pos = 0;
          ifs.read((char *)&pos, sizeof(int));
          if (ifs.eof()) { return false; }
          e.a_.push_back(pos);
        }
        this->g_.push_back(e);
      }
      return true;
    }
    bool read(const char *filename) {
      std::ifstream ifs;
      ifs.open(filename, std::ios::in | std::ios::binary);
      if (!ifs) {
        throw "error at basic_trie::read(). file cannot open for read.";
      }
      bool ret = this->read(ifs);
      ifs.close();
      return ret;
    }

    void write(std::ofstream &ofs) {
      int g_size = this->g_.size();
      ofs.write((char *)&g_size, sizeof(int));
      std::vector<element>::iterator i = this->g_.begin();
      std::vector<element>::iterator e = this->g_.end();
      while (i != e) {
        const char *buf = i->n_.label_.c_str();
        int len = i->n_.label_.length();
        ofs.write((char *)&len, sizeof(int));
        ofs.write((char *)buf, sizeof(char) * len);
        ofs.write((char *)&(i->n_.is_value_), sizeof(bool));
        int a_size = i->a_.size();
        ofs.write((char *)&a_size, sizeof(int));
        std::vector<int>::iterator ia = i->a_.begin();
        std::vector<int>::iterator ea = i->a_.end();
        while (ia != ea) {
          int pos = *ia;
          ofs.write((char *)&pos, sizeof(int));
          ia++;
        }
        i++;
      }
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
        throw "error at basic_trie::write(). file cannot open for write.";
      }
      this->write(ofs);
      ofs.close();
      return;
    }
  };
}

#endif

