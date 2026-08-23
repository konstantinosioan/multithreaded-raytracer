#ifndef VEC3_H
#define VEC3_H

#include "rtweekend.h"

#include <array>
#include <cmath>
#include <cstddef>
#include <iostream>

/// @brief A 3D vector, used interchangeably for points, directions and colours
class Vec3
{
  public:
	std::array<double, 3> e{};

	/// @brief Constructs the zero vector
	Vec3() : e{0, 0, 0} {}

	/// @brief Constructs a vector from its three components
	Vec3(double e0, double e1, double e2) : e{e0, e1, e2} {}

	/// @brief Returns the x component
	double x() const { return e[0]; }

	/// @brief Returns the y component
	double y() const { return e[1]; }

	/// @brief Returns the z component
	double z() const { return e[2]; }

	/// @brief Returns the negation of this vector
	Vec3 operator-() const { return Vec3(-e[0], -e[1], -e[2]); }

	/// @brief Returns the component at index i
	/// @param i Index in [0, 2]
	/// @note No bounds checking; an out-of-range index is undefined behaviour
	double operator[](int i) const { return e[static_cast<std::size_t>(i)]; }

	/// @brief Returns a mutable reference to the component at index i, for
	///        in-place modification
	/// @param i Index in [0, 2]
	/// @note No bounds checking; an out-of-range index is undefined behaviour
	double& operator[](int i) { return e[static_cast<std::size_t>(i)]; }

	/// @brief Adds another vector to this one, component-wise
	/// @param v The vector to add
	/// @return A reference to this vector to allow chaining
	Vec3& operator+=(const Vec3& v)
	{
		e[0] += v.e[0];
		e[1] += v.e[1];
		e[2] += v.e[2];

		return *this;
	}

	/// @brief Scales this vector by a scalar
	/// @param t The scale factor
	/// @return A reference to this vector to allow chaining
	Vec3& operator*=(double t)
	{
		e[0] *= t;
		e[1] *= t;
		e[2] *= t;

		return *this;
	}

	/// @brief Divides this vector by a scalar
	/// @param t The divisor
	/// @return A reference to this vector to allow chaining
	Vec3& operator/=(double t) { return *this *= 1 / t; }

	/// @brief Returns the Euclidean length of this vector
	double length() const { return std::sqrt(length_squared()); }

	/// @brief Returns the squared length of this vector
	/// @note Avoids the sqrt() that length() requires; prefer this when only
	///       comparing magnitudes rather than needing the actual length
	double length_squared() const
	{
		return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
	}

	/// @brief Returns true if the vector is close to zero in all dimensions
	bool near_zero() const
	{
		constexpr double epsilon{1e-8};
		return (std::fabs(e[0]) < epsilon) && (std::fabs(e[1]) < epsilon) &&
			   (std::fabs(e[2]) < epsilon);
	}

	/// @brief Returns a random vector with each component in [0, 1)
	static Vec3 random()
	{
		return Vec3{random_double(), random_double(), random_double()};
	}

	/// @brief Returns a random vector with each component in [min, max)
	/// @param min The lower bound, inclusive
	/// @param max The upper bound, exclusive
	static Vec3 random(double min, double max)
	{
		return Vec3{random_double(min, max), random_double(min, max),
					random_double(min, max)};
	}
};

// the alias is useful for geometric clarity in the code
using point3 = Vec3;

// vector utility functions

