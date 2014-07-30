#include <gtest/gtest.h>

#include "helpers.hpp"

template <typename T>
class StlIteratorTest : public ::testing::Test
{
};

// Retro types with STL iterators
typedef ::testing::Types<
  retro::detail::ordered_list<int>,
  retro::full_map<int, int>
> RetroTypes;

TYPED_TEST_CASE(StlIteratorTest, RetroTypes);

TYPED_TEST(StlIteratorTest, compareBeginEndWorksinitially)
{
  TypeParam v;
  
  EXPECT_TRUE(v.begin() == v.end());
  EXPECT_FALSE(v.begin() < v.end());
  EXPECT_FALSE(v.begin() > v.end());
}

TYPED_TEST(StlIteratorTest, compareBeginEndWorksAfterInserting)
{
  TypeParam v;
  insert_data(v, 100);
  
  EXPECT_FALSE(v.begin() == v.end());
  EXPECT_TRUE(v.begin() < v.end());
  EXPECT_FALSE(v.begin() > v.end());
}

TYPED_TEST(StlIteratorTest, compareAllIteratorsWorksAfterInserting)
{
  TypeParam v;
  insert_data(v, 100);

  for (auto from = v.begin(); from != v.end(); ++from)
  {
    auto to = v.begin();
    for (; to != from; ++to)
    {
      EXPECT_FALSE(to == from);
      EXPECT_TRUE(to < from);
      EXPECT_FALSE(to > from);
    }

    EXPECT_TRUE(to == from);
    EXPECT_FALSE(to < from);
    EXPECT_FALSE(to > from);
    ++to;

    for (; to != v.end(); ++to)
    {
      EXPECT_FALSE(to == from);
      EXPECT_FALSE(to < from);
      EXPECT_TRUE(to > from);
    }

    EXPECT_TRUE(to == v.end());
    EXPECT_FALSE(to < v.end());
    EXPECT_FALSE(to > v.end());
  }
}

TYPED_TEST(StlIteratorTest, canDereferenceIterators)
{
  TypeParam v;
  insert_data(v, 100);

  // These will fail to compile if they are not defined
  auto it = v.begin();
  *it;
  it->();
}

TYPED_TEST(StlIteratorTest, canAdvanceIterators)
{
  TypeParam v;
  insert_data(v, 100);

  // These will fail to compile if they are not defined
  auto it = v.begin();
  ++it;
  it++;
  std::advance(it);
}
