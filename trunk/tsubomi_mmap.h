// $Id$
#ifndef TSUBOMI_MMAP
#define TSUBOMI_MMAP
#include "tsubomi_defs.h"

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

  template<class T> class mmap_reader {
#ifdef  TSUBOMI_WIN
    HANDLE   hFile_;
    HANDLE   hMap_;
#endif
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
#ifdef  TSUBOMI_WIN
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

#else
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
      throw "error at tsubomi::mmap_reader(). failure open().";
    }

    struct stat st;
    int ret = fstat(fd, &st);
    if (ret != 0) {
      close(fd);
      throw "error at tsubomi::mmap_reader(). failure fstat().";
    }
    this->size_ = st.st_size;

    this->map_ = mmap(NULL, this->size_, PROT_READ, MAP_SHARED, fd, 0);
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
    munmap(this->map_, this->size_);
#endif
    return;
  }
}

#endif

