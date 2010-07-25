// $Id$
#ifndef TSUBOMI_VERTICAL_CODE
#define TSUBOMI_VERTICAL_CODE
#include <iostream>
#include <fstream>
#include <vector>

namespace tsubomi {
  typedef unsigned long long ullong;
  typedef unsigned int       uint;

  class vertical_code {
    std::vector<uint>   cMSB_;
    std::vector<ullong> base_;
    std::vector<ullong> V_;
    ullong              size_;
    ullong              block_size_;

    vertical_code(const vertical_code &);
    vertical_code &operator=(const vertical_code &);

    ullong popcount64(ullong x) {
      x = ((x & 0xaaaaaaaaaaaaaaaaULL) >> 1) 
        +  (x & 0x5555555555555555ULL); 
      x = ((x & 0xccccccccccccccccULL) >> 2) 
        +  (x & 0x3333333333333333ULL); 
      x = ((x & 0xf0f0f0f0f0f0f0f0ULL) >> 4) 
        +  (x & 0x0f0f0f0f0f0f0f0fULL); 
      x = ((x & 0xff00ff00ff00ff00ULL) >> 8) 
        +  (x & 0x00ff00ff00ff00ffULL); 
      x = ((x & 0xffff0000ffff0000ULL) >> 16) 
        +  (x & 0x0000ffff0000ffffULL); 
      x = ((x & 0xffffffff00000000ULL) >> 32) 
        +  (x & 0x00000000ffffffffULL); 
      return x; 
    }

  public:

    vertical_code() : size_(0) {
      this->cMSB_.push_back(0);
      this->base_.push_back(0);
      this->block_size_ = sizeof(ullong) * 8;
      return;
    }
    ~vertical_code() {}

    void push(ullong d) {
      // add element to vector if size is over
      ullong q = this->size_ % this->block_size_;
      if (q == 0) {
        this->cMSB_.push_back(*(this->cMSB_.rbegin()));
        this->base_.push_back(*(this->base_.rbegin()));
      }

      // get MSB
      std::vector<uint>::reverse_iterator icMSB = this->cMSB_.rbegin();
      uint MSB = *icMSB;
      icMSB++;
      uint preMSB = *icMSB;
      MSB -= preMSB;

      // get SB
      uint SB = this->block_size_;
      while ((SB > 0) && (((d & (1ULL << (SB - 1))) >> (SB - 1)) == 0)) { SB--; }

      // update cMSB & base
      for (uint i = MSB; i < SB; i++) {
        (*(this->cMSB_.rbegin()))++;
        this->V_.push_back(0);
      }
      (*(this->base_.rbegin())) += d;

      // update V
      for (uint i = 0; i < SB; i++) {
        this->V_[preMSB + i] |= (((d & (1ULL << i)) >> i) << q);
      }
      this->size_++;
      return;
    }

    ullong get(ullong pos) {
      if (pos < 0 || this->size_ <= pos) {
        throw "error at vertical_code::get(). pos is out of range.";
      }

      ullong p     = pos / this->block_size_;
      ullong q     = (pos % this->block_size_) + 1ULL;
      uint   begin = this->cMSB_[p];
      uint   end   = this->cMSB_[p + 1];

      ullong value = this->base_[p];
      if (q == this->block_size_) {
        for (uint i = begin; i < end; i++) {
          value += (this->popcount64(this->V_[i])
                    << (i - begin));
        }
      } else {
        for (uint i = begin; i < end; i++) {
          value += (this->popcount64((this->V_[i] & ((1ULL << q) - 1)))
                    << (i - begin));
        }
      }
      return value;
    }
  };
}

#endif
