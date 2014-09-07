#ifndef ARRAY2DVIEW_H_
#define ARRAY2DVIEW_H_

#include <iterator>
#include <stdexcept>
#include <algorithm>
#include <cmath>
#include <type_traits>
#include <cassert>
#include <stddef.h>

#define throw_line(line) throw std::runtime_error(line);

#ifndef CHECK_BOUNDS_IF_ENABLED
#ifdef _ARRAYND_BOUNDS_CHECK_
#define CHECK_BOUNDS_IF_ENABLED(cond) \
  if (!(cond)) throw_line("Bound check failed.");
#define CHECK_IF_ARRAYS_HAVE_SAME_SIZE
#else
#define CHECK_BOUNDS_IF_ENABLED(cond)
#define CHECK_IF_ARRAYS_HAVE_SAME_SIZE
#endif
#endif

#define FOREACH2D(i0, i1, view)                   \
  for (size_t i0 = 0; i0 < view.GetSize0(); ++i0) \
    for (size_t i1 = 0; i1 < view.GetSize1(); ++i1)

namespace sil {

template <typename T1, typename T2>
static inline bool same_size(T1 src1, T2 src2) {
  return (src1.GetSize0() == src2.GetSize0() &&
          src1.GetSize1() == src2.GetSize1());
}

template <typename View>
static inline size_t Stride(View v) {
  return v.GetStride();
}

template <typename T>
class iterator_non_strided
    : public std::iterator<std::random_access_iterator_tag, T> {
 private:
  T *ptr_;

 public:
  typedef std::random_access_iterator_tag iterator_category;
  typedef typename std::iterator<std::random_access_iterator_tag, T>::value_type
      value_type;
  typedef typename std::iterator<std::random_access_iterator_tag,
                                 T>::difference_type difference_type;
  typedef typename std::iterator<std::random_access_iterator_tag, T>::reference
      reference;
  typedef typename std::iterator<std::random_access_iterator_tag, T>::pointer
      pointer;

  iterator_non_strided() : ptr_(NULL) {}

  template <typename T2>
  iterator_non_strided(const iterator_non_strided<T2> &r)
      : ptr_(&(*r)) {}

  iterator_non_strided(pointer pData) : ptr_(pData) {}

  template <typename T2>
  iterator_non_strided &operator=(const iterator_non_strided<T2> &r) {
    ptr_ = &(*r);
    return *this;
  }

  iterator_non_strided &operator++() {
    ++ptr_;
    return *this;
  }

  iterator_non_strided &operator--() {
    --ptr_;
    return *this;
  }

  iterator_non_strided operator++(int) { return iterator_non_strided(ptr_++); }

  iterator_non_strided operator--(int) { return iterator_non_strided(ptr_--); }

  iterator_non_strided operator+(const difference_type &n) const {
    return iterator_non_strided(ptr_ + n);
  }

  iterator_non_strided &operator+=(const difference_type &n) {
    ptr_ += n;
    return *this;
  }

  iterator_non_strided operator-(const difference_type &n) const {
    return iterator_non_strided(pointer(ptr_ - n));
  }

  iterator_non_strided &operator-=(const difference_type &n) {
    ptr_ -= n;
    return *this;
  }

  reference operator*() const { return *ptr_; }

  pointer operator->() const { return ptr_; }

  reference operator[](const difference_type &n) const { return ptr_[n]; }

  template <typename T2>
  friend bool operator==(const iterator_non_strided<T2> &r1,
                         const iterator_non_strided<T2> &r2);

  template <typename T2>
  friend bool operator!=(const iterator_non_strided<T2> &r1,
                         const iterator_non_strided<T2> &r2);

  template <typename T2>
  friend bool operator<(const iterator_non_strided<T2> &r1,
                        const iterator_non_strided<T2> &r2);

  template <typename T2>
  friend bool operator>(const iterator_non_strided<T2> &r1,
                        const iterator_non_strided<T2> &r2);

  template <typename T2>
  friend bool operator<=(const iterator_non_strided<T2> &r1,
                         const iterator_non_strided<T2> &r2);

  template <typename T2>
  friend bool operator>=(const iterator_non_strided<T2> &r1,
                         const iterator_non_strided<T2> &r2);

  template <typename T2>
  friend difference_type operator+(const iterator_non_strided<T2> &r1,
                                   const iterator_non_strided<T2> &r2);

  template <typename T2>
  friend difference_type operator-(const iterator_non_strided<T2> &r1,
                                   const iterator_non_strided<T2> &r2);
};

template <typename T>
bool operator==(const iterator_non_strided<T> &r1,
                const iterator_non_strided<T> &r2) {
  return (r1.ptr_ == r2.ptr_);
}

template <typename T>
bool operator!=(const iterator_non_strided<T> &r1,
                const iterator_non_strided<T> &r2) {
  return (r1.ptr_ != r2.ptr_);
}

template <typename T>
bool operator<(const iterator_non_strided<T> &r1,
               const iterator_non_strided<T> &r2) {
  return (r1.ptr_ < r2.ptr_);
}

template <typename T>
bool operator>(const iterator_non_strided<T> &r1,
               const iterator_non_strided<T> &r2) {
  return (r1.ptr_ > r2.ptr_);
}

template <typename T>
bool operator<=(const iterator_non_strided<T> &r1,
                const iterator_non_strided<T> &r2) {
  return (r1.ptr_ <= r2.ptr_);
}

template <typename T>
bool operator>=(const iterator_non_strided<T> &r1,
                const iterator_non_strided<T> &r2) {
  return (r1.ptr_ >= r2.ptr_);
}

template <typename T>
typename iterator_non_strided<T>::difference_type operator+(
    const iterator_non_strided<T> &r1, const iterator_non_strided<T> &r2) {
  return iterator_non_strided<T>(r1.ptr_ + r2.ptr_);
}

template <typename T>
typename iterator_non_strided<T>::difference_type operator-(
    const iterator_non_strided<T> &r1, const iterator_non_strided<T> &r2) {
  return r1.ptr_ - r2.ptr_;
}

template <typename T>
class iterator_strided
    : public std::iterator<std::random_access_iterator_tag, T> {
 public:
  typedef std::random_access_iterator_tag iterator_category;
  typedef typename std::iterator<std::random_access_iterator_tag, T>::value_type
      value_type;
  typedef typename std::iterator<std::random_access_iterator_tag,
                                 T>::difference_type difference_type;
  typedef typename std::iterator<std::random_access_iterator_tag, T>::reference
      reference;
  typedef typename std::iterator<std::random_access_iterator_tag, T>::pointer
      pointer;

  iterator_strided() : ptr_(NULL) {}

  template <typename T2>
  iterator_strided(const iterator_strided<T2> &r)
      : ptr_(&(*r)), stride_(r.stride_) {}

  iterator_strided(pointer pData, difference_type stride)
      : ptr_(pData), stride_(stride) {}

  template <typename T2>
  iterator_strided &operator=(const iterator_strided<T2> &r) {
    ptr_ = &(*r);
    stride_ = r.stride_;
    return *this;
  }

  iterator_strided &operator++() {
    ptr_ += stride_;
    return *this;
  }

  iterator_strided &operator--() {
    ptr_ -= stride_;
    return *this;
  }

  iterator_strided operator++(int) {
    T *tmp = ptr_;
    ptr_ += stride_;
    return iterator_strided(tmp, stride_);
  }

  iterator_strided operator--(int) {
    T *tmp = ptr_;
    ptr_ -= stride_;
    return iterator_strided(tmp, stride_);
  }

  iterator_strided operator+(const difference_type &n) const {
    return iterator_strided(ptr_ + n * stride_, stride_);
  }

  iterator_strided &operator+=(const difference_type &n) {
    ptr_ += n * stride_;
    return *this;
  }

  iterator_strided operator-(const difference_type &n) const {
    return iterator_strided(ptr_ - n * stride_, stride_);
  }

  iterator_strided &operator-=(const difference_type &n) {
    ptr_ -= n * stride_;
    return *this;
  }

  reference operator*() const { return *ptr_; }

  pointer operator->() const { return ptr_; }

  reference operator[](const difference_type &n) const {
    return ptr_[n * stride_];
  }

  template <typename T2>
  friend bool operator==(const iterator_strided<T2> &r1,
                         const iterator_strided<T2> &r2);

  template <typename T2>
  friend bool operator!=(const iterator_strided<T2> &r1,
                         const iterator_strided<T2> &r2);

  template <typename T2>
  friend bool operator<(const iterator_strided<T2> &r1,
                        const iterator_strided<T2> &r2);

  template <typename T2>
  friend bool operator>(const iterator_strided<T2> &r1,
                        const iterator_strided<T2> &r2);

  template <typename T2>
  friend bool operator<=(const iterator_strided<T2> &r1,
                         const iterator_strided<T2> &r2);

  template <typename T2>
  friend bool operator>=(const iterator_strided<T2> &r1,
                         const iterator_strided<T2> &r2);

  template <typename T2>
  friend difference_type operator+(const iterator_strided<T2> &r1,
                                   const iterator_strided<T2> &r2);

  template <typename T2>
  friend difference_type operator-(const iterator_strided<T2> &r1,
                                   const iterator_strided<T2> &r2);

 private:
  difference_type stride_;

  T *ptr_;
};

template <typename T>
bool operator==(const iterator_strided<T> &r1, const iterator_strided<T> &r2) {
  return (r1.ptr_ == r2.ptr_);
}

template <typename T>
bool operator!=(const iterator_strided<T> &r1, const iterator_strided<T> &r2) {
  return (r1.ptr_ != r2.ptr_);
}

template <typename T>
bool operator<(const iterator_strided<T> &r1, const iterator_strided<T> &r2) {
  return (r1.ptr_ < r2.ptr_);
}

template <typename T>
bool operator>(const iterator_strided<T> &r1, const iterator_strided<T> &r2) {
  return (r1.ptr_ > r2.ptr_);
}

template <typename T>
bool operator<=(const iterator_strided<T> &r1, const iterator_strided<T> &r2) {
  return (r1.ptr_ <= r2.ptr_);
}

template <typename T>
bool operator>=(const iterator_strided<T> &r1, const iterator_strided<T> &r2) {
  return (r1.ptr_ >= r2.ptr_);
}

template <typename T>
typename iterator_strided<T>::difference_type operator+(
    const iterator_strided<T> &r1, const iterator_strided<T> &r2) {
  return iterator_strided<T>(r1.ptr_ + r2.ptr_);
}

template <typename T>
typename iterator_strided<T>::difference_type operator-(
    const iterator_strided<T> &r1, const iterator_strided<T> &r2) {
  return r1.ptr_ - r2.ptr_;
}

template <class T>
struct ConstArray2dView {
  typedef size_t size_type;
  typedef std::ptrdiff_t difference_type;
  typedef iterator_strided<const T> const_iterator0;
  typedef iterator_non_strided<const T> const_iterator1;
  typedef T ElementType;
  typedef T pixel_type;

