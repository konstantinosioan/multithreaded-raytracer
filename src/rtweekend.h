#ifndef RTWEEKEND_H
#define RTWEEKEND_H

#include <limits>

constexpr double POSITIVE_INFINITY{std::numeric_limits<double>::infinity()};
constexpr double PI{3.1415926535897932385};

/// @brief Converts an angle from degrees to radians
/// @param degrees The angle in degrees
/// @return The equivalent angle in radians
inline double degrees_to_radians(double degrees)
{
	return degrees * PI / 180.0;
}

#endif // RTWEEKEND_H
