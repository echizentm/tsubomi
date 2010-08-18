// $Id$
#ifndef TSUBOMI_DEFS
#define TSUBOMI_DEFS
#include <map>
#include <iostream>

namespace tsubomi {
  typedef int                           sa_index;
  typedef std::pair<sa_index, sa_index> sa_range;

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
        if (star_ < 40) { std::cerr << "*"; this->star_++; }
        this->cur_ = 0;
      }
      return;
    }
  private:
    progress_bar(const progress_bar &);
    const progress_bar &operator=(const progress_bar &);
  };

}

#endif

