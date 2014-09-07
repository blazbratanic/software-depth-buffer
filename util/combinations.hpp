#ifndef COMBINATIONS_HPP
#define COMBINATIONS_HPP
#include <vector>
#include <algorithm>
#include <cassert>

template <typename T>
static inline std::vector<T> randomly_pick_n(std::vector<T> const& v,
                                             size_t n) {
  assert(n <= v.size());
  std::vector<size_t> idx(v.size());
  std::iota(idx.begin(), idx.end(), static_cast<size_t>(0));
  std::random_shuffle(idx.begin(), idx.end());

  std::vector<T> output;
  output.reserve(n);
  auto idx_it = idx.begin();
  for (size_t i = 0; i < n; i++) {
    output.emplace_back(v[*idx_it++]);
  }
  return output;
}

static inline std::vector<int> random_combination(size_t n, size_t k) {
  assert(n >= k);
  std::vector<int> i(n);

  std::iota(i.begin(), i.end(), 0);
  std::random_shuffle(i.begin(), i.end());
  i.resize(k);
  std::vector<int> output(i);
  return output;
}

#endif
