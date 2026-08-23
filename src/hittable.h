#ifndef HITTABLE_H
#define HITTABLE_H

#include "interval.h"
#include "ray.h"
#include "vec3.h"

#include <memory>

class Material;

/// @brief Bundles the result of a successful ray-object intersection: the
///        hit point, surface normal, material, ray parameter and which
///        face was hit
class HitRecord
{
  public:
	point3 p{};
	Vec3 normal{};
	std::shared_ptr<Material> mat{};
	double t{};
	bool front_face{};

	/// @brief Sets the hit record's normal vector and front-face flag
	/// @param r The ray that produced this hit
	/// @param outward_normal The surface normal, pointing away from the
	///        object's interior
	/// @note outward_normal is assumed to have unit length
	void set_face_normal(const Ray& r, const Vec3& outward_normal)
	{
		front_face = dot(r.direction(), outward_normal) < 0;
		normal	   = front_face ? outward_normal : -outward_normal;
	}
};

/// @brief Base class for anything a ray can intersect
class Hittable
{
  public:
	/// @brief Virtual destructor, required since hittable is used
	///        polymorphically through base-class pointers
	virtual ~Hittable() = default;

	/// @brief Tests whether a ray intersects this object within a t-range
	/// @param r The ray to test
	/// @param ray_t The acceptable hit range
	/// @param rec Output parameter, only filled in if a hit occurs
	/// @return True if the ray hits this object within ray_t
	virtual bool hit(const Ray& r, Interval ray_t, HitRecord& rec) const = 0;
};

#endif // HITTABLE_H
