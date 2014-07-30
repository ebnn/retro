#include <gtest/gtest.h>

#include "retro/map.hpp"

TEST(full_map, canFindInsertedElements)
{
  retro::full_map<int, int> m;
  
  m.insert(std::make_pair(1, 1));
  m.insert(std::make_pair(2, 2));
  m.insert(std::make_pair(3, 3));

  EXPECT_EQ(1, m.find(1)->first);
  EXPECT_EQ(1, m.find(1)->second);
  EXPECT_EQ(2, m.find(2)->first);
  EXPECT_EQ(2, m.find(2)->second);
  EXPECT_EQ(3, m.find(3)->first);
  EXPECT_EQ(3, m.find(3)->second);
  EXPECT_EQ(m.end(), m.find(0));
  EXPECT_EQ(m.end(), m.find(4));
}

TEST(full_map, canRetroactivelyIterateThroughPresentInsertions)
{
  retro::full_map<int, int> m;
  auto t1 = m.insert(std::make_pair(1, 1));
  auto t2 = m.insert(std::make_pair(2, 2));
  auto t3 = m.insert(std::make_pair(3, 3));

  // Before t1, there was nothing.
  EXPECT_EQ(m.begin(t1), m.end(t1));

  // Before t2, there was only '1'
  EXPECT_NE(m.begin(t2), m.end(t2));
  EXPECT_EQ(1, m.begin(t2)->first);
  EXPECT_EQ(m.end(t2), std::next(m.begin(t2)));
 
  // Before t3, there was only '1', '2'
  EXPECT_NE(m.begin(t3), m.end(t3));
  EXPECT_EQ(1, m.begin(t3)->first);
  EXPECT_EQ(2, std::next(m.begin(t3))->first);
  EXPECT_EQ(m.end(t3), std::next(std::next(m.begin(t3))));
}

TEST(full_map, canRetroactivelyIteratePresentInsertions)
{
  retro::full_map<int, int> m;
  
  m.insert(std::make_pair(4, 3));
  m.insert(std::make_pair(1, 6));
  m.insert(std::make_pair(3, 1));

  auto it = m.begin();
  EXPECT_EQ(1, it->first);
  EXPECT_EQ(6, it->second);

  ++it;
  EXPECT_EQ(3, it->first);
  EXPECT_EQ(1, it->second);

  ++it;
  EXPECT_EQ(4, it->first);
  EXPECT_EQ(3, it->second);

  ++it;
  EXPECT_EQ(m.end(), it);
}

TEST(full_map, retroactiveInsertionAffectsPresent)
{
  retro::full_map<int, int> m;
  
  auto t = m.insert(std::make_pair(1, 1));
  for (int i = 2; i <= 10; i++)
  {
    t = m.insert(t, std::make_pair(i, i)); 
  }

  int i = 1;
  for (auto it = m.begin(); it != m.end(); ++it, ++i)
  {
    EXPECT_EQ(i, it->first);
  }
}

TEST(full_map, presentInsertionsCanBeRetroactivelyFound)
{
  retro::full_map<int, int> m;
  auto t1 = m.insert(std::make_pair(1, 1));
  auto t2 = m.insert(std::make_pair(2, 2));
  auto t3 = m.insert(std::make_pair(3, 3));

  // Before t1, the map is empty
  EXPECT_EQ(m.end(t1), m.find(t1, 1));
  EXPECT_EQ(m.end(t1), m.find(t1, 2));
  EXPECT_EQ(m.end(t1), m.find(t1, 3));

  // Before t2, the map contains '1'
  EXPECT_EQ(1, m.find(t2, 1)->second);
  EXPECT_EQ(m.end(t2), m.find(t2, 2));
  EXPECT_EQ(m.end(t2), m.find(t2, 3));

  // Before t3, the map contains '1', '2'
  EXPECT_EQ(1, m.find(t3, 1)->second);
  EXPECT_EQ(2, m.find(t3, 2)->second);
  EXPECT_EQ(m.end(t3), m.find(t3, 3));
}

TEST(full_map, retroactiveInsertionsCanBeRetroactivelyFound)
{
  retro::full_map<int, int> m;
  auto t3 = m.insert(std::make_pair(3, 3));
  auto t1 = m.insert(t3, std::make_pair(1, 1));

  // Before t1, the map is empty
  EXPECT_EQ(m.end(t1), m.find(t1, 1));
  EXPECT_EQ(m.end(t1), m.find(t1, 2));
  EXPECT_EQ(m.end(t1), m.find(t1, 3));

  // Before t3, the map contains '1'
  EXPECT_EQ(1, m.find(t3, 1)->second);
  EXPECT_EQ(m.end(t3), m.find(t3, 2));
  EXPECT_EQ(m.end(t3), m.find(t3, 3));

  // Retroactively insert '2'
  auto t2 = m.insert(t3, std::make_pair(2, 2));
 
  // Before t2, the map contains '1'
  EXPECT_EQ(1, m.find(t2, 1)->second);
  EXPECT_EQ(m.end(t2), m.find(t2, 2));
  EXPECT_EQ(m.end(t2), m.find(t2, 3));

  // Before t3, the map contains '1', '2'
  EXPECT_EQ(1, m.find(t3, 1)->second);
  EXPECT_EQ(2, m.find(t3, 2)->second);
  EXPECT_EQ(m.end(t3), m.find(t3, 3));
}
