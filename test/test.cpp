#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>
#include "cpcli/app.hpp"
#include <initializer_list>

TEST_CASE("parse test", "[test]") {
  std::vector<std::string> args = {"--bin", "-n", "test"};
  cpcli::App app("test");
  app.parse(args);
  REQUIRE(app.args().size() == 3);
}

