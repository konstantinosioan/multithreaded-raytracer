#ifndef RTWEEKEND_H
#define RTWEEKEND_H

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

// thread_local, so each worker owns its generator and threads never share
// RNG state
inline thread_local std::mt19937 generator{};

/// @brief Reseeds this thread's generator from a tile index
/// @param tile_index The tile about to be rendered
/// @note Seeding from the tile rather than the thread is what makes the
///       render reproducible, whichever worker claims the tile
/// @note seed_seq mixes the index, so neighbouring tiles get unrelated
///       sequences instead of near-identical ones
inline void seed_generator(int tile_index)
{
	std::seed_seq ss{tile_index};
	generator.seed(ss);
}

/// @brief Returns a random real number in [0, 1)
inline double random_double()
{
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
