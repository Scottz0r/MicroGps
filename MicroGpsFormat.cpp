#include "MicroGpsFormat.h"

namespace scottz0r
{
namespace gps
{
    static char *format_ddmm_min_part(char *dst, float minutes);

    /// @brief Format latitude in degrees into NDD MM.MMMM format. Destination buffer must be 16 characters or
    /// larger.
    ///
    /// @param deg Degrees of latitude.
    /// @param dst Destination character buffer.
    /// @param dst_size Size of destination buffer.
    /// @return True if format was successful.
    bool format_lat_ddmm(float deg, char *dst, size_type dst_size)
    {
        // Required minimum buffer size.
        if (!dst || dst_size < 16)
        {
            return false;
        }

        // Valid latitude range check.
        if (deg < -90.0f || deg > 90.0f)
        {
            return false;
        }

        char *p_dst = dst;

        // North vs South. Always make the degrees absolute value to avoid negative signs.
        float abs_deg;
        if (deg < 0)
        {
            *p_dst = 'S';
            abs_deg = -1 * deg;
        }
        else
        {
            *p_dst = 'N';
            abs_deg = deg;
        }
        ++p_dst;

        // This format is very specifically dd mm.mmmm, so a very direct formatting method can be used. This requires
        // that input be check for bounds before running this chunk.

        // Whole number (two digits).
        int whole_part = (int)abs_deg;
        *p_dst = (whole_part / 10) + '0';
        ++p_dst;
        whole_part %= 10;

        *p_dst = whole_part + '0';
        ++p_dst;

        *p_dst = ' ';
        ++p_dst;

        // Convert remainder degrees into minutes.
        float minutes = (abs_deg - (int)abs_deg) * 60.0f;
        p_dst = format_ddmm_min_part(p_dst, minutes);
        *p_dst = 0;

        return true;
    }

    /// @brief  Format longitude in degrees into NDDD MM.MMMM format. Destination buffer must be 16 characters or
    /// larger.
    ///
    /// @param deg Degrees of longitude.
    /// @param dst Destination character buffer.
    /// @param dst_size Size of destination character buffer.
    /// @return True if format was successful.
    bool format_lon_ddmm(float deg, char *dst, size_type dst_size)
    {
        // Required minimum buffer size.
        if (!dst || dst_size < 16)
        {
            return false;
        }

        // Valid longitude range check
        if (deg < -180.0 || deg > 180.0)
        {
            return false;
        }

        char *p_dst = dst;

        // East vs West. Always make the degrees absolute value to avoid negative signs.
        float abs_deg;
        if (deg < 0)
        {
            *p_dst = 'W';
            abs_deg = -1 * deg;
        }
        else
        {
            *p_dst = 'E';
            abs_deg = deg;
        }
        ++p_dst;

        // Whole number (three digits).
        int whole_part = (int)abs_deg;
        *p_dst = (whole_part / 100) + '0';
        ++p_dst;
        whole_part %= 100;

        *p_dst = (whole_part / 10) + '0';
        ++p_dst;
        whole_part %= 10;

        *p_dst = whole_part + '0';
        ++p_dst;

        *p_dst = ' ';
        ++p_dst;

        // Convert remainder degrees into minutes.
        float minutes = (abs_deg - (int)abs_deg) * 60.0f;
        p_dst = format_ddmm_min_part(p_dst, minutes);
        *p_dst = 0;

        return true;
    }

    /// @brief Format the MM.MMMM part of latitude or longitude. Does very explicit formatting and assumes destination
    /// is large enough.
    ///
    /// @param dst Format destination buffer.
    /// @param minutes The minutes to format.
    /// @return A pointer to the next char in dst after formatted characters.
    static char *format_ddmm_min_part(char *dst, float minutes)
    {
        int whole_part;
        char *p_dst = dst;

        whole_part = (int)minutes;

        *p_dst = (whole_part / 10) + '0';
        ++p_dst;
        whole_part %= 10;

        *p_dst = (whole_part) + '0';
        ++p_dst;

        *p_dst = '.';
        ++p_dst;

        // Remaining 4 decimal digits of minutes.
        whole_part = (int)((minutes - (int)minutes) * 10000.0f);

        *p_dst = (whole_part / 1000) + '0';
        ++p_dst;
        whole_part %= 1000;

        *p_dst = (whole_part / 100) + '0';
        ++p_dst;
        whole_part %= 100;

        *p_dst = (whole_part / 10) + '0';
        ++p_dst;
        whole_part %= 10;

        *p_dst = (whole_part) + '0';
        ++p_dst;

        return p_dst;
    }

} // namespace gps
} // namespace scottz0r
