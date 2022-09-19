#include <vector>

#include <doctest.h>

#include "lib/contains.hpp"

const std::vector<int> data{1, 2, 3, 4, 5};

TEST_CASE("contains, shouldFindElementThatExists")
{
  CHECK_UNARY(lib::contains(data, 1));
  CHECK_UNARY(lib::contains(data, 2));
  CHECK_UNARY(lib::contains(data, 3));
  CHECK_UNARY(lib::contains(data, 4));
  CHECK_UNARY(lib::contains(data, 5));
}

TEST_CASE("contains, shouldNotFindNonExistantElement")
{
  CHECK_UNARY_FALSE(lib::contains(data, 0));
  CHECK_UNARY_FALSE(lib::contains(data, 6));
}
