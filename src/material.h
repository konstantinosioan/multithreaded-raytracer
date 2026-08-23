#ifndef MATERIAL_H
#define MATERIAL_H

#include "colour.h"
#include "hittable.h"
#include "ray.h"
#include "rtweekend.h"
#include "vec3.h"

#include <cmath>

/// @brief Base class for anything that decides how a ray bounces off a
///        surface
class Material
{
  public:
	/// @brief Virtual destructor, required since materials are used
	///        polymorphically through base-class pointers
	virtual ~Material() = default;

	/// @brief Scatters an incoming ray off a surface
	/// @param r_in The ray that hit the surface
	/// @param rec Details of where and how the ray hit
	/// @param attenuation Output parameter, set to how much colour the
	///        surface keeps from the bounce
	/// @param scattered Output parameter, set to the bounced ray
	/// @return True if the ray scattered, false if it was absorbed
	/// @note This default absorbs everything; derived materials override it
	virtual bool scatter([[maybe_unused]] const Ray& r_in,
						 [[maybe_unused]] const HitRecord& rec,
						 [[maybe_unused]] colour& attenuation,
						 [[maybe_unused]] Ray& scattered) const
	{
		return false;
	}
};

/// @brief A matte surface that scatters light in every direction
class Lambertian final : public Material
{
  public:
	/// @brief Constructs a diffuse material of the given colour
	/// @param albedo The fraction of each colour channel the surface
	///        reflects rather than absorbs
	explicit Lambertian(const colour& albedo) : albedo{albedo} {}

	/// @brief Scatters the ray in a random direction biased toward the
	///        surface normal
	/// @param r_in Unused; diffuse scattering ignores the incoming
	///        direction
	/// @param rec Details of where and how the ray hit
	/// @param attenuation Output parameter, set to this surface's albedo
	/// @param scattered Output parameter, set to the bounced ray
	/// @return Always true; this material never absorbs a ray outright
	bool scatter([[maybe_unused]] const Ray& r_in, const HitRecord& rec,
				 colour& attenuation, Ray& scattered) const override
	{
		// Offsetting the normal by a random unit vector samples a sphere
		// tangent to the surface, which weights directions by cos(angle
		// from normal) - true Lambertian reflection
		Vec3 scatter_direction{rec.normal + random_unit_vector()};

		// Catch degenerate scatter direction
		if (scatter_direction.near_zero())
		{
			scatter_direction = rec.normal;
		}

		scattered	= Ray{rec.p, scatter_direction};
		attenuation = albedo;

		return true;
	}

  private:
	colour albedo{};
};

/// @brief A reflective surface, from a polished mirror to a brushed finish
class Metal final : public Material
{
  public:
	/// @brief Constructs a metal material of the given colour and finish
	/// @param albedo The fraction of each colour channel the surface
	///        reflects rather than absorbs
	/// @param fuzz How much to randomly perturb each reflection; 0 is a
	///        perfect mirror, and anything above 1 is capped
	Metal(const colour& albedo, double fuzz)
		: albedo{albedo}, fuzz{fuzz < 1 ? fuzz : 1}
	{
	}

	/// @brief Reflects the ray about the surface normal, perturbed by fuzz
	/// @param r_in The ray that hit the surface
	/// @param rec Details of where and how the ray hit
	/// @param attenuation Output parameter, set to this surface's albedo
	/// @param scattered Output parameter, set to the reflected ray
	/// @return False if fuzz sent the reflection below the surface, in
	///         which case the surface absorbs it
	bool scatter(const Ray& r_in, const HitRecord& rec, colour& attenuation,
				 Ray& scattered) const override
	{
		Vec3 reflected{reflect(r_in.direction(), rec.normal)};
		reflected	= unit_vector(reflected) + fuzz * random_unit_vector();
		scattered	= Ray{rec.p, reflected};
		attenuation = albedo;

		return (dot(scattered.direction(), rec.normal) > 0);
	}

  private:
	colour albedo{};

	// Capped at 1; beyond that it swamps the unit-length reflection
	double fuzz{};
};

/// @brief A clear surface such as glass or water, which both bends and
///        reflects the light passing through it
class Dielectric final : public Material
{
  public:
	/// @brief Constructs a clear material with the given refractive index
	/// @param refraction_index How strongly the material bends light,
	///        relative to whatever surrounds it
	explicit Dielectric(double refraction_index)
		: refraction_index{refraction_index}
	{
	}

	/// @brief Either refracts the ray through the surface or reflects it
	///        off it, chosen per bounce
	/// @param r_in The ray that hit the surface
	/// @param rec Details of where and how the ray hit
	/// @param attenuation Output parameter, always white since clear
	///        materials absorb nothing
	/// @param scattered Output parameter, set to the refracted or
	///        reflected ray
	/// @return Always true; the ray always continues one way or the other
	bool scatter(const Ray& r_in, const HitRecord& rec, colour& attenuation,
				 Ray& scattered) const override
	{
		attenuation = colour{1.0, 1.0, 1.0};
		double ri{rec.front_face ? (1.0 / refraction_index) : refraction_index};

		Vec3 unit_direction{unit_vector(r_in.direction())};
		double cos_theta{std::fmin(dot(-unit_direction, rec.normal), 1.0)};
		double sin_theta{std::sqrt(1.0 - cos_theta * cos_theta)};

		// Snell's law has no solution when sin(theta') would exceed 1, so
		// the surface must reflect instead - total internal reflection
		bool cannot_refract{ri * sin_theta > 1.0};
		Vec3 direction{};

		if (cannot_refract || reflectance(cos_theta, ri) > random_double())
		{
			direction = reflect(unit_direction, rec.normal);
		}
		else
		{
			direction = refract(unit_direction, rec.normal, ri);
		}

		scattered = Ray{rec.p, direction};
		return true;
	}

  private:
	// Refractive index in vacuum or air, or the ratio of the material's
	// refractive index over that of the enclosing media
	double refraction_index{};

	/// @brief Returns how likely the surface is to reflect rather than
	///        refract at the given angle
	/// @param cosine The cosine of the angle between the ray and the normal
	/// @param refraction_index The material's refractive index
	/// @note Real glass turns mirror-like at glancing angles, which is
	///       what this captures
	static double reflectance(double cosine, double refraction_index)
	{
		// Schlick's polynomial approximation to the Fresnel equations,
		// which are far more expensive and no more convincing here
		double r0{(1 - refraction_index) / (1 + refraction_index)};
		r0 = r0 * r0;
		return r0 + (1 - r0) * std::pow(1 - cosine, 5);
	}
};

#endif // MATERIAL_H
