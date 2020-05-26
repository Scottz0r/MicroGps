#include "MicroGps.h"
#include "catch.hpp"
#include <iostream>
#include <string>

namespace scottz0r
{
namespace MicroGps_tests
{
    using namespace scottz0r::gps;
    using MessageType = MicroGps::MessageType;

    TEST_CASE("MicroGps")
    {
        SECTION("It should process good GPGGA messages with data")
        {
            const std::string msg("$GPGGA,153621.000,3854.8732,N,09445.3680,W,1,04,2.07,243.9,M,-30.1,M,,*5B\r\n");

            MicroGps gps;

            for (std::size_t i = 0; i < msg.size() - 1; ++i)
            {
                bool result = gps.process(msg.at(i));
                REQUIRE_FALSE(result);
            }

            bool result = gps.process(msg.back());
            REQUIRE(result);

            REQUIRE(gps.good());
            REQUIRE_FALSE(gps.bad());
            REQUIRE(gps.message_type() == MessageType::GPGGA);

            const auto &posn = gps.position_data();

            REQUIRE(posn.timestamp == 153621);
            REQUIRE(posn.latitude == Approx(38.0f + (54.8732f / 60.0f)));
            REQUIRE(posn.longitude == Approx(-1.0f * (94.0f + (45.3680f / 60.f))));
            REQUIRE(posn.fix_quality == 1);
            REQUIRE(posn.number_satellites == 4);
            REQUIRE(posn.horizontal_dilution == Approx(2.07));
            REQUIRE(posn.altitude_msl == Approx(243.9));
            REQUIRE(posn.geoid_height == Approx(-30.1));
        }

        SECTION("It should process good GPGGA messages without data")
        {
            const std::string msg("$GPGGA,152541.096,,,,,0,00,,,M,,M,,*71\r\n");

            MicroGps gps;

            for (std::size_t i = 0; i < msg.size() - 1; ++i)
            {
                bool result = gps.process(msg.at(i));
                REQUIRE_FALSE(result);
            }

            bool result = gps.process(msg.back());
            REQUIRE(result);

            REQUIRE(gps.good());
            REQUIRE_FALSE(gps.bad());
            REQUIRE(gps.message_type() == MessageType::GPGGA);

            const auto &posn = gps.position_data();

            REQUIRE(posn.timestamp == 152541);
            REQUIRE(posn.latitude == 0.0f);
            REQUIRE(posn.longitude == 0.0f);
            REQUIRE(posn.fix_quality == 0);
            REQUIRE(posn.number_satellites == 0);
            REQUIRE(posn.horizontal_dilution == 0.0f);
            REQUIRE(posn.altitude_msl == 0.0f);
            REQUIRE(posn.geoid_height == 0.0f);
        }

        SECTION("It should reset position state when new message being processing")
        {
            const std::string msg_0("$GPGGA,153621.000,3854.8732,N,09445.3680,W,1,04,2.07,243.9,M,-30.1,M,,*5B\r\n");
            const std::string msg_1("$GPGGA,152541.096,,,,,0,00,,,M,,M,,*71\r\n");

            MicroGps gps;
            for (const auto& c: msg_0)
            {
                gps.process(c);
            }

            REQUIRE(gps.position_data().number_satellites == 4);

            for (std::size_t i = 0; i < 7; ++i)
            {
                gps.process(msg_1.at(i));
            }

            const auto &posn = gps.position_data();

            REQUIRE(posn.timestamp == 0);
            REQUIRE(posn.latitude == 0.0f);
            REQUIRE(posn.longitude == 0.0f);
            REQUIRE(posn.fix_quality == 0);
            REQUIRE(posn.number_satellites == 0);
            REQUIRE(posn.horizontal_dilution == 0.0f);
            REQUIRE(posn.altitude_msl == 0.0f);
            REQUIRE(posn.geoid_height == 0.0f);
        }

        SECTION("It should fail GPGGA with bad checksum")
        {
            const std::string msg("$GPGGA,153621.000,3854.8732,N,09445.3680,W,1,04,2.07,243.9,M,-30.1,M,,*00\r\n");

            MicroGps gps;

            for (std::size_t i = 0; i < msg.size() - 1; ++i)
            {
                bool result = gps.process(msg.at(i));
                REQUIRE_FALSE(result);
            }

            // Should give a end of message indicator.
            bool result = gps.process(msg.back());
            REQUIRE(result);

            // Good should not be set because of bad checksum.
            REQUIRE_FALSE(gps.good());
            REQUIRE(gps.bad());
            REQUIRE(gps.message_type() == MessageType::GPGGA);

            // GPS data undefined due to bad message.
        }

        SECTION("It should ignore characters before $")
        {
            std::string beg_junk = "ASDF1234,SADF93KA.DFJ";
            std::string msg =
                beg_junk + "$GPGGA,153621.000,3854.8732,N,09445.3680,W,1,04,2.07,243.9,M,-30.1,M,,*5B\r\n";

            MicroGps gps;

            for (std::size_t i = 0; i < msg.size() - 1; ++i)
            {
                bool result = gps.process(msg.at(i));
                REQUIRE_FALSE(result);
            }

            // Should give a end of message indicator.
            bool result = gps.process(msg.back());
            REQUIRE(result);
            REQUIRE(gps.good());
            REQUIRE_FALSE(gps.bad());
        }
    }

