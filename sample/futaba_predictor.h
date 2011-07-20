#ifndef FUTABA_PREDICTOR
#define FUTABA_PREDICTOR

#include <map>
#include <queue>
#include <vector>
#include <iostream>
#include <cstdlib>
#include "tsubomi_basic_searcher.h"

namespace futaba {
  typedef std::pair<std::string, double> k2v_type;
  typedef std::pair<double, std::string> v2k_type;
  typedef std::map<std::string, double>  vector_type;
  typedef std::vector<k2v_type >         result_type;
  class data {
  public:
    std::string c_;
    vector_type v_;
    data(const char *s) {
      const char *p  = s;
      int        pos = 0;
      while (p[pos] != '\0' && p[pos] != '\t') { pos++; }
      this->c_ = std::string(p, pos); p += pos; pos = 0;
      while (p[pos] != '\0') {
        while (p[pos] != '\0' && p[pos] != ':') { pos++; }
        std::string key   = std::string(p + 1, pos - 1); p += pos; pos = 0;
        while (p[pos] != '\0' && p[pos] != ',') { pos++; }
        std::string value = std::string(p + 1, pos - 1); p += pos; pos = 0;
        this->v_[key] = atof(value.c_str());
      }
      return;
    }
    ~data(){}
  };

  class predictor {
    tsubomi::basic_searcher s_;
  public:
    predictor(const char *filename) : s_(filename) {
      return;
    }
    ~predictor() {}

    result_type predict(data &d, int n) {
      vector_type::iterator i = d.v_.begin();
      vector_type::iterator e = d.v_.end();
      vector_type result;
      char buf[65536];
      while (i != e) {
        tsubomi::sa_range r = this->s_.search(i->first.c_str());
        if (r.first < 0) { i++; continue; }
        for (tsubomi::sa_index j = r.first; j <= r.second; j++) {
          bool f = this->s_.get_line(j, buf, 65536);
          if (!f) { throw "error at predictor::predict(). buf is not enough."; }
          data d2(buf);
          vector_type::iterator i2 = d2.v_.begin();
          vector_type::iterator e2 = d2.v_.end();
          while (i2 != e2) {
            result[i2->first] += i->second * i2->second;
            i2++;
          }
        }
        i++;
      }
      std::priority_queue<v2k_type > q;
      i = result.begin();
      e = result.end();
      while (i != e) { q.push(v2k_type(i->second, i->first)); i++; }

      result_type result_top_n;
      while (!(q.empty()) && result_top_n.size() < n) {
        v2k_type p = q.top();
        q.pop();
        result_top_n.push_back(k2v_type(p.second, p.first));
      }
      return result_top_n;
    }
  };
}


#endif
