#ifndef COLOUR_H
#define COLOUR_H

#include "vec3.h"

#include <iostream>

using colour = vec3;

constexpr int MAX_COLOUR_VALUE{255};

/// @brief Writes a single pixel's colour to a stream as three
///        space-separated byte values
/// @param out The output stream
/// @param pixel_colour The colour to write, with components expected in [0, 1]
/// @note No clamping; components outside [0, 1] produce an
///       out-of-range byte value
void write_colour(std::ostream& out, const colour& pixel_colour)
{
	constexpr double colour_scale{MAX_COLOUR_VALUE + 0.999};

	double r{pixel_colour.x()};
	double g{pixel_colour.y()};
	double b{pixel_colour.z()};

	// translate the [0, 1] component values to the byte range [0, 255]
	int rbyte{static_cast<int>(colour_scale * r)};
	int gbyte{static_cast<int>(colour_scale * g)};
	int bbyte{static_cast<int>(colour_scale * b)};

	// write out the pixel colour components
	out << rbyte << ' ' << gbyte << ' ' << bbyte << '\n';
}

#endif // COLOUR_H
