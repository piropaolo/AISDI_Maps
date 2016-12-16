#include <cstddef>
#include <cstdlib>
#include <string>

#include <iostream>
#include <chrono>
#include <ctime>

#include <algorithm>
#include <vector>
#include <random>

#include "TreeMap.h"
#include "HashMap.h"

namespace
{

template <typename K, typename V>
using TreeMap = aisdi::TreeMap<K, V>;
template <typename K, typename V>
using HashMap = aisdi::HashMap<K, V>;

void performTest(std::size_t n)
{
  TreeMap<int, std::string> map;
  HashMap<int, std::string> map2;
  std::chrono::time_point<std::chrono::system_clock> start, end;
  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();

  std::vector<int> keys;
  for (size_t i = 0; i < n; ++i) keys.push_back(i);
  std::shuffle (keys.begin(), keys.end(), std::default_random_engine(seed));

  start = std::chrono::system_clock::now();
  for (auto it = keys.begin(); it !=  keys.end(); ++it)
    map[*it] = "DONE";
  end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = end-start;
  std::cout << "TreeMap   Add time:       " << elapsed_seconds.count() << "s\n";

  start = std::chrono::system_clock::now();
  for (auto it = keys.begin(); it !=  keys.end(); ++it)
    map2[*it] = "DONE";
  end = std::chrono::system_clock::now();
  elapsed_seconds = end-start;
  std::cout << "HashMap   Add time:       " << elapsed_seconds.count() << "s\n";

  seed = std::chrono::system_clock::now().time_since_epoch().count();
  std::shuffle (keys.begin(), keys.end(), std::default_random_engine(seed));

  start = std::chrono::system_clock::now();
  for (auto it = keys.begin(); it !=  keys.end(); ++it)
    map[*it] = "CHANGED";
  end = std::chrono::system_clock::now();
  elapsed_seconds = end-start;
  std::cout << "TreeMap   Change time:    " << elapsed_seconds.count() << "s\n";

  start = std::chrono::system_clock::now();
  for (auto it = keys.begin(); it !=  keys.end(); ++it)
    map2[*it] = "CHANGED";
  end = std::chrono::system_clock::now();
  elapsed_seconds = end-start;
  std::cout << "HashMap   Change time:    " << elapsed_seconds.count() << "s\n";

  start = std::chrono::system_clock::now();
  for (std::size_t i = 0; i < n; ++i)
    map.remove(begin(map));
  end = std::chrono::system_clock::now();
  elapsed_seconds = end-start;
  std::cout << "TreeMap   Remove time:    " << elapsed_seconds.count() << "s\n";

  start = std::chrono::system_clock::now();
  for (std::size_t i = 0; i < n; ++i)
    map2.remove(begin(map2));
  end = std::chrono::system_clock::now();
  elapsed_seconds = end-start;
  std::cout << "HashMap   Remove time:    " << elapsed_seconds.count() << "s\n";
}

} // namespace

int main(int argc, char** argv)
{
  srand(time(NULL));
  const std::size_t repeatCount = argc > 1 ? std::atoll(argv[1]) : 10000;
  performTest(repeatCount);
  //performTest2(repeatCount);
  return 0;
}
