#ifndef _SCOTTZ0R_GPS_FORMAT_INCLUDE_GUARD
#define _SCOTTZ0R_GPS_FORMAT_INCLUDE_GUARD

#include "MicroGpsTypes.h"

namespace scottz0r
{
namespace gps
{
    bool format_lat_ddmm(float deg, char *dst, size_type dst_size);

    bool format_lon_ddmm(float deg, char *dst, size_type dst_size);

    bool format_deg_ddmm(float deg, char *dst, size_type dst_size);

    size_type int_to_string(int number, char *dst, size_type dst_size);

    // TODO: Float to string.

} // namespace gps
} // namespace scottz0r

#endif // _SCOTTZ0R_GPS_FORMAT_INCLUDE_GUARD
