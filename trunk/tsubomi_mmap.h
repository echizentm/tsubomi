// $Id$
#ifndef TSUBOMI_MMAP
#define TSUBOMI_MMAP
#include "tsubomi_defs.h"
#include <string>

#ifdef  WIN32
#define TSUBOMI_WIN
#endif
#ifdef  WIN64
#define TSUBOMI_WIN
#endif

#ifdef  TSUBOMI_WIN
#include <windows.h>
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#endif


namespace tsubomi {
  using namespace std;

  template<class T> class mmap_reader {
#ifdef  TSUBOMI_WIN
    HANDLE   hFile_;
    HANDLE   hMap_;
#endif
    void     *map_;
    sa_index size_byte_;
    sa_index size_;

  public:
    mmap_reader(const string &filename);
    ~mmap_reader();
    sa_index size_byte() { return this->size_byte_; }
    sa_index size() { return this->size_; }
    T operator[](sa_index offset) {
      T *tmp = (T *)(this->map_);
      return *(tmp + offset);
    }
  private:
    mmap_reader(const mmap_reader &);
    const mmap_reader &operator=(const mmap_reader &);
  };

  template<class T> mmap_reader<T>::mmap_reader(const string &filename) {
#ifdef  TSUBOMI_WIN
    this->hFile_ = CreateFile(filename.c_str(), GENERIC_READ, 0, NULL, 
                              OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (this->hFile_ == INVALID_HANDLE_VALUE) {
      throw "error at tsubomi::mmap_reader(). cannot open file handle.";
    }
    this->size_byte_ = GetFileSize(this->hFile_, NULL);
    this->size_      = this->size_byte_ / sizeof(T);

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

#else
    int fd = open(filename.c_str(), O_RDONLY);
    if (fd == -1) {
      throw "error at tsubomi::mmap_reader(). failure open().";
    }

    struct stat st;
    int ret = fstat(fd, &st);
    if (ret != 0) {
      close(fd);
      throw "error at tsubomi::mmap_reader(). failure fstat().";
    }
    this->size_byte_ = st.st_size;
    this->size_      = this->size_byte_ / sizeof(T);

    this->map_ = mmap(NULL, this->size_byte_, PROT_READ, MAP_SHARED, fd, 0);
    close(fd);
    if (this->map_ == MAP_FAILED) {
      throw "error at tsubomi::mmap_reader(). failure mmap().";
    }
#endif
    return;
  }

  template<class T> mmap_reader<T>::~mmap_reader() {
#ifdef  TSUBOMI_WIN
    UnmapViewOfFile(this->map_);
    CloseHandle(this->hMap_);
    CloseHandle(this->hFile_);
#else
    munmap(this->map_, this->size_byte_);
#endif
    return;
  }
}

#endif

