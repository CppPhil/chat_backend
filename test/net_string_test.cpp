#include <stdexcept>
#include <string>

#include <gtest/gtest.h>

#include "lib/net_string.hpp"

static lib::NetString parseNetString(const std::string& string)
{
  return lib::NetString{lib::FromNetStringData{}, string.data(), string.size()};
}

using namespace std::string_literals;

TEST(NetString, shouldParseValidNetString)
{
  const std::string    string{"16:This is a text.,"};
  const lib::NetString netString{parseNetString(string)};
  EXPECT_EQ("This is a text."s, netString.asPlainString());
  EXPECT_EQ(string, netString.asNetString());
}

TEST(NetString, shouldParseEmptyNetString)
{
  const std::string    string{"0:,"};
  const lib::NetString netString{parseNetString(string)};
  EXPECT_EQ(""s, netString.asPlainString());
  EXPECT_EQ(string, netString.asNetString());
}

TEST(NetString, shouldParseOneElementNetString)
{
  const std::string    string{"1:A,"};
  const lib::NetString netString{parseNetString(string)};
  EXPECT_EQ("A"s, netString.asPlainString());
  EXPECT_EQ(string, netString.asNetString());
}

TEST(NetString, shouldFailToParseStringWithoutColon)
{
  const std::string string{"5Hello,"};
  EXPECT_THROW(parseNetString(string), std::runtime_error);
}

TEST(NetString, shouldFailToParseStringWithInvalidSize)
{
  const std::string string{"oueuoeiu:Test,"};
  EXPECT_THROW(parseNetString(string), std::runtime_error);
}

TEST(NetString, shouldFailToParseStringWithIncorrectSize)
{
  const std::string string1{"29:This is a very lengthy string,"};
  const std::string string2{"50:This is a very short string,"};
  EXPECT_THROW(parseNetString(string1), std::runtime_error);
  EXPECT_THROW(parseNetString(string2), std::runtime_error);
}

TEST(NetString, shouldFailToParseStringWithoutComma)
{
  const std::string string{"5:Hello"};
  EXPECT_THROW(parseNetString(string), std::runtime_error);
}
