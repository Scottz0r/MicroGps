#ifndef _SCOTTZ0R_NANO_GPS_INCLUDE_GUARD
#define _SCOTTZ0R_NANO_GPS_INCLUDE_GUARD

#include "MicroGpsTypes.h"

namespace scottz0r
{
namespace gps
{

    struct GpsPosition
    {
        unsigned timestamp;
        unsigned char fix_quality;
        unsigned char number_satellites;
        float latitude;
        float longitude;
        float horizontal_dilution;
        float altitude_msl;
        float geoid_height;
    };

    // Detail implementations. Do not used. Exposed for test coverage.
    namespace _detail
    {
        /**
            Safe buffer implementation that does range checking.
        */
        template <size_type _Capacity> class GpsBuffer
        {
        public:
            GpsBuffer() : m_size(0)
            {
            }

            /**
                Attempt to append the character to the buffer. Does nothing and returns false if the buffer capacity
                is exceeded.
            */
            bool append(char c)
            {
                if (m_size < _Capacity)
                {
                    m_buffer[m_size] = c;
                    ++m_size;
                    return true;
                }

                return false;
            }

            /**
                Get the character at the given index, or returns 0 if out of bounds.
            */
            inline char at(size_type index) const
            {
                if (index < size())
                {
                    return m_buffer[index];
                }

                return 0;
            }

            /**
                Get the total number of bytes the buffer can hold.
            */
            constexpr const size_type capacity() const
            {
                return _Capacity;
            }

            /**
                "Clear" the buffer, by resetting collection to start at the beginning. Contents are not reset!
            */
            void clear()
            {
                m_size = 0;
            }

            /**
                Get a pointer to the beginning of the buffer.
            */
            inline const char *get() const
            {
                return m_buffer;
            }

            /**
                Get the current size of the buffer.
            */
            size_type size() const
            {
                return m_size;
            }

        private:
            char m_buffer[_Capacity];
            size_type m_size;
        };

        /**
            Tests if given ASCII character is a digit.
        */
        inline bool is_digit(char c)
        {
            return c >= '0' && c <= '9';
        }

        /**
            Converts given ASCII character to an integer value.
        */
        inline char to_digit(char c)
        {
            if (c >= '0' && c <= '9')
            {
                return c - '0';
            }

            return 0;
        }

        char from_hex(char c);

        bool string_equals(const char *lhs, const char *rhs);

        int string_to_int(const char *val);

        float string_to_float(const char *val);

        float parse_latitude(const char *val, size_type size);

        float parse_longitude(const char *val, size_type size);
    } // namespace _detail

    class MicroGps
    {
        enum class StateBits : unsigned char
        {
            ChecksumBit = 0x01,
            BadBit = 0x02,
            CollectBit = 0x04
        };

    public:
        enum class MessageType : unsigned char
        {
            GPGGA,
            Unknown
        };

        MicroGps();

        bool process(char c);

        /**
            Get the GPS position data. Data will be valid after a GPGGA message has been parsed successfully up to
            the start of the next GPGGA message.
        */
        inline const GpsPosition &position_data()
        {
            return m_position;
        }

        /**
            Returns true if the bad bit is set. This indicates that the last message parse is invalid.
        */
        inline bool bad() const
        {
            return m_bit_flags & (unsigned char)StateBits::BadBit;
        }

        /**
            Returns true if the last message parse was successful.
        */
        inline bool good() const
        {
            return !bad();
        }

        /**
            Get the last parsed message type.
        */
        inline MessageType message_type() const
        {
            return m_message_type;
        }

    private:
        void process_field();

        void process_checksum();

        void process_gpgga_fields();

        unsigned char m_bit_flags; // Booleans, combined to save space.
        _detail::GpsBuffer<32> m_buffer;
        char m_checksum;
        unsigned char m_field_num;
        MessageType m_message_type;
        GpsPosition m_position;
    };

} // namespace gps
} // namespace scottz0r

#endif // _SCOTTZ0R_NANO_GPS_INCLUDE_GUARD
