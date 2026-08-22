#ifndef RAY_H
#define RAY_H

#include "vec3.h"

/// @brief A ray, defined as P(t) = origin + t * direction
class ray
{
  public:
	/// @brief Constructs a ray with zero origin and zero direction
	ray() = default;

	/// @brief Constructs a ray from an origin point and a direction vector
	/// @param origin The ray's starting point
	/// @param direction The ray's direction; not required to be unit length
	ray(const point3& origin, const vec3& direction)
		: orig{origin}, dir{direction}
	{
	}

	/// @brief Returns the ray's origin point
	const point3& origin() const { return orig; }

	/// @brief Returns the ray's direction vector
	const vec3& direction() const { return dir; }

	/// @brief Returns the point at parameter t along the ray
	/// @param t The ray parameter; t = 0 is the origin, increasing t moves
	///          along direction, negative t moves the opposite way
	point3 at(double t) const { return orig + t * dir; }

  private:
	point3 orig{};
	vec3 dir{};
};

#endif // RAY_H
