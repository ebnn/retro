#pragma once

#include "retro/queue.hpp"
#include "retro/map.hpp"
#include "retro/detail/ordered_list.hpp"

void insert_dummy(retro::detail::ordered_list<int> &v)
{
  v.push_back(0);
}

void insert_dummy(retro::partial_queue<int> &v)
{
  v.push(0);
}

void insert_dummy(retro::full_map<int, int> &v)
{
  v.insert(std::make_pair(0, 0));
}

// Insert dummy data into a container
template <class Container>
void insert_data(Container &v, int n)
{
  for (int i = 0; i < n; i++)
    insert_dummy(v);
}

