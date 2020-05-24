#include "MicroGps.h"

namespace scottz0r
{
namespace gps
{

    /**
        Returns true if the given bit flag is set in integral type x.
    */
    template <typename _T, typename _F> inline bool is_flag_set(_T x, _F flag)
    {
        return (x & (_T)flag) > 0;
    }

    /**
        Set the given bit flag on the integral type x.
    */
    template <typename _T, typename _F> inline _T set_flag(_T x, _F flag)
    {
        return x | (_T)flag;
    }

    /**
        Clear the given bit flag on the integral type x.
    */
    template <typename _T, typename _F> inline _T clear_flag(_T x, _F flag)
    {
        return x & (~((_T)flag));
    }

    namespace _detail
    {
        /**
            Convert a base 16 ASCII character into an char/integer.
        */
        char from_hex(char c)
        {
            if (c >= '0' && c <= '9')
            {
                return c - '0';
            }
            else if (c >= 'a' && c <= 'f')
            {
                return c - 'a' + 10;
            }
            else if (c >= 'A' && c <= 'F')
            {
                return c - 'A' + 10;
            }
            else
            {
                return 0;
            }
        }

        /**
            Checks string equality with character arrays. Behavior is undefined is character array is not null
            terminated. Returns false if either input is null.
        */
        bool string_equals(const char *lhs, const char *rhs)
        {
            // Return false if either input is null.
            if (!lhs || !rhs)
            {
                return false;
            }

            do
            {
                if (*lhs == 0)
                {
                    return *rhs == 0;
                }

                ++lhs;
                ++rhs;
            } while (*lhs == *rhs);

            return false;
        }

        /**
            Converts an ASCII character array into an integer. Behavior is undefined is character array is not null
            terminated.
        */
        int string_to_int(const char *val)
        {
            if (!val)
            {
                return 0;
            }

            int result = 0;
            bool is_negative = false;

            // Handle negative and plus signs.
            if (*val == '-' || *val == '+')
            {
                ++val;
                is_negative = true;
            }

            while (*val != 0)
            {
                if (is_digit(*val))
                {
                    result *= 10;
                    result += to_digit(*val);
                }
                else
                {
                    break;
                }
                ++val;
            }

            if (is_negative)
            {
                result *= -1;
            }

            return result;
        }

        /**
            Converts an ASCII character array into a float. Behavior is undefined is character array is not null
            terminated.
        */
        float string_to_float(const char *val)
        {
            if (!val)
            {
                return 0.0f;
            }

            float result = 0.0f;
            char e = 0;
            bool found_dot = false;
            bool is_negative = false;

            // Handle negative and plus signs.
            if (*val == '-' || *val == '+')
            {
                is_negative = true;
                ++val;
            }

            while (*val != 0)
            {
                if (is_digit(*val))
                {
                    result *= 10;
                    result += to_digit(*val);

                    if (found_dot)
                    {
                        ++e;
                    }
                }
                else if (*val == '.' && !found_dot)
                {
                    found_dot = true;
                }
                else
                {
                    break;
                }
                ++val;
            }

            while (e > 0)
            {
                --e;
                result /= 10.0f;
            }

            if (is_negative)
            {
                result *= -1.0f;
            }

            return result;
        }

        /**
            Parse a NMEA latitude string.
        */
        float parse_latitude(const char *val, size_type size)
        {
            if (size < 2)
            {
                return 0.0f;
            }

            char deg_buffer[3];
            deg_buffer[0] = val[0];
            deg_buffer[1] = val[1];
            deg_buffer[2] = 0;

            float degrees = string_to_float(deg_buffer);
            float minutes = string_to_float(val + 2);

            degrees += (minutes / 60.0f);
            return degrees;
        }

        /**
            Parse a NMEA longitude string.
        */
        float parse_longitude(const char *val, size_type size)
        {
            if (size < 3)
            {
                return 0.0f;
            }

            char deg_buffer[4];
            deg_buffer[0] = val[0];
            deg_buffer[1] = val[1];
            deg_buffer[2] = val[2];
            deg_buffer[3] = 0;

            float degrees = string_to_float(deg_buffer);
            float minutes = string_to_float(val + 3);

            degrees += (minutes / 60.0f);
            return degrees;
        }
    } // namespace _detail

    using namespace scottz0r::gps::_detail;

    MicroGps::MicroGps()
        : m_bit_flags(0), m_checksum(0), m_field_num(0), m_message_type(MessageType::Unknown), m_position()
    {
    }