  ConstArray2dView() {}
  ConstArray2dView(const T *data, size_type size0, size_type size1,
                   size_type stride)
      : data_(const_cast<T *>(data)),
        size0_(size0),
        size1_(size1),
        stride_(stride) {}

  ConstArray2dView(ConstArray2dView<T> const &other, difference_type start0,
                   difference_type start1, difference_type end0,
                   difference_type end1) {
    this->Initialize(other, start0, start1, end0, end1);
  }

  ConstArray2dView SubView(difference_type start0, difference_type start1,
                           difference_type end0, difference_type end1) const {
    ConstArray2dView<T> v;
    v.Initialize((*this), start0, start1, end0, end1);
    return v;
  }

  T *GetData() const { return data_; }

  size_type rows() const { return size0_; }

  size_type cols() const { return size1_; }

  size_type GetSize0() const { return size0_; }

  size_type GetSize1() const { return size1_; }

  size_type GetSize() const { return size0_ * size1_; }

  size_type GetStride() const { return stride_; }

  inline std::pair<size_t, size_t> GetSSERange(size_t row,
                                               size_t vec_size) const {
    int bitMask = vec_size - 1;
    size_t baseBytes = (size_t) & operator()(row, 0);
    assert(baseBytes % sizeof(T) == 0);

    size_t base = baseBytes / sizeof(T);
    size_t start_sse = ((base + bitMask) & ~bitMask) - base;
    size_t end_sse = ((base + size1_) & ~bitMask) - base;

    return std::make_pair(start_sse, end_sse);
  }

