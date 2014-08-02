#include <hayai.hpp>

#include "retro/map.hpp"

#include <map>

BENCHMARK(FullMap, InsertAndFindNumericKeys, 1, 10)
{
  retro::full_map<int, int> q;

  for (int i = 0; i < 100000; i++)
    q.insert(std::make_pair(i, i));

  for (int i = 0; i < 100000; i++)
    q.find(i);
}

BENCHMARK(StlMap, InsertAndFindNumericKeys, 1, 10)
{
  std::map<int, int> q;

  for (int i = 0; i < 100000; i++)
    q.insert(std::make_pair(i, i));

  for (int i = 0; i < 100000; i++)
    q.find(i);
}