    /**
        Process a character in an NMEA message.

        @param c    Character to process.
        @return     True is a message is ready. False if a message is still being processed.
    */
    bool MicroGps::process(char c)
    {
        // Start of sentence. Reset collection state.
        if (c == '$')
        {
            m_buffer.clear();
            m_checksum = 0;
            m_bit_flags = 0;
            m_field_num = 0;
            m_bit_flags = set_flag(m_bit_flags, StateBits::CollectBit);
            m_message_type = MessageType::Unknown;
            return false;
        }

        // If not in a collection state, then do not attempt to process.
        if (!is_flag_set(m_bit_flags, StateBits::CollectBit))
        {
            return false;
        }

        // Don't process if in a bad state.
        if (is_flag_set(m_bit_flags, StateBits::BadBit))
        {
            return false;
        }

        // Do not process if has a message identifier and message is unknown.
        if (m_field_num > 1 && m_message_type == MessageType::Unknown)
        {
            return false;
        }

        switch (c)
        {
        case '$':
            // Code coverage exclusion: This case is already handled.
            return false;

        case ',':
            // field separator. Used in checksum.
            m_checksum ^= c;

            // If buffer cannot be terminated set bad state and do not process.
            if (!m_buffer.append(0))
            {
                m_bit_flags = set_flag(m_bit_flags, StateBits::BadBit);
                return false;
            }

            process_field();
            ++m_field_num;
            m_buffer.clear();
            return false;

        case '*':
            // Checksum indicator. End current field.
            if (!m_buffer.append(0))
            {
                m_bit_flags = set_flag(m_bit_flags, StateBits::BadBit);
                return false;
            }

            process_field();
            ++m_field_num;
            m_buffer.clear();

            // Set to checksum collecting state.
            m_bit_flags = set_flag(m_bit_flags, StateBits::ChecksumBit);
            return false;

        case '\r':
            // Do nothing on carriage return. Newline is the real message terminator for this logic.
            return false;

        case '\n':
            // End of message. Turn off collection state.
            m_bit_flags = clear_flag(m_bit_flags, StateBits::CollectBit);

            // If there is a checksum, then need to do stuff here.
            if (is_flag_set(m_bit_flags, StateBits::ChecksumBit))
            {
                // Do not need to null terminate for checksum.
                process_checksum();

                // Return indicator that message is ready.
                return true;
            }

            // No checksum. This is a failure condition.
            m_bit_flags = set_flag(m_bit_flags, StateBits::BadBit);
            return false;

        default:
            // Collect character if in bounds. Don't add to checksum if it's the checksum field.
            if (!is_flag_set(m_bit_flags, StateBits::ChecksumBit))
            {
                m_checksum ^= c;
            }

            // If buffer is full, set to bad state.
            if (!m_buffer.append(c))
            {
                m_bit_flags = set_flag(m_bit_flags, StateBits::BadBit);
            }

            return false;
        }
    }

    /**
        Process a field, which is contained in the field buffer. Fields will be null terminated. The first field is
        always used as a message identifier, which drives m_message_type.
    */
    void MicroGps::process_field()
    {
        if (m_field_num == 0)
        {
            if (string_equals(m_buffer.get(), "GPGGA"))
            {
                m_message_type = MessageType::GPGGA;
            }
            else
            {
                m_message_type = MessageType::Unknown;
            }
        }
        else
        {
            switch (m_message_type)
            {
            case MessageType::GPGGA:
                process_gpgga_fields();
                break;
            default:
                // Do nothing.
                break;
            }
        }
    }

    /**
        Process GPGGA message fields.
    */
    void MicroGps::process_gpgga_fields()
    {
        switch (m_field_num)
        {
        case 1:
            // Time
            m_position.timestamp = (unsigned)string_to_int(m_buffer.get());
            break;
        case 2: {
            // Latitude
            m_position.latitude = parse_latitude(m_buffer.get(), m_buffer.size());
            break;
        }
        case 3:
            // Latitude North/South
            if (m_buffer.at(0) == 'S')
            {
                m_position.latitude *= -1;
            }
            break;
        case 4:
            // Longitude East/West
            m_position.longitude = parse_longitude(m_buffer.get(), m_buffer.size());
            break;
        case 5:
            if (m_buffer.at(0) == 'W')
            {
                m_position.longitude *= -1;
            }
            break;
        case 6:
            // Fix Quality
            m_position.fix_quality = (unsigned char)string_to_int(m_buffer.get());
            break;
        case 7:
            // Number of satellites
            m_position.number_satellites = (unsigned char)string_to_int(m_buffer.get());
            break;
        case 8:
            // HDOP
            m_position.horizontal_dilution = string_to_float(m_buffer.get());
            break;
        case 9:
            // Altitude
            m_position.altitude_msl = string_to_float(m_buffer.get());
            break;
        case 11:
            // Geoid Adjustment to WGS-84
            m_position.geoid_height = string_to_float(m_buffer.get());
            break;
        case 10:
        case 12:
        case 13:
        case 14:
            break; // Ignore these fields.
        default:
            // Set bad to indicate unexpected message format.
            m_bit_flags = set_flag(m_bit_flags, StateBits::BadBit);
        }
    }

    /**
        Process the checksum. Sets the bad bit if the computed checksum does not match the message checksum.
        Assumes message checksum is only 2 hex characters.
    */
    void MicroGps::process_checksum()
    {
        if (m_buffer.size() > 2)
        {
            set_flag(m_bit_flags, StateBits::BadBit);
            return;
        }

        char msg_checksum = (from_hex(m_buffer.at(0)) << 4) | from_hex(m_buffer.at(1));

        if (msg_checksum != m_checksum)
        {
            m_bit_flags = set_flag(m_bit_flags, StateBits::BadBit);
        }
    }
} // namespace gps
} // namespace scottz0r