    TEST_CASE("_detail::GpsBuffer")
    {
        SECTION("It should collect characters up to capacity.")
        {
            std::string msg("ABCDEFGHIJKLMNOPQRSTUVWXYZ");

            _detail::GpsBuffer<32> buffer;
            for (const auto &c : msg)
            {
                buffer.append(c);
            }

            REQUIRE(buffer.size() == msg.size());

            for (std::size_t i = 0; i < msg.size(); ++i)
            {
                REQUIRE(msg.at(i) == buffer.at(i));
            }
        }

        SECTION("It should return 0 index out of bounds")
        {
            _detail::GpsBuffer<32> buffer;
            buffer.append('a');

            REQUIRE(buffer.at(0) == 'a');
            REQUIRE(buffer.at(123) == 0);
        }

        SECTION("It should not overflow")
        {
            _detail::GpsBuffer<4> buffer;

            REQUIRE(buffer.append('Z'));
            REQUIRE(buffer.append('Z'));
            REQUIRE(buffer.append('Z'));
            REQUIRE(buffer.append('Z'));

            REQUIRE_FALSE(buffer.append('X'));
            REQUIRE(buffer.size() == 4);
        }
    }

    TEST_CASE("_detail::is_digit")
    {
        REQUIRE(_detail::is_digit('0'));
        REQUIRE(_detail::is_digit('1'));
        REQUIRE(_detail::is_digit('2'));
        REQUIRE(_detail::is_digit('3'));
        REQUIRE(_detail::is_digit('4'));
        REQUIRE(_detail::is_digit('5'));
        REQUIRE(_detail::is_digit('6'));
        REQUIRE(_detail::is_digit('7'));
        REQUIRE(_detail::is_digit('8'));
        REQUIRE(_detail::is_digit('9'));

        REQUIRE_FALSE(_detail::is_digit('A'));
        REQUIRE_FALSE(_detail::is_digit('Z'));
        REQUIRE_FALSE(_detail::is_digit(0));
    }

    TEST_CASE("_detail::to_digit")
    {
        REQUIRE(_detail::to_digit('0') == 0);
        REQUIRE(_detail::to_digit('1') == 1);
        REQUIRE(_detail::to_digit('2') == 2);
        REQUIRE(_detail::to_digit('3') == 3);
        REQUIRE(_detail::to_digit('4') == 4);
        REQUIRE(_detail::to_digit('5') == 5);
        REQUIRE(_detail::to_digit('6') == 6);
        REQUIRE(_detail::to_digit('7') == 7);
        REQUIRE(_detail::to_digit('8') == 8);
        REQUIRE(_detail::to_digit('9') == 9);

        REQUIRE(_detail::to_digit('A') == 0);
        REQUIRE(_detail::to_digit('Z') == 0);
        REQUIRE(_detail::to_digit(0) == 0);
    }

