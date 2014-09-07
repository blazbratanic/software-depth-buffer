#ifndef _ARRAY3D_H_
#define _ARRAY3D_H_

#ifndef throw_line
#ifndef _THROW_LINE_STD_EXCEPTION
#define throw_line(msg) throw std::runtime_error("msg")
#else
#define throw_line(msg) throw std::runtime_error("msg")
#endif
#endif

#include <complex>
#include <algorithm>
#include <memory>  // for std::allocator
#include <cstring>
#include <stdexcept>

template <class T, class TAllocator = std::allocator<T> >
class Array3d {
 private:
  TAllocator allocator;
  int size0;  // size for index0
  int size1;  // size for index1
  int size2;  // size for index2
  int data_size_;
  T* data;

  bool bDataOwner;

  void DeleteData() {
    if ((data) && bDataOwner) {
      //  should we destroy first (call destructors? array 2d should be used
      // with primitive types.)
      //  allocator.destroy(data[0]) ...
      allocator.deallocate(data, data_size_);
    }
  }

 public:
  Array3d()
      : size0(0),
        size1(0),
        size2(0),
        data_size_(0),
        data(NULL),
        bDataOwner(false) {}

  Array3d(T* data_, size_t size0_, size_t size1_, size_t size2_)
      : size0(size0_),
        size1(size1_),
        size2(size2_),
        data_size_(size0_ * size1_ * size2_),
        data(data_),
        bDataOwner(false) {}
  ~Array3d() { DeleteData(); }

  void Zero() {
    if (data) {
      memset(data, 0, sizeof(T) * size0 * size1 * size2);
    } else {
      throw_line("Data not initialized.");
    }
  }

  void Fill(const T& value) {
    if (data) {
      for (std::size_t idx = 0; idx < (size0 * size1 * size2); ++idx) {
        data[idx] = value;
      }
    } else {
      throw_line("Data not initialized.");
    }
  }

  void Free() {
    DeleteData();
    this->data = NULL;
    data_size_ = 0;
    this->size0 = 0;
    this->size1 = 0;
    this->size2 = 0;
    this->bDataOwner = false;
  }

  void Allocate(int size) { Allocate(size, size, size); }
  void Allocate(std::size_t size) { Allocate((int)size); }

  void Allocate(int size0, int size1, int size2) {
    // cheap 'allocation' = only allocate if larger storage is needed
    // or if we didn't own data.
    if (((size0 * size1 * size2) > data_size_) || (bDataOwner == false)) {
      // we need to reallocate.
      DeleteData();

      this->data = allocator.allocate(size0 * size1 * size2);
      this->bDataOwner = true;
      data_size_ = size0 * size1 * size2;
    }

    this->size0 = size0;
    this->size1 = size1;
    this->size2 = size2;
  }
  template <typename T1, typename T2, typename T3>
  void Allocate(T1 size0, T2 size1, T3 size2) {
    Allocate(static_cast<int>(size0), static_cast<int>(size1),
             static_cast<int>(size2));
  }

  void Reshape(int size0, int size1, int size2) {
    if ((size0 * size1 * size2) > data_size_) {
      throw_line("Reshape dimensions larger than underlying array.");
    } else {
      // reinterpret the indices
      this->size0 = size0;
      this->size1 = size1;
      this->size2 = size2;
    }
  }

  void TakeData(T* newdata, int size0, int size1, int size2,
                bool bTakeOwnership = false) {
    // dangerous. assuming that newdata is a valid array
    // atleast size0*size1*size2 in size
    // array will be freed by this object
    DeleteData();

    this->size0 = size0;
    this->size1 = size1;
    this->size2 = size2;
    data_size_ = size0 * size1 * size2;  // at least this big is assumed
    this->data = newdata;
    this->bDataOwner = bTakeOwnership;
  }

  T* DisownData(int* arraySize = NULL, int* psize0 = NULL, int* psize1 = NULL,
                int* psize2 = NULL) {
    T* ret = this->data;
    if (arraySize) *arraySize = data_size_;
    if (psize0) *psize0 = this->size0;
    if (psize1) *psize1 = this->size1;
    if (psize2) *psize2 = this->size2;

    this->data = NULL;
    data_size_ = 0;
    this->size0 = 0;
    this->size1 = 0;
    this->size2 = 0;
    this->bDataOwner = false;

    return ret;
  }

  void Copy(const Array3d<T>& array) {
    CopyData(array.GetData(), array.GetSize0(), array.GetSize1(),
             array.GetSize2());
  }

  void CopyData(const T* srcdata, int size0, int size1, int size2) {
    this->size0 = size0;
    this->size1 = size1;
    this->size2 = size2;

    Allocate(size0, size1, size2);

    // TODO intitialize storage.
    memcpy(this->data, srcdata, sizeof(T) * size0 * size1 * size2);
  }

  // getting members.
  int GetSize0() const { return size0; }
  int GetSize1() const { return size1; }
  int GetSize2() const { return size2; }
  const T* GetData() const { return data; }
  T* GetData() { return data; }

  // accessing elements in a linear fashion.
  T& operator[](std::size_t idx) { return data[idx]; }
  T operator[](std::size_t idx) const { return data[idx]; }

  // accessing by indices
  T& operator()(int i0, int i1, int i2) {
    return data[i2 + size2 * (i1 + i0 * size1)];
  }
  T operator()(int i0, int i1, int i2) const {
    return data[i2 + size2 * (i1 + i0 * size1)];
  }
};

// typedef Array3d<float> float3d;
typedef Array3d<double> double3d;
typedef Array3d<unsigned char> uchar3d;
typedef Array3d<int> int3d;
typedef Array3d<unsigned int> uint3d;

#endif  // _ARRAYND_H_
