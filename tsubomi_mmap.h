// $Id$
#ifndef TSUBOMI_MMAP
#define TSUBOMI_MMAP
#include "tsubomi_defs.h"
#include <windows.h>

namespace tsubomi {
  using namespace std;

  template<class T> class mmap_reader {
    HANDLE   hFile_;
    HANDLE   hMap_;
    void     *map_;
    sa_index size_;

  public:
    mmap_reader(const char *filename);
    ~mmap_reader();
    sa_index size() { return this->size_; }
    T operator[](sa_index offset) {
      T *tmp = (T *)(this->map_);
      return *(tmp + offset);
    }
  };

  template<class T> mmap_reader<T>::mmap_reader(const char *filename) {
    this->hFile_ = CreateFile(filename, GENERIC_READ, 0, NULL, 
                              OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (this->hFile_ == INVALID_HANDLE_VALUE) {
      throw "error at tsubomi::mmap_reader(). cannot open file handle.";
    }
    this->size_ = GetFileSize(this->hFile_, NULL);

    this->hMap_ = CreateFileMapping(this->hFile_, NULL, 
                                   PAGE_READONLY, 0, 0, NULL);
    if (this->hMap_ == NULL) {
      CloseHandle(this->hFile_);
      throw "error at tsubomi::mmap_reader(). cannot open map handle.";
    }

    this->map_ = MapViewOfFile(this->hMap_, FILE_MAP_READ, 0, 0, 0);
    if (this->map_ == NULL) {
      CloseHandle(this->hMap_);
      CloseHandle(this->hFile_);
      throw "error at tsubomi::mmap_reader(). cannot map view of file.";
    }
    return;
  }

  template<class T> mmap_reader<T>::~mmap_reader() {
    UnmapViewOfFile(this->map_);
    CloseHandle(this->hMap_);
    CloseHandle(this->hFile_);
    return;
  }
}

#endif