    TEST_CASE("_detail::from_hex")
    {
        SECTION("it should convert numbers")
        {
            REQUIRE(_detail::from_hex('0') == 0);
            REQUIRE(_detail::from_hex('1') == 1);
            REQUIRE(_detail::from_hex('2') == 2);
            REQUIRE(_detail::from_hex('3') == 3);
            REQUIRE(_detail::from_hex('4') == 4);
            REQUIRE(_detail::from_hex('5') == 5);
            REQUIRE(_detail::from_hex('6') == 6);
            REQUIRE(_detail::from_hex('7') == 7);
            REQUIRE(_detail::from_hex('8') == 8);
            REQUIRE(_detail::from_hex('9') == 9);
        }

        SECTION("it should convert upper A-F")
        {
            REQUIRE(_detail::from_hex('A') == 10);
            REQUIRE(_detail::from_hex('B') == 11);
            REQUIRE(_detail::from_hex('C') == 12);
            REQUIRE(_detail::from_hex('D') == 13);
            REQUIRE(_detail::from_hex('E') == 14);
            REQUIRE(_detail::from_hex('F') == 15);
        }

        SECTION("it should convert upper a-f")
        {
            REQUIRE(_detail::from_hex('a') == 10);
            REQUIRE(_detail::from_hex('b') == 11);
            REQUIRE(_detail::from_hex('c') == 12);
            REQUIRE(_detail::from_hex('d') == 13);
            REQUIRE(_detail::from_hex('e') == 14);
            REQUIRE(_detail::from_hex('f') == 15);
        }

        SECTION("it should return 0 bad input")
        {
            REQUIRE(_detail::from_hex('Z') == 0);
            REQUIRE(_detail::from_hex(-35) == 0);
            REQUIRE(_detail::from_hex(0x10) == 0);
        }
    }

    TEST_CASE("_detail::string_equals")
    {
        SECTION("equal strings")
        {
            REQUIRE(_detail::string_equals("ABC", "ABC"));
        }

        SECTION("not equal strings")
        {
            REQUIRE_FALSE(_detail::string_equals("ABC", "ABCDEF"));
            REQUIRE_FALSE(_detail::string_equals("QWERTY", "WASD"));
        }

        SECTION("null pointers")
        {
            REQUIRE_FALSE(_detail::string_equals("QWERTY", nullptr));
            REQUIRE_FALSE(_detail::string_equals(nullptr, "QWERTY"));
            REQUIRE_FALSE(_detail::string_equals(nullptr, nullptr));
        }
    }

    TEST_CASE("_detail::string_to_int")
    {
        SECTION("It should convert integers")
        {
            REQUIRE(_detail::string_to_int("10") == 10);
            REQUIRE(_detail::string_to_int("1324") == 1324);
            REQUIRE(_detail::string_to_int("159.88") == 159);
        }

        SECTION("It should convert stop non numeric")
        {
            REQUIRE(_detail::string_to_int("159aab") == 159);
            REQUIRE(_detail::string_to_int("abc150") == 0);
        }

        SECTION("It return 0 null input")
        {
            REQUIRE(_detail::string_to_int(nullptr) == 0);
        }
    }

    TEST_CASE("_detail::string_to_float")
    {
        SECTION("It should convert numbers with decimals")
        {
            REQUIRE(_detail::string_to_float("10") == 10.0f);
            REQUIRE(_detail::string_to_float("1324") == 1324.0f);
            REQUIRE(_detail::string_to_float("159.88") == Approx(159.88f));
        }

        SECTION("It should convert stop non numeric")
        {
            REQUIRE(_detail::string_to_float("159.123aab") == Approx(159.123f));
            REQUIRE(_detail::string_to_float("abc150.15") == 0);
        }

        SECTION("It return 0 null input")
        {
            REQUIRE(_detail::string_to_float(nullptr) == 0.0f);
        }
    }

    TEST_CASE("_detail::parse_latitude")
    {
        SECTION("it should parse good input")
        {
            const char input[] = "3854.8732";
            constexpr float expected = 38.0f + (54.8732f / 60.0f);
            REQUIRE(_detail::parse_latitude(input, sizeof(input)) == Approx(expected));
        }

        SECTION("it should return 0 bad input")
        {
            const char input[] = "";
            REQUIRE(_detail::parse_latitude(input, sizeof(input)) == 0.0f);
        }
    }

    TEST_CASE("_detail::parse_longitude")
    {
        SECTION("it should parse good input")
        {
            const char input[] = "09445.3680";
            constexpr float expected = 94.0f + (45.3680f / 60.0f);
            REQUIRE(_detail::parse_longitude(input, sizeof(input)) == Approx(expected));
        }

        SECTION("it should return 0 bad input")
        {
            const char input[] = "";
            REQUIRE(_detail::parse_longitude(input, sizeof(input)) == 0.0f);
        }
    }

} // namespace MicroGps_tests
} // namespace scottz0r