  inline std::pair<size_t, size_t> GetSSERange(size_t row, int vec_size,
                                               size_t start_col,
                                               size_t end_col) const {
    std::pair<size_t, size_t> p =
        SubView(0, start_col, this->_size0, end_col).GetSSERange(row, vec_size);

    return std::make_pair(p.first + start_col, p.first + start_col);
  }

  bool IsContiguous() const { return size1_ == stride_; }

  bool IsValid(size_t k) const {
    return k >= 0 && k / stride_ < size0_ && k % stride_ < size1_;
  }

  bool IsValid(size_t i0, size_t i1) const {
    return (i0 >= 0 && i1 >= 0 && i0 < size0_ && i1 < size1_);
  }

  const T &operator()(size_t i0, size_t i1) const {
    CHECK_BOUNDS_IF_ENABLED(IsValid(i0, i1));
    return data_[i1 + i0 * stride_];
  }

  const T &operator()(std::pair<int, int> position) const {
    CHECK_BOUNDS_IF_ENABLED(IsValid(position.first, position.second));
    return data_[position.second + position.first * stride_];
  }

  T const *operator[](const difference_type k) const {
    CHECK_BOUNDS_IF_ENABLED(IsValid(k));
    return &operator()(k, 0);
  }

  const_iterator1 Begin1(difference_type i0) const {
    CHECK_BOUNDS_IF_ENABLED(IsValid(i0, 0));
    return const_iterator1(&operator()(i0, 0));
  }

  const_iterator1 End1(difference_type i0) const {
    CHECK_BOUNDS_IF_ENABLED(IsValid(i0, 0));
    return const_iterator1(&operator()(i0, size1_));
  }

  const_iterator1 begin_row(difference_type i0) const {
    CHECK_BOUNDS_IF_ENABLED(IsValid(i0, 0));
    return const_iterator1(&operator()(i0, 0));
  }

  const_iterator1 end_row(difference_type i0) const {
    CHECK_BOUNDS_IF_ENABLED(IsValid(i0, 0));
    return const_iterator1(&operator()(i0, size1_));
  }

  // Iterate along y
  const_iterator0 Begin0(difference_type i1) const {
    CHECK_BOUNDS_IF_ENABLED(IsValid(0, i1));
    return const_iterator0(&operator()(0, i1), stride_);
  }

  const_iterator0 End0(difference_type i1) const {
    CHECK_BOUNDS_IF_ENABLED(IsValid(0, i1));
    return const_iterator0(&operator()(size0_, i1), stride_);
  }

  const_iterator0 begin_col(difference_type i1) const {
    CHECK_BOUNDS_IF_ENABLED(IsValid(0, i1));
    return const_iterator0(&operator()(0, i1), stride_);
  }

  const_iterator0 end_col(difference_type i1) const {
    CHECK_BOUNDS_IF_ENABLED(IsValid(0, i1));
    return const_iterator0(&operator()(size0_, i1), stride_);
  }

  T *ptr() { return data_; }

 protected:
  T *data_;
  size_type size0_;
  size_type size1_;
  size_type stride_;

  template <class Other>
  void Initialize(Other &other, difference_type start0, difference_type start1,
                  difference_type end0, difference_type end1) {
    size0_ = end0 - start0;
    size1_ = end1 - start1;

    CHECK_BOUNDS_IF_ENABLED(start0 >= 0 && end0 >= start0 &&
                            end0 <= other.GetSize0() && start1 >= 0 &&
                            end1 >= start1 && end1 <= other.GetSize1());

    stride_ = Stride(other);
    data_ = const_cast<T *>(other.GetData()) + start1 + start0 * stride_;
  }
};

template <class T>
class Array2dView : public ConstArray2dView<T> {
  typedef ConstArray2dView<T> Base;

