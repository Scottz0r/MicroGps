#include "MicroGpsFormat.h"
#include "catch.hpp"
#include <string>

namespace scottz0r
{
namespace MicroGpsFormat_tests
{
    using namespace std::string_literals;
    using namespace scottz0r::gps;

    TEST_CASE("format_lat_ddmm")
    {
        SECTION("it should format good positive latitude numbers")
        {
            char buffer[16];
            bool rc = format_lat_ddmm(12.34f, buffer, sizeof(buffer));
            auto expected = "N12 20.4000"s;

            REQUIRE(rc);
            REQUIRE(buffer == expected);
        }

        SECTION("it should format good negative latitude numbers")
        {
            char buffer[16];
            bool rc = format_lat_ddmm(-34.334455f, buffer, sizeof(buffer));
            auto expected = "S34 20.0672"s;

            REQUIRE(rc);
            REQUIRE(buffer == expected);
        }

        SECTION("it error small buffer")
        {
            char buffer[4];
            bool rc = format_lat_ddmm(12.34f, buffer, sizeof(buffer));

            REQUIRE_FALSE(rc);
        }

        SECTION("it should error bad input")
        {
            char buffer[32];
            bool rc = format_lat_ddmm(1000.42f, buffer, sizeof(buffer));

            REQUIRE_FALSE(rc);
        }
    }

} // namespace MicroGpsFormat_tests
} // namespace scottz0r