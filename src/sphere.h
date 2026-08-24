#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "interval.h"
#include "material.h"
#include "vec3.h"

#include <cmath>
#include <memory>
#include <utility>

/// @brief A sphere, hittable by a ray
class Sphere final : public Hittable
{
  public:
	/// @brief Constructs a sphere from a center, radius and material
	/// @param center The sphere's center
	/// @param radius The sphere's radius; negative values are clamped to zero
	/// @param mat The sphere's material, shared with the caller unless
	///        moved in
	Sphere(const point3& center, double radius, std::shared_ptr<Material> mat)
		: center{center}, radius{std::fmax(0, radius)}, mat{std::move(mat)}
	{
	}

	/// @brief Tests whether a ray intersects this sphere within a t-range
	/// @param r The ray to test
	/// @param ray_t The acceptable hit range
	/// @param rec Output parameter, only filled in if a hit occurs
	/// @return True if the ray hits this sphere within ray_t
	bool hit(const Ray& r, Interval ray_t, HitRecord& rec) const override
	{
		Vec3 oc{center - r.origin()};
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

		if (!ray_t.surrounds(root))
		{
			root = (h + sqrtd) / a;

			if (!ray_t.surrounds(root))
			{
				return false;
			}
		}

		rec.t = root;
		rec.p = r.at(rec.t);
		Vec3 outward_normal{(rec.p - center) / radius};
		rec.set_face_normal(r, outward_normal);
		rec.mat = mat.get();

		return true;
	}

  private:
	point3 center{};
	double radius{};
	std::shared_ptr<Material> mat{};
};

#endif // SPHERE_H
