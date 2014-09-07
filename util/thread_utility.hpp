#ifndef THREAD_UTILITY_HPP
#define THREAD_UTILITY_HPP

#include <vector>
#include <utility>
#include <algorithm>

std::vector<std::pair<int, int>> distribute_uniform_workload(
    std::pair<int, int> interval, size_t nworkers) {
  assert(interval.second > interval.first);
  if (interval.second - interval.first < 10)
    return std::vector<std::pair<int, int>>{interval};

  const size_t batch_size = (interval.second - interval.first) / nworkers;
  std::vector<std::pair<int, int>> batches;

  int idx = interval.first;
  for (size_t i = 0; i < nworkers - 1; i++) {
    batches.push_back(std::make_pair(idx, idx + batch_size));
    idx += batch_size;
  }
  batches.push_back(std::make_pair(idx, interval.second));
  return batches;
}

#endif
