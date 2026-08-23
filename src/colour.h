#ifndef COLOUR_H
#define COLOUR_H

#include "interval.h"
#include "vec3.h"

#include <iostream>

using colour = Vec3;

constexpr int MAX_COLOUR_VALUE{255};

/// @brief Converts a colour component from linear space to gamma space
/// @param linear_component The component to convert
/// @note Uses a gamma of 2, so the transform is just a square root
inline double linear_to_gamma(double linear_component)
{
	if (linear_component > 0)
	{
		return std::sqrt(linear_component);
	}

	return 0;
}

/// @brief Writes a single pixel's colour to a stream as three
///        space-separated byte values
/// @param out The output stream
/// @param pixel_colour The colour to write; components may exceed [0, 1]
///        (e.g. from summed multi-sample averaging) and will be clamped
/// @note Components are clamped to [0, 1] before being scaled to a byte
inline void write_colour(std::ostream& out, const colour& pixel_colour)
{
	double r{pixel_colour.x()};
	double g{pixel_colour.y()};
	double b{pixel_colour.z()};

	// image viewers expect gamma space, not the linear values computed
	// above; without this the image renders too dark
	r = linear_to_gamma(r);
	g = linear_to_gamma(g);
	b = linear_to_gamma(b);

	// translate the [0, 1] component values to the byte range [0, 255]
	static const Interval intensity{0.000, 0.999};
	int rbyte{static_cast<int>((MAX_COLOUR_VALUE + 1) * intensity.clamp(r))};
	int gbyte{static_cast<int>((MAX_COLOUR_VALUE + 1) * intensity.clamp(g))};
	int bbyte{static_cast<int>((MAX_COLOUR_VALUE + 1) * intensity.clamp(b))};

	// write out the pixel colour components
	out << rbyte << ' ' << gbyte << ' ' << bbyte << '\n';
}

#endif // COLOUR_H
