#ifndef HITTABLE_H
#define HITTABLE_H

#include "ray.h"
#include "vec3.h"

/// @brief Bundles the result of a successful ray-object intersection:
///        the hit point, surface normal, ray parameter and which face was hit
class hit_record
{
  public:
	point3 p{};
	vec3 normal{};
	double t{};
	bool front_face{};

	/// @brief Sets the hit record's normal vector and front-face flag
	/// @param r The ray that produced this hit
	/// @param outward_normal The surface normal, pointing away from the
	///        object's interior
	/// @note outward_normal is assumed to have unit length
	void set_face_normal(const ray& r, const vec3& outward_normal)
	{
		front_face = dot(r.direction(), outward_normal) < 0;
		normal	   = front_face ? outward_normal : -outward_normal;
	}
};

/// @brief Base class for anything a ray can intersect
class hittable
{
  public:
	/// @brief Virtual destructor, required since hittable is used
	///        polymorphically through base-class pointers
	virtual ~hittable() = default;

	/// @brief Tests whether a ray intersects this object within a t-range
	/// @param r The ray to test
	/// @param ray_tmin The lower bound of the acceptable hit range, exclusive
	/// @param ray_tmax The upper bound of the acceptable hit range, exclusive
	/// @param rec Output parameter, only filled in if a hit occurs
	/// @return True if the ray hits this object within [ray_tmin, ray_tmax]
	virtual bool hit(const ray& r, double ray_tmin, double ray_tmax,
					 hit_record& rec) const = 0;
};

#endif // HITTABLE_H
