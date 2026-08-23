#ifndef RTWEEKEND_H
#define RTWEEKEND_H

#include <chrono>
#include <limits>
#include <random>

constexpr double POSITIVE_INFINITY{std::numeric_limits<double>::infinity()};
constexpr double PI{3.1415926535897932385};

/// @brief Converts an angle from degrees to radians
/// @param degrees The angle in degrees
/// @return The equivalent angle in radians
inline double degrees_to_radians(double degrees)
{
	return degrees * PI / 180.0;
}

/// @brief Returns a random real number in [0, 1)
inline double random_double()
{
	static std::mt19937 generator{
		[]()
		{
			std::random_device rd{};
			std::seed_seq ss{static_cast<std::seed_seq::result_type>(
								 std::chrono::steady_clock::now()
									 .time_since_epoch()
									 .count()),
							 rd(),
							 rd(),
							 rd(),
							 rd(),
							 rd(),
							 rd(),
							 rd()};
			return std::mt19937{ss};
		}()};

	return std::uniform_real_distribution<double>{0.0, 1.0}(generator);
}

/// @brief Returns a random real number in [min, max)
/// @param min The lower bound, inclusive
/// @param max The upper bound, exclusive
inline double random_double(double min, double max)
{
	return min + (max - min) * random_double();
}

#endif // RTWEEKEND_H
