#include <hayai.hpp>

#include "retro/queue.hpp"

#include <queue>
#include <list>

BENCHMARK(PartialQueue, PushEmptyStrings, 1, 10)
{
  retro::partial_queue<std::string> q;

  for (int i = 0; i < 100000; i++)
    q.push("");
}

BENCHMARK(StlQueue, PushEmptyStrings, 1, 10)
{
  std::queue<std::string> q;

  for (int i = 0; i < 100000; i++)
    q.push("");
}

BENCHMARK(StlList, PushEmptyStrings, 1, 10)
{
  std::list<std::string> q;

  for (int i = 0; i < 100000; i++)
    q.push_back("");
}

BENCHMARK(PartialQueue, PushPopEmptyStrings, 1, 10)
{
  retro::partial_queue<std::string> q;

  for (int i = 0; i < 100000; i++)
    q.push("");

  while (!q.empty())
  {
    q.pop();
  }
}

BENCHMARK(StlQueue, PushPopEmptyStrings, 1, 10)
{
  std::queue<std::string> q;

  for (int i = 0; i < 100000; i++)
    q.push("");

  while (!q.empty())
  {
    q.pop();
  }
}

BENCHMARK(StlList, PushPopEmptyStrings, 1, 10)
{
  std::list<std::string> q;

  for (int i = 0; i < 100000; i++)
    q.push_back("");

  while (!q.empty())
  {
    q.pop_back();
  }
}
