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

    void initialize() {
      this->cMSB_.push_back(0);
      this->base_.push_back(0);
      this->size_       = 0;
      this->block_size_ = sizeof(ullong) * 8;
    }

  public:

    vertical_code() {
      this->initialize();
      return;
    }
    vertical_code(std::ifstream &ifs) {
      this->initialize();
      if (!(this->read(ifs))) {
        throw "error at vertical_code::vertical_code(). read() returns false.";
      }
      return;
    }
    vertical_code(const char *filename) {
      this->initialize();
      if (!(this->read(filename))) {
        throw "error at vertical_code::vertical_code(). read() returns false.";
      }
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

    bool read(std::ifstream &ifs) {
      ullong buf_ull;
      uint   buf_ui;

      ifs.read((char *)&(this->size_), sizeof(ullong));
      if (ifs.eof()) { return false; }
      ullong block_num = ((this->size_ + this->block_size_ - 1) / this->block_size_)
                         + 1;

      this->cMSB_.clear();
      while(1) {
        ifs.read((char *)&buf_ui, sizeof(uint));
        if (ifs.eof()) { return false; }
        this->cMSB_.push_back(buf_ui);
        if (this->cMSB_.size() >= block_num) { break; }
      }

      this->base_.clear();
      while(1) {
        ifs.read((char *)&buf_ull, sizeof(ullong));
        if (ifs.eof()) { return false; }
        this->base_.push_back(buf_ull);
        if (this->base_.size() >= block_num) { break; }
      }

      this->V_.clear();
      while(1) {
        ifs.read((char *)&buf_ull, sizeof(ullong));
        if (ifs.eof()) { return false; }
        this->V_.push_back(buf_ull);
        if (this->V_.size() >= *(this->cMSB_.rbegin())) { break; }
      }
      return true;
    }
    bool read(const char *filename) {
      std::ifstream ifs;
      ifs.open(filename, std::ios::in | std::ios::binary);
      if (!ifs) { throw "error at vertical_code::read(). file cannot open for read."; }
      bool ret = this->read(ifs);
      ifs.close();
      return ret;
    }

    void write(std::ofstream &ofs) {
      ofs.write((char *)&(this->size_), sizeof(ullong));
      for (ullong i = 0; i < this->cMSB_.size(); i++) {
        ofs.write((char *)&(this->cMSB_[i]), sizeof(uint));
      }
      for (ullong i = 0; i < this->base_.size(); i++) {
        ofs.write((char *)&(this->base_[i]), sizeof(ullong));
      }
      for (ullong i = 0; i < this->V_.size(); i++) {
        ofs.write((char *)&(this->V_[i]), sizeof(ullong));
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
      if (!ofs) { throw "error at vertical_code::write(). file cannot open for write."; }
      this->write(ofs);
      ofs.close();
      return;
    }
  };

  inline void read_vertical_codes(const char *filename,
                                  std::vector<vertical_code *> &vcs) {
    std::ifstream ifs;
    ifs.open(filename, std::ios::in | std::ios::binary);
    if (!ifs) { throw "error at read_vertical_codes(). file cannot open for read."; }

    while (!ifs.eof()) {
      vertical_code *pvc = new vertical_code();
      if (!(pvc->read(ifs))) { break; }
      vcs.push_back(pvc);
    }
    ifs.close();
    return;
  }

  inline void write_vertical_codes(const char *filename,
                                   std::vector<vertical_code *> &vcs) {
    std::ofstream ofs;
    ofs.open(filename, std::ios::out | std::ios::binary | std::ios::trunc);
    if (!ofs) { throw "error at write_vertical_codes(). file cannot open for write."; }

    std::vector<vertical_code *>::iterator it = vcs.begin();
    while (it != vcs.end()) {
      (*it)->write(ofs);
      it++;
    }
    ofs.close();
    return;
  }

  inline void clear_vertical_codes(std::vector<vertical_code *> &vcs) {
    std::vector<vertical_code *>::iterator it = vcs.begin();
    while (it != vcs.end()) {
      if (*it) { delete *it; }
      it++;
    }
    vcs.clear();
    return;
  }
}

#endif
