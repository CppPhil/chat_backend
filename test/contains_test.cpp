#include <vector>

#include <gtest/gtest.h>

#include "lib/contains.hpp"

const std::vector<int> data{1, 2, 3, 4, 5};

TEST(contains, shouldFindElementThatExists)
{
  EXPECT_TRUE(lib::contains(data, 1));
  EXPECT_TRUE(lib::contains(data, 2));
  EXPECT_TRUE(lib::contains(data, 3));
  EXPECT_TRUE(lib::contains(data, 4));
  EXPECT_TRUE(lib::contains(data, 5));
}

TEST(contains, shouldNotFindNonExistantElement)
{
  EXPECT_FALSE(lib::contains(data, 0));
  EXPECT_FALSE(lib::contains(data, 6));
}
