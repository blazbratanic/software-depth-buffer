/* mode()
 *
 * Determines mode (most common element) of an array of integers.
 *
 * Uses the std::map container to calculate the mode of an array of integers.
 * Creates a map with key-value pairs given by the numbers in the array and
 * their frequencies.  Utilizes the fact that only unique keys are stored to
 * create the tally, and returns the key with the highest frequency.  Not
 * suitable for bimodal/multimodal data, as the returned mode will be biased
 * towards the lower of the two (or more) equally frequent values.
 *
 * Dependencies: requires std::map header.
 */

#ifndef MODE_H_
#define MODE_H_

#include <map>
#include <limits>
#include <algorithm>

inline int mode(int array[], int nElements) {

  //  declare map to store tallies
  std::map<int, int> tally;

  //  run through each element of the array.  if the element has not yet been
  //  encountered, create a key with that element and set value to 1.  If it
  //  has been encountered, incremenet the value for that key
  for (int i = 0; i < nElements; ++i) {
    tally[array[i]] =
        (tally.find(array[i]) == tally.end()) ? 1 : tally[array[i]] + 1;
  }

  //  run through map and determine most common element, by comparing the
  //  value for each key.  If the value of the current key is greater than the
  //  stored value, store the current key and its value, and continue
  int maxfrequency = std::numeric_limits<int>::min();
  std::map<int, int>::iterator maxfreqindex = tally.begin();

  for (std::map<int, int>::iterator itr = tally.begin(); itr != tally.end();
       ++itr) {
    if (itr->second > maxfrequency) {
      maxfrequency = itr->second;
      maxfreqindex = itr;
    }
  }

  //  return the stored key (the element of the array with the highest tally)
  return maxfreqindex->first;
}

inline int fast_mode(int values[]) {
  int frequency[8] = {0};
  for (int i = 0; i < 9; i++) ++frequency[values[i]];
  return std::distance(frequency, std::max_element(frequency, &frequency[8]));
}

inline std::pair<int, int> mode_frequency(int values[]) {
  int frequency[8] = {0};
  for (int i = 0; i < 9; i++) ++frequency[values[i]];
  auto max_el = std::max_element(frequency, &frequency[8]);
  return std::make_pair(std::distance(frequency, max_el), *max_el);
}
#endif
