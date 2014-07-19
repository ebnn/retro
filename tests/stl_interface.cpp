#include <gtest/gtest.h>

#include "retro/queue.hpp"
#include "retro/detail/ordered_list.hpp"

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
