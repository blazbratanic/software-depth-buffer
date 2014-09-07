#ifndef _ARRAYND_H_
#define _ARRAYND_H_

#include <memory>
#include <stdexcept>
#include <util/array2dview.h>

// simple 2d array with linear storage on the heap
template <class T, class TAllocator = std::allocator<T> >
class Array2d : public Array2dView<T> {
 public:
  Array2d() : size0_(0), size1_(0), data_size_(0) {}

  Array2d(int size0, int size1) : Array2d() { Allocate(size0, size1); }

  Array2d(T* ptr, int size0, int size1)
      : size0_(size0), size1_(size1), data_size_(size0 * size1) {
    data_ = std::shared_ptr<T>(ptr);
  }

  Array2d(Array2d const& rhs)
      : size0_(rhs.size0_),
        size1_(rhs.size1_),
        data_size_(rhs.data_size_),
        data_(std::shared_ptr<T>(rhs.data_)) {}

  Array2d& operator=(Array2d const& rhs) {
    size0_ = rhs.size0_;
    size1_ = rhs.size1_;
    data_size_ = rhs.data_size_;
    data_ = rhs.data_;
  }

  ~Array2d() { data_.reset(); }

  void Free() {
    data_.reset();
    data_size_ = 0;
    size0_ = 0;
    size1_ = 0;
  }

  void Allocate(int size) { Allocate(size, size); }
  void Allocate(std::size_t size) { Allocate(size); }

  // Reallocate only if requested space is larger than currently
  // allocated space. Otherwise just reshape.
  void Allocate(size_t size0, size_t size1) {
    if (size0 * size1 > data_size_) {
      data_ = std::shared_ptr<T>(allocator.allocate(size0 * size1));
      data_size_ = size0 * size1;
    }

    size0_ = size0;
    size1_ = size1;
  }

  template <typename T1, typename T2>
  void Allocate(T1 size0, T2 size1) {
    Allocate(static_cast<size_t>(size0), static_cast<size_t>(size1));
  }

  void Reshape(size_t size0, size_t size1) { Allocate(size0, size1); }

  const T& operator()(ssize_t i0, ssize_t i1) const {
    return data_.get()[i1 + i0 * size1_];
  }
  T& operator()(ssize_t i0, ssize_t i1) {
    return data_.get()[i1 + i0 * size1_];
  }
  const T& operator[](ssize_t k) const { return data_.get()[k]; }
  T& operator[](ssize_t k) { return data_.get()[k]; }

  // getting members.
  int GetSize0() const { return size0_; }
  int GetSize1() const { return size1_; }
  const T* GetData() const { return data_.get(); }
  T* GetData() { return data_.get(); }

 private:
  TAllocator allocator;
  int size0_;  // size for index0
  int size1_;  // size for index1
  int data_size_;
  std::shared_ptr<T> data_;
};

#endif  // _ARRAYND_H_
