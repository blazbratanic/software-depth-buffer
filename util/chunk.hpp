#ifndef CHUNK_HPP_
#define CHUNK_HPP_
#include <cppitertools/iterator_range.hpp>
#include <cppitertools/slice.hpp>

template <typename Container>
std::vector<iter::iterator_range<typename Container::const_iterator>> chunk_iter(
    Container const& container, size_t chunk_size) {
  assert(container.size() >= chunk_size);

  std::vector<iter::iterator_range<typename Container::const_iterator>> chunkit;

  auto it_begin = container.cbegin();
  auto it_end = container.cend();

  while (true) {
    if (std::distance(it_begin, it_end) > chunk_size) {
      chunkit.emplace_back(typename decltype(chunkit)::value_type(it_begin, it_end));
      break;
    }
    chunkit.emplace_back(typename decltype(chunkit)::value_type(
        it_begin, std::next(it_begin, chunk_size)));
    std::advance(it_begin, chunk_size);
  }
  return chunkit;
}

#endif
