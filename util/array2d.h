#ifndef _ARRAYND_H_
#define _ARRAYND_H_

#include <memory>
#include <stdexcept>
#include <util/array2dview.h>
#include <type_traits>

template <class T, class TAllocator = std::allocator<T> >
class Array2d : public Array2dView<T> {
  typedef Array2dView<T> Base;

 public:
  Array2d() : Base(nullptr, 0, 0, 0), data_size_(0) {}
  Array2d(int size0, int size1) : data_size_(0) { Allocate(size0, size1); }

  Array2d(T* ptr, int size0, int size1) : Base(ptr, size0, size1, size1) {
    data_size_ = size0 * size1;
  }

  Array2d(Array2d const& rhs)
      : Base(rhs.data_, rhs.size0_, rhs.size1_, rhs.size1_) {
    data_ptr_ = rhs.data_ptr_;
    data_size_ = rhs.data_size_;
  }

  Array2d& operator=(Array2d const& rhs) {
    this->size0_ = rhs.size0_;
    this->size1_ = rhs.size1_;
    this->stride_ = rhs.stride_;
    this->data_size_ = rhs.data_size_;
    this->data_ptr_ = rhs.data_ptr_;
    this->data_ = rhs.data_;
    return *this;
  }

  ~Array2d() { this->data_ptr_.reset(); }

  void Free() {
    this->data_ptr_.reset();
    this->data_size_ = 0;
    this->size0_ = 0;
    this->size1_ = 0;
  }

  void Allocate(int size) { Allocate(size, size); }
  void Allocate(std::size_t size) { Allocate(size); }

  // Reallocate only if requested space is larger than currently
  // allocated space. Otherwise just reshape.
  void Allocate(size_t size0, size_t size1) {
    if (size0 * size1 > data_size_) {
      this->data_ptr_ = std::shared_ptr<T>(allocator.allocate(size0 * size1));
      this->data_size_ = size0 * size1;
    }

    this->size0_ = size0;
    this->size1_ = size1;
    this->stride_ = size1;
    this->data_ = data_ptr_.get();
  }

  template <typename T1, typename T2>
  void Allocate(T1 size0, T2 size1) {
    Allocate(static_cast<size_t>(size0), static_cast<size_t>(size1));
  }

  void Reshape(size_t size0, size_t size1) { Allocate(size0, size1); }

 protected:
  TAllocator allocator;
  size_t data_size_;
  std::shared_ptr<T> data_ptr_;
};

#endif  // _ARRAYND_H_
