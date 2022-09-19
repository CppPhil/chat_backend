#include <stdexcept>
#include <string>

#include <doctest.h>

#include "lib/net_string.hpp"

static lib::NetString parseNetString(const std::string& string)
{
  return lib::NetString{lib::FromNetStringData{}, string.data(), string.size()};
}

using namespace std::string_literals;

TEST_CASE("NetString, shouldParseValidNetString")
{
  const std::string    string{"15:This is a text.,"};
  const lib::NetString netString{parseNetString(string)};
  CHECK_EQ("This is a text."s, netString.asPlainString());
  CHECK_EQ(string, netString.asNetString());
}

TEST_CASE("NetString, shouldParseOneElementNetString")
{
  const std::string    string{"1:A,"};
  const lib::NetString netString{parseNetString(string)};
  CHECK_EQ("A"s, netString.asPlainString());
  CHECK_EQ(string, netString.asNetString());
}

TEST_CASE("NetString, shouldFailToParseStringWithoutColon")
{
  const std::string string{"5Hello,"};
  CHECK_THROWS_AS(parseNetString(string), std::runtime_error);
}

TEST_CASE("NetString, shouldFailToParseStringWithInvalidSize")
{
  const std::string string{"oueuoeiu:Test,"};
  CHECK_THROWS_AS(parseNetString(string), std::runtime_error);
}

TEST_CASE("NetString, shouldFailToParseStringWithIncorrectSize")
{
  const std::string string1{"28:This is a very lengthy string,"};
  const std::string string2{"50:This is a very short string,"};
  CHECK_THROWS_AS(parseNetString(string1), std::runtime_error);
  CHECK_THROWS_AS(parseNetString(string2), std::runtime_error);
}

TEST_CASE("NetString, shouldFailToParseStringWithoutComma")
{
  const std::string string{"5:Hello"};
  CHECK_THROWS_AS(parseNetString(string), std::runtime_error);
}