/// @brief Writes a vector's components to a stream, space-separated
/// @param out The output stream
/// @param v The vector to write
/// @return The same output stream to allow chaining
inline std::ostream& operator<<(std::ostream& out, const Vec3& v)
{
	return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

/// @brief Adds two vectors component-wise
/// @param u The first vector
/// @param v The second vector
/// @return The sum u + v
inline Vec3 operator+(const Vec3& u, const Vec3& v)
{
	return Vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
}

/// @brief Subtracts two vectors component-wise
/// @param u The first vector to subtract from
/// @param v The second vector to be subtracted
/// @return The difference u - v
inline Vec3 operator-(const Vec3& u, const Vec3& v)
{
	return Vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}

/// @brief Multiplies two vectors component-wise
/// @param u The first vector
/// @param v The second vector
/// @return The product u * v
inline Vec3 operator*(const Vec3& u, const Vec3& v)
{
	return Vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}

/// @brief Scales a vector by a scalar
/// @param t The scale factor
/// @param v The vector to scale
/// @return The scaled vector
inline Vec3 operator*(double t, const Vec3& v)
{
	return Vec3(t * v.e[0], t * v.e[1], t * v.e[2]);
}

/// @brief Scales a vector by a scalar
/// @param v The vector to scale
/// @param t The scale factor
/// @return The scaled vector
inline Vec3 operator*(const Vec3& v, double t) { return t * v; }

/// @brief Divides a vector by a scalar
/// @param v The vector to divide
/// @param t The divisor
/// @return The scaled vector
inline Vec3 operator/(const Vec3& v, double t) { return (1 / t) * v; }

/// @brief Computes the dot product of two vectors
/// @param u The first vector
/// @param v The second vector
/// @return The scalar dot product u . v
inline double dot(const Vec3& u, const Vec3& v)
{
	return u.e[0] * v.e[0] + u.e[1] * v.e[1] + u.e[2] * v.e[2];
}

/// @brief Computes the cross product of two vectors
/// @param u The first vector
/// @param v The second vector
/// @return The vector u x v, perpendicular to both u and v
/// @note Not commutative
inline Vec3 cross(const Vec3& u, const Vec3& v)
{
	return Vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
				u.e[2] * v.e[0] - u.e[0] * v.e[2],
				u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}

/// @brief Returns a unit-length vector in the same direction as v
/// @param v The vector to normalise
/// @return v scaled to length 1
inline Vec3 unit_vector(const Vec3& v) { return v / v.length(); }

/// @brief Returns a random unit-length vector, uniformly distributed over
///        the surface of the unit sphere
inline Vec3 random_unit_vector()
{
	// Guards against a near-zero vector underflowing to length 0 and
	// normalizing to a bogus [+-inf, +-inf, +-inf] result.
	constexpr double min_vector_length_squared{1e-160};

	while (true)
	{
		Vec3 p{Vec3::random(-1, 1)};
		double lensq{p.length_squared()};

		if (min_vector_length_squared < lensq && lensq <= 1)
		{
			return p / std::sqrt(lensq);
		}
	}
}

/// @brief Returns a random unit-length vector on the same hemisphere as
///        the given normal
/// @param normal The surface normal defining which hemisphere is valid
inline Vec3 random_on_hemisphere(const Vec3& normal)
{
	Vec3 on_unit_sphere{random_unit_vector()};

	// In the same hemisphere as the normal
	if (dot(on_unit_sphere, normal) > 0.0)
	{
		return on_unit_sphere;
	}
	else
	{
		return -on_unit_sphere;
	}
}

/// @brief Mirrors a vector about a surface normal
/// @param v The incoming vector, pointing into the surface
/// @param n The surface normal; assumed to be unit length
inline Vec3 reflect(const Vec3& v, const Vec3& n)
{
	// Subtracting v's projection onto n twice cancels the component
	// along n and adds it back outward, mirroring v about the surface
	return v - 2 * dot(v, n) * n;
}

/// @brief Bends a vector as it crosses between two materials
/// @param uv The incoming vector; must be unit length
/// @param n The surface normal; assumed to be unit length
/// @param etai_over_etat The ratio of the two materials' refractive
///        indices, incoming over outgoing
/// @note Assumes refraction is possible; the caller checks for total
///       internal reflection first
inline Vec3 refract(const Vec3& uv, const Vec3& n, double etai_over_etat)
{
	// Snell's law, solved by splitting the refracted ray into components
	// perpendicular and parallel to the normal. uv points into the
	// surface, so negating it gives cos(theta) against the normal
	double cos_theta{std::fmin(dot(-uv, n), 1.0)};
	Vec3 r_out_perp{etai_over_etat * (uv + cos_theta * n)};
	Vec3 r_out_parallel{
		-std::sqrt(std::fabs(1.0 - r_out_perp.length_squared())) * n};

	return r_out_perp + r_out_parallel;
}

#endif // VEC3_H