 public:
  typedef typename Base::size_type size_type;
  typedef typename Base::difference_type difference_type;
  typedef typename Base::const_iterator0 const_iterator0;
  typedef typename Base::const_iterator1 const_iterator1;
  typedef iterator_strided<T> iterator0;
  typedef iterator_non_strided<T> iterator1;

  Array2dView() : Base(NULL, 0, 0, 0) {}

  Array2dView(T *data, size_t size0, size_t size1, size_t stride)
      : Base(data, size0, size1, stride) {}

  Array2dView(Array2dView<T> &other)
      : Base(other.data_, other.size0_, other.size1_, other.stride_) {}

  Array2dView(Array2dView<T> const &other)
      : Base(other.data_, other.size0_, other.size1_, other.stride_) {}

  Array2dView(Array2dView<T> const &other, difference_type start0,
              difference_type start1, difference_type end0,
              difference_type end1) {
    this->Initialize(other, start0, start1, end0, end1);
  }

  Array2dView<T> SubView(difference_type start0, difference_type start1,
                         difference_type end0, difference_type end1) const {
    difference_type stride = (*this).GetStride();
    return Array2dView<T>(((*this).GetData() + start0 * stride + start1),
                          difference_type(end0 - start0),
                          difference_type(end1 - start1), stride);
  }

  Array2dView &operator=(Array2dView<T> &other) {
    this->data_ = other.data_;
    this->size0_ = other.size0_;
    this->size1_ = other.size1_;
    this->stride_ = other.stride_;
    return *this;
  }

  Array2dView &operator=(Array2dView<T> const &other) {
    this->data_ = other.data_;
    this->size0_ = other.size0_;
    this->size1_ = other.size1_;
    this->stride_ = other.stride_;
    return *this;
  }

  T &operator()(difference_type i0, difference_type i1) {
    CHECK_BOUNDS_IF_ENABLED(this->IsValid(i0, i1));
    return this->data_[i1 + i0 * this->stride_];
  }

  T *operator[](const difference_type k) {
    CHECK_BOUNDS_IF_ENABLED(IsValid(k));
    return &operator()(k, 0);
  }

  T &At(difference_type k) {
    if (!this->IsValid(k))
      throw std::runtime_error("Array2dView out of bounds");
    return *this(k);
  }

  T &At(difference_type i0, difference_type i1) {
    if (!this->IsValid(i0, i1))
      throw std::runtime_error("Array2dView out of bounds");
    return *this(i0, i1);
  }

  // Iterate along x

  iterator1 Begin1(difference_type i0) {
    CHECK_BOUNDS_IF_ENABLED(this->IsValid(i0, 0));
    return iterator1(&operator()(i0, 0));
  }

  iterator1 End1(difference_type i0) {
    CHECK_BOUNDS_IF_ENABLED(this->IsValid(i0, 0));
    return iterator1(&operator()(i0, this->size1_));
  }

  iterator0 Begin0(difference_type i1) {
    CHECK_BOUNDS_IF_ENABLED(this->IsValid(0, i1));
    return iterator0(&operator()(0, i1), this->stride_);
  }

  iterator0 End0(difference_type i1) {
    CHECK_BOUNDS_IF_ENABLED(this->IsValid(0, i1));
    return iterator0(&operator()(this->size0_, i1), this->stride_);
  }

  const T &operator()(difference_type i0, difference_type i1) const {
    return Base::operator()(i0, i1);
  }

  const T &operator[](const difference_type k) const {
    return Base::operator[](k);
  }

  const_iterator1 Begin1(difference_type i0) const { return Base::Begin1(i0); }

  const_iterator1 End1(difference_type i0) const { return Base::End1(i0); }

  // Iterate along y
  const_iterator0 Begin0(difference_type i1) const { return Base::Begin0(i1); }

  const_iterator0 End0(difference_type i1) const { return Base::End0(i1); }

  template <typename U>
  Array2dView &operator+=(ConstArray2dView<U> other) {
    CHECK_IF_ARRAYS_HAVE_SAME_SIZE;
    for (size_t i0 = 0; i0 < this->size0_; i0++)
      for (size_t i1 = 0; i1 < this->size1_; i1++)
        (*this)(i0, i1) += other(i0, i1);
    return *this;
  }

  template <typename U>
  Array2dView &operator-=(ConstArray2dView<U> other) {
    CHECK_IF_ARRAYS_HAVE_SAME_SIZE;
    for (size_t i0 = 0; i0 < this->size0_; i0++)
      for (size_t i1 = 0; i1 < this->size1_; i1++)
        (*this)(i0, i1) -= other(i0, i1);
    return *this;
  }

  template <typename U>
  Array2dView &operator*=(ConstArray2dView<U> other) {
    CHECK_IF_ARRAYS_HAVE_SAME_SIZE;
    for (size_t i0 = 0; i0 < this->size0_; i0++)
      for (size_t i1 = 0; i1 < this->size1_; i1++)
        (*this)(i0, i1) *= other(i0, i1);
    return *this;
  }

  template <typename U>
  Array2dView &operator/=(ConstArray2dView<U> other) {
    CHECK_IF_ARRAYS_HAVE_SAME_SIZE;
    for (size_t i0 = 0; i0 < this->size0_; i0++)
      for (size_t i1 = 0; i1 < this->size1_; i1++)
        (*this)(i0, i1) /= other(i0, i1);
    return *this;
  }

  template <typename U>
  Array2dView &fill(const U other) {
    CHECK_IF_ARRAYS_HAVE_SAME_SIZE;
    for (size_t i0 = 0; i0 < this->size0_; i0++)
      for (size_t i1 = 0; i1 < this->size1_; i1++) (*this)(i0, i1) = other;
    return *this;
  }

  template <typename U>
  Array2dView &add(ConstArray2dView<U> &other) {
    *this += other;
    return *this;
  }

  template <typename U>
  Array2dView &subtract(ConstArray2dView<U> &other) {
    *this -= other;
    return *this;
  }

  template <typename U>
  Array2dView &multiply(ConstArray2dView<U> &other) {
    *this *= other;
    return *this;
  }

  template <typename U>
  Array2dView<T> &divide(ConstArray2dView<U> &other) {
    *this /= other;
    return *this;
  }

  Array2dView<T> &negate() {
    for (size_t i0 = 0; i0 < this->size0_; i0++)
      for (size_t i1 = 0; i1 < this->size0_; i1++)
        this->operator()(i0, i1) = -this->operator()(i0, i1);
    return *this;
  }

  template <typename U>
  Array2dView<T> &equals(ConstArray2dView<U> other) {
    CHECK_IF_ARRAYS_HAVE_SAME_SIZE;
    for (size_t i0 = 0; i0 < this->size0_; i0++)
      for (size_t i1 = 0; i1 < this->size0_; i1++)
        this->operator()(i0, i1) = this->operator()(i0, i1) == other(i0, i1);
    return *this;
  }

  template <typename U>
  Array2dView<T> &equals(U const value) {
    for (size_t i0 = 0; i0 < this->size0_; i0++)
      for (size_t i1 = 0; i1 < this->size1_; i1++)
        *this(i0, i1) = *this(i0, i1) == value;
    return *this;
  }

  template <typename U>
  Array2dView<T> &eps_equals(ConstArray2dView<U> other, T eps) {
    CHECK_IF_ARRAYS_HAVE_SAME_SIZE;
    for (size_t i0 = 0; i0 < this->size0_; i0++)
      for (size_t i1 = 0; i1 < this->size1_; i1++)
        this->operator()(i0, i1) =
            abs((this->operator()(i0, i1)) - other(i0, i1)) < eps;
    return *this;
  }

  template <typename U>
  Array2dView<T> &eps_equals(U const value, T eps) {
    for (size_t i0 = 0; i0 < this->size0_; i0++)
      for (size_t i1 = 0; i1 < this->size1_; i1++)
        this->operator()(i0, i1) =
            abs((this->operator()(i0, i1)) - value) < eps;
    return *this;
  }

