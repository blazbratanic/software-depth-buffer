#ifndef FUNCTORS_H
#define FUNCTORS_H

namespace sil {
namespace functors {
template <typename T, typename U>
struct conditional_copy {
  T operator()(T const &a, U const &b) { return a * b; }
};

template <typename T>
struct conditional_binary_copy {
  float operator()(T const &a) {
    return a ? static_cast<float>(1) : static_cast<float>(0);
  }
};

template <typename T>
struct lower_bound {
  T operator()(T lower_bound, T const &a) {
    return a < lower_bound ? lower_bound : a;
  }
};

template <typename T>
// Workaround due to visual studio 2010 bug with bind2nd and bind1st
struct lower_bound_ {
  lower_bound_(T bound) : bound_(bound) {}
  T operator()(T const &a) { return a < bound_ ? bound_ : a; }

 private:
  T bound_;
};

template <typename T>
struct upper_bound {
  T operator()(T const &a, const T upper_bound) {
    return a > upper_bound ? upper_bound : a;
  }
};

template <typename T>
struct threshold {
  T operator()(T const &a, const T thresh, const T label = 255) {
    return a > thresh ? label : 0;
  }
};

template <typename T, typename U>
struct equals {
  T operator()(T const &a, U const &b) { return a == b; }
};

template <typename T, typename U>
struct equals_value {
  equals_value(T value) : value_(value) {}
  T operator()(T const &a) {
    return a == value_ ? static_cast<U>(1) : static_cast<U>(0);
  }
  T value_;
};

template <typename T>
struct unary_identity {
  T operator()(T a) { return a; }
};

template <typename T>
struct unique_increasing {
  T start_;
  unique_increasing(T start) : start_(start) {}
  T operator()() { return start_++; }
};

template <typename T>
struct constant {
  constant(T value) : value_(value) {}
  T operator()(T) { return value_; }
  T value_;
};

template <typename T, typename U>
struct conditional_constant {
  conditional_constant(T value) : value_(value) {}
  T operator()(T a, U cond) { return cond ? value_ : a; }
  T value_;
};

template <typename T>
struct fatan2 {
  T operator()(T a, T b) {
    return fast_atan2f(static_cast<float>(a), static_cast<float>(b));
  }
};

template <typename T>
struct fatan {
  T operator()(T a, T b) {
    return fast_atanf(static_cast<float>(a), static_cast<float>(b));
  }
};
}
}

#endif
