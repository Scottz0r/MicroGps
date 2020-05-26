#include "MicroGpsFormat.h"
#include "catch.hpp"
#include <limits>
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
            bool rc;

            rc = format_lat_ddmm(12.34f, buffer, sizeof(buffer));

            REQUIRE(rc);
            REQUIRE(buffer == "N12 20.4000"s);

            rc = format_lat_ddmm(1.0f, buffer, sizeof(buffer));
            REQUIRE(rc);
            REQUIRE(buffer == "N01 00.0000"s);
        }

        SECTION("it should format good negative latitude numbers")
        {
            char buffer[16];
            bool rc;

            rc = format_lat_ddmm(-34.334455f, buffer, sizeof(buffer));

            REQUIRE(rc);
            REQUIRE(buffer == "S34 20.0672"s);

            rc = format_lat_ddmm(-1.0f, buffer, sizeof(buffer));
            REQUIRE(rc);
            REQUIRE(buffer == "S01 00.0000"s);
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

    TEST_CASE("format_lon_ddmm")
    {
        SECTION("it should format good positive longitude numbers")
        {
            char buffer[16];
            bool rc;

            rc = format_lon_ddmm(120.34f, buffer, sizeof(buffer));

            REQUIRE(rc);
            REQUIRE(buffer == "E120 20.3997"s);

            rc = format_lon_ddmm(1.0f, buffer, sizeof(buffer));
            REQUIRE(rc);
            REQUIRE(buffer == "E001 00.0000"s);
        }

        SECTION("it should format good negative longitude numbers")
        {
            char buffer[16];
            bool rc;

            rc = format_lon_ddmm(-134.334455f, buffer, sizeof(buffer));

            REQUIRE(rc);
            REQUIRE(buffer == "W134 20.0674"s);

            rc = format_lon_ddmm(-1.0f, buffer, sizeof(buffer));
            REQUIRE(rc);
            REQUIRE(buffer == "W001 00.0000"s);
        }

        SECTION("it error small buffer")
        {
            char buffer[4];
            bool rc = format_lon_ddmm(12.34f, buffer, sizeof(buffer));

            REQUIRE_FALSE(rc);
        }

        SECTION("it should error bad input")
        {
            char buffer[32];
            bool rc = format_lon_ddmm(1000.42f, buffer, sizeof(buffer));

            REQUIRE_FALSE(rc);
        }
    }

} // namespace MicroGpsFormat_tests
} // namespace scottz0r