  template <typename U>
  struct unary_identity {
    inline U operator()(U src) { return src; }
  };

  template <typename U>
  Array2dView<T> &copy(ConstArray2dView<U> &other) {
    CHECK_IF_ARRAYS_HAVE_SAME_SIZE;
    for (size_t i0 = 0; i0 < this->size0_; i0++) {
      const U *src = &other(i0, 0);
      std::copy(src, src + this->size1_, &this->operator()(i0, 0));
    }

    return *this;
  }
};

template <typename T>
ConstArray2dView<T> get_rows(ConstArray2dView<T> const &src, size_t first,
                             size_t last) {
  return ConstArray2dView<T>(src, first, 0, last, src.GetSize1());
}

template <typename T>
Array2dView<T> get_rows(Array2dView<T> const &src, size_t first, size_t last) {
  return Array2dView<T>(src, first, 0, last, src.GetSize1());
}

template <typename T>
ConstArray2dView<T> get_nth_row(ConstArray2dView<T> const &src, size_t row) {
  return get_rows(src, row, row + 1);
}

template <typename T>
Array2dView<T> get_nth_row(Array2dView<T> const &src, size_t row) {
  return get_rows(src, row, row + 1);
}

template <typename T>
ConstArray2dView<T> get_last_n_rows(ConstArray2dView<T> const &src, size_t n) {
  return get_rows(src, src.GetSize0() - n, src.GetSize0());
}

template <typename T>
Array2dView<T> get_last_n_rows(Array2dView<T> const &src, size_t n) {
  return get_rows(src, src.GetSize0() - n, src.GetSize0());
}

template <typename T>
ConstArray2dView<T> get_first_n_rows(ConstArray2dView<T> const &src, size_t n) {
  return get_rows(src, 0, n);
}

template <typename T>
Array2dView<T> get_first_n_rows(Array2dView<T> const &src, size_t n) {
  return get_rows(src, 0, n);
}

template <typename T>
ConstArray2dView<T> get_cols(ConstArray2dView<T> const &src, size_t first,
                             size_t last) {
  return ConstArray2dView<T>(src, 0, first, src.GetSize0(), last);
}

template <typename T>
Array2dView<T> get_cols(Array2dView<T> const &src, size_t first, size_t last) {
  return Array2dView<T>(src, 0, first, src.GetSize0(), last);
}

template <typename T>
ConstArray2dView<T> get_nth_col(ConstArray2dView<T> const &src, size_t col) {
  return get_cols(src, col, col + 1);
}

template <typename T>
Array2dView<T> get_nth_col(Array2dView<T> const &src, size_t col) {
  return get_cols(src, col, col + 1);
}

template <typename T>
ConstArray2dView<T> get_last_n_cols(ConstArray2dView<T> const &src, size_t n) {
  return get_cols(src, src.GetSize1() - n, src.GetSize1());
}

template <typename T>
Array2dView<T> get_last_n_cols(Array2dView<T> const &src, size_t n) {
  return get_cols(src, src.GetSize1() - n, src.GetSize1());
}

template <typename T>
ConstArray2dView<T> get_first_n_cols(ConstArray2dView<T> const &src, size_t n) {
  return get_cols(src, 0, n);
}

template <typename T>
Array2dView<T> get_first_n_cols(Array2dView<T> const &src, size_t n) {
  return get_cols(src, 0, n);
}

template <typename T>
Array2dView<T> make_view(T *data, ptrdiff_t rows, ptrdiff_t cols) {
  return Array2dView<T>(data, rows, cols, cols);
}

template <typename T>
ConstArray2dView<T> make_const_view(const T *data, ptrdiff_t rows,
                                    ptrdiff_t cols) {
  return ConstArray2dView<T>(data, rows, cols, cols);
}

#undef CHECK_IF_ARRAYS_HAVE_SAME_SIZE
}  // sil
using sil::Array2dView;
using sil::ConstArray2dView;
#endif
