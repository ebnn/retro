#include <gtest/gtest.h>

#include "retro/detail/ordered_list.hpp"

template <class T>
bool is_correct_order(T &ol)
{
  bool correct = true;

  for (auto from = ol.begin(); from != ol.end(); ++from)
  {
    auto to = ol.begin();
    for (; to != from; ++to)
      correct &= (to < from);

    correct &= (to == from);
    ++to;

    for (; to != ol.end(); ++to)
      correct &= (to > from);
  }

  return correct;
}

TEST(ordered_list, pushBackDoesNotChangeFrontButChangeBack)
{
  retro::detail::ordered_list<int> ol;

  ol.push_back(1);
  ASSERT_EQ(1U, ol.size());
  EXPECT_EQ(1, ol.front());
  EXPECT_EQ(1, ol.back());

  ol.push_back(2);
  ASSERT_EQ(2U, ol.size());
  EXPECT_EQ(1, ol.front());
  EXPECT_EQ(2, ol.back());

  ol.push_back(3);
  ASSERT_EQ(3U, ol.size());
  EXPECT_EQ(1, ol.front());
  EXPECT_EQ(3, ol.back());
}

TEST(ordered_list, pushFrontDoesNotChangeBackButChangeFront)
{
  retro::detail::ordered_list<int> ol;

  ol.push_front(1);
  ASSERT_EQ(1U, ol.size());
  EXPECT_EQ(1, ol.front());
  EXPECT_EQ(1, ol.back());

  ol.push_front(2);
  ASSERT_EQ(2U, ol.size());
  EXPECT_EQ(2, ol.front());
  EXPECT_EQ(1, ol.back());

  ol.push_front(3);
  ASSERT_EQ(3U, ol.size());
  EXPECT_EQ(3, ol.front());
  EXPECT_EQ(1, ol.back());
}

TEST(ordered_list, pushBackMaintainsOrder)
{
  retro::detail::ordered_list<int> ol;

  for (int i = 0; i < 100; i++)
  {
    ol.push_back(i);
  }

  ASSERT_EQ(100U, ol.size());
  EXPECT_TRUE(is_correct_order(ol));
}

TEST(ordered_list, pushFrontMaintainsOrder)
{
  retro::detail::ordered_list<int> ol;

  for (int i = 0; i < 100; i++)
  {
    ol.push_front(i);
  }

  ASSERT_EQ(100U, ol.size());
  EXPECT_TRUE(is_correct_order(ol));
}

TEST(ordered_list, insertMiddleMaintainsOrder)
{
  retro::detail::ordered_list<int> ol;

  ol.push_back(0);
  ol.push_back(0);

  auto middle = std::next(ol.begin());
  for (int i = 1; i <= 100; i++)
  {
    ol.insert(middle, i);
    if (i % 2 == 0) --middle;
  }

  ASSERT_EQ(102U, ol.size());
  EXPECT_TRUE(is_correct_order(ol));
}

TEST(ordered_list, canReachMaximumSize)
{
  retro::detail::ordered_list<int, unsigned char> ol;

  while (ol.size() != ol.max_size())
    ol.push_back(0);

  EXPECT_TRUE(is_correct_order(ol));
}
