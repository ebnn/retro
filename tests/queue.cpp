#include <gtest/gtest.h>

#include "retro/queue.hpp"

TEST(partial_queue, pushingElementsDoesNotChangeFrontButChangeBack)
{
  retro::partial_queue<int> q;

  q.push(1);
  ASSERT_EQ(1U, q.size());
  EXPECT_EQ(1, q.front());
  EXPECT_EQ(1, q.back());

  q.push(2);
  ASSERT_EQ(2U, q.size());
  EXPECT_EQ(1, q.front());
  EXPECT_EQ(2, q.back());

  q.push(3);
  ASSERT_EQ(3U, q.size());
  EXPECT_EQ(1, q.front());
  EXPECT_EQ(3, q.back());
}

TEST(partial_queue, poppingElementsGivesCorrectFrontAndBack)
{
  retro::partial_queue<int> q;

  q.push(1);
  q.push(2);
  q.push(3);
  ASSERT_EQ(3U, q.size());
  EXPECT_EQ(1, q.front());
  EXPECT_EQ(3, q.back());

  q.pop();
  ASSERT_EQ(2U, q.size());
  EXPECT_EQ(2, q.front());
  EXPECT_EQ(3, q.back());

  q.pop();
  ASSERT_EQ(1U, q.size());
  EXPECT_EQ(3, q.front());
  EXPECT_EQ(3, q.back());

  q.pop();
  ASSERT_EQ(0U, q.size());
}

TEST(partial_queue, pushInThePastGivesCorrectFrontAndBack)
{
  retro::partial_queue<int> q;

  // queue: [3]
  auto t3 = q.push(3);
  ASSERT_EQ(1U, q.size());
  EXPECT_EQ(3, q.front());
  EXPECT_EQ(3, q.back());

  // queue: [2, 3]
  auto t2 = q.push(t3, 2);
  ASSERT_EQ(2U, q.size());
  EXPECT_EQ(2, q.front());
  EXPECT_EQ(3, q.back());

  // queue: [1, 2, 3]
  q.push(t2, 1);
  ASSERT_EQ(3U, q.size());
  EXPECT_EQ(1, q.front());
  EXPECT_EQ(3, q.back());

  // queue: [1, 4, 2, 3]
  q.push(t2, 4);
  ASSERT_EQ(4U, q.size());
  EXPECT_EQ(1, q.front());
  EXPECT_EQ(3, q.back());
}

TEST(partial_queue, revertPushGivesCorrectFrontAndBack)
{
  retro::partial_queue<int> q;

  // queue: [1, 2, 3, 4]
  auto t1 = q.push(1);
  auto t2 = q.push(2);
  auto t3 = q.push(3);
  auto t4 = q.push(4);
  ASSERT_EQ(4U, q.size());
  EXPECT_EQ(1, q.front());
  EXPECT_EQ(4, q.back());

  // queue: [1, 3, 4]
  q.revert(t2);
  ASSERT_EQ(3U, q.size());
  EXPECT_EQ(1, q.front());
  EXPECT_EQ(4, q.back());

  // queue: [3, 4]
  q.revert(t1);
  ASSERT_EQ(2U, q.size());
  EXPECT_EQ(3, q.front());
  EXPECT_EQ(4, q.back());

  // queue: [3]
  q.revert(t4);
  ASSERT_EQ(1U, q.size());
  EXPECT_EQ(3, q.front());
  EXPECT_EQ(3, q.back());

  // queue: []
  q.revert(t3);
  ASSERT_EQ(0U, q.size());
}

TEST(partial_queue, moveIsEquivalentToPush)
{
  retro::partial_queue<std::pair<int, int>> q;

  std::pair<int, int> p1 = { 1, 2 };
  std::pair<int, int> p2 = { 3, 4 };

  q.push(std::move(p1));
  q.push(std::move(p2));

  ASSERT_EQ(2U, q.size());
  EXPECT_EQ(1, q.front().first);
  EXPECT_EQ(2, q.front().second);
  EXPECT_EQ(3, q.back().first);
  EXPECT_EQ(4, q.back().second);
}
