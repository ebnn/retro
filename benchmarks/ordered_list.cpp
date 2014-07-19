#include <hayai.hpp>

#include "retro/detail/ordered_list.hpp"

#include <list>

BENCHMARK(OrderedList, PushEmptyStrings, 1, 10)
{
  retro::detail::ordered_list<std::string> q;

  for (int i = 0; i < 100000; i++)
    q.push_back("");
}

BENCHMARK(StlList, PushEmptyStrings, 1, 10)
{
  std::list<std::string> q;

  for (int i = 0; i < 100000; i++)
    q.push_back("");
}
