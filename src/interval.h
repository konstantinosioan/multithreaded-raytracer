#ifndef INTERVAL_H
#define INTERVAL_H

#include "rtweekend.h"

/// @brief A range of real numbers between a minimum and a maximum
class Interval
{
  public:
	double min{};
	double max{};

	/// @brief Constructs an empty interval, containing nothing
	/// @note The bounds are deliberately inverted, so no value can fall
	///       inside them
	Interval() : min{POSITIVE_INFINITY}, max{-POSITIVE_INFINITY} {}

	/// @brief Constructs an interval from its bounds
	/// @param min The lower bound
	/// @param max The upper bound
	Interval(double min, double max) : min{min}, max{max} {}

	/// @brief Returns how wide the interval is
	double size() const { return max - min; }

	/// @brief Returns whether x falls within the interval, bounds included
	/// @param x The value to test
	bool contains(double x) const { return min <= x && max >= x; }

	/// @brief Returns whether x falls strictly inside the interval
	/// @param x The value to test
	/// @note Unlike contains(), a value sitting exactly on a bound does
	///       not count
	bool surrounds(double x) const { return min < x && max > x; }

	/// @brief Returns x pulled back into the interval if it falls outside
	/// @param x The value to clamp
	double clamp(double x) const
	{
		if (x < min)
		{
			return min;
		}

		if (x > max)
		{
			return max;
		}

		return x;
	}

	static const Interval empty, universe;
};

/// @brief An interval containing nothing
inline const Interval Interval::empty{POSITIVE_INFINITY, -POSITIVE_INFINITY};

/// @brief An interval containing every real number
inline const Interval Interval::universe{-POSITIVE_INFINITY,
										 POSITIVE_INFINITY};

#endif // INTERVAL_H
