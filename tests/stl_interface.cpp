#include <gtest/gtest.h>

#include "helpers.hpp"

template <typename T>
class StlInterfaceTest : public ::testing::Test
{
};

// Retro types with STL interfaces
typedef ::testing::Types<
  retro::partial_queue<int>,
  retro::detail::ordered_list<int>
> RetroTypes;

TYPED_TEST_CASE(StlInterfaceTest, RetroTypes);

TYPED_TEST(StlInterfaceTest, emptyWhenInitiallyCreated)
{
  TypeParam v;
  ASSERT_EQ(0U, v.size());
  ASSERT_TRUE(v.empty());
}

TYPED_TEST(StlInterfaceTest, sizeIsCorrectAfterInsertingElements)
{
  TypeParam v;
  insert_data(v, 100);
  ASSERT_EQ(100U, v.size());
  ASSERT_FALSE(v.empty());
}

TYPED_TEST(StlInterfaceTest, hasSizeTypeTypedef)
{
  typename TypeParam::size_type i;
  ASSERT_EQ(i, i);
}

TYPED_TEST(StlInterfaceTest, hasMaxSize)
{
  TypeParam v;
  ASSERT_EQ(v.max_size(), v.max_size());
}
