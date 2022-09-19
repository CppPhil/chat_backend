#include <string>

#include <doctest.h>

#include "lib/client_list_message.hpp"

TEST_CASE("ClientListMessage, shouldBeAbleToParseValidJson")
{
  const std::string            json{"[\"192.168.123.1\",\"123.456.789.1\"]"};
  const lib::ClientListMessage clientListMessage{json};
  CHECK_EQ(json, clientListMessage.asJson());
}
