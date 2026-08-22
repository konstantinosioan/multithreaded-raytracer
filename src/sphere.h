#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "vec3.h"

#include <cmath>

/// @brief A sphere, hittable by a ray
class sphere final : public hittable
{
  public:
	/// @brief Constructs a sphere from a center and radius
	/// @param center The sphere's center
	/// @param radius The sphere's radius; negative values are clamped to zero
	sphere(const point3& center, double radius)
		: center{center}, radius{std::fmax(0, radius)}
	{
	}

	/// @brief Tests whether a ray intersects this sphere within a t-range
	/// @param r The ray to test
	/// @param ray_tmin The lower bound of the acceptable hit range, exclusive
	/// @param ray_tmax The upper bound of the acceptable hit range, exclusive
	/// @param rec Output parameter, only filled in if a hit occurs
	/// @return True if the ray hits this sphere within [ray_tmin, ray_tmax]
	bool hit(const ray& r, double ray_tmin, double ray_tmax,
			 hit_record& rec) const override
	{
		vec3 oc{center - r.origin()};
		double a{r.direction().length_squared()};
		double h{dot(r.direction(), oc)};
		double c{oc.length_squared() - radius * radius};
		double discriminant{h * h - a * c};

		if (discriminant < 0)
		{
			return false;
		}

		double sqrtd{std::sqrt(discriminant)};

		// find the nearest root that lies in the acceptable range
		double root{(h - sqrtd) / a};

		if (root <= ray_tmin || root >= ray_tmax)
		{
			root = (h + sqrtd) / a;

			if (root <= ray_tmin || root >= ray_tmax)
			{
				return false;
			}
		}

		rec.t = root;
		rec.p = r.at(rec.t);
		vec3 outward_normal{(rec.p - center) / radius};
		rec.set_face_normal(r, outward_normal);

		return true;
	}

  private:
	point3 center{};
	double radius{};
};

#endif // SPHERE_H
