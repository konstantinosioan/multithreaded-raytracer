#ifndef VEC3_H
#define VEC3_H

#include <array>
#include <cmath>
#include <cstddef>
#include <iostream>

/// @brief A 3D vector, used interchangeably for points, directions and colours
class vec3
{
  public:
	std::array<double, 3> e{};

	/// @brief Constructs the zero vector
	vec3() : e{0, 0, 0} {}

	/// @brief Constructs a vector from its three components
	vec3(double e0, double e1, double e2) : e{e0, e1, e2} {}

	/// @brief Returns the x component
	double x() const { return e[0]; }

	/// @brief Returns the y component
	double y() const { return e[1]; }

	/// @brief Returns the z component
	double z() const { return e[2]; }

	/// @brief Returns the negation of this vector
	vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }

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
	vec3& operator+=(const vec3& v)
	{
		e[0] += v.e[0];
		e[1] += v.e[1];
		e[2] += v.e[2];

		return *this;
	}

	/// @brief Scales this vector by a scalar
	/// @param t The scale factor
	/// @return A reference to this vector to allow chaining
	vec3& operator*=(double t)
	{
		e[0] *= t;
		e[1] *= t;
		e[2] *= t;

		return *this;
	}

	/// @brief Divides this vector by a scalar
	/// @param t The divisor
	/// @return A reference to this vector to allow chaining
	vec3& operator/=(double t) { return *this *= 1 / t; }

	/// @brief Returns the Euclidean length of this vector
	double length() const { return std::sqrt(length_squared()); }

	/// @brief Returns the squared length of this vector
	/// @note Avoids the sqrt() that length() requires; prefer this when only
	///       comparing magnitudes rather than needing the actual length
	double length_squared() const
	{
		return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
	}
};

// the alias is useful for geometric clarity in the code
using point3 = vec3;

// vector utility functions

/// @brief Writes a vector's components to a stream, space-separated
/// @param out The output stream
/// @param v The vector to write
/// @return The same output stream to allow chaining
inline std::ostream& operator<<(std::ostream& out, const vec3& v)
{
	return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

/// @brief Adds two vectors component-wise
/// @param u The first vector
/// @param v The second vector
/// @return The sum u + v
inline vec3 operator+(const vec3& u, const vec3& v)
{
	return vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
}

/// @brief Subtracts two vectors component-wise
/// @param u The first vector to subtract from
/// @param v The second vector to be subtracted
/// @return The difference u - v
inline vec3 operator-(const vec3& u, const vec3& v)
{
	return vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}

/// @brief Multiplies two vectors component-wise
/// @param u The first vector
/// @param v The second vector
/// @return The product u * v
inline vec3 operator*(const vec3& u, const vec3& v)
{
	return vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}

/// @brief Scales a vector by a scalar
/// @param t The scale factor
/// @param v The vector to scale
/// @return The scaled vector
inline vec3 operator*(double t, const vec3& v)
{
	return vec3(t * v.e[0], t * v.e[1], t * v.e[2]);
}

/// @brief Scales a vector by a scalar
/// @param v The vector to scale
/// @param t The scale factor
/// @return The scaled vector
inline vec3 operator*(const vec3& v, double t) { return t * v; }

/// @brief Divides a vector by a scalar
/// @param v The vector to divide
/// @param t The divisor
/// @return The scaled vector
inline vec3 operator/(const vec3& v, double t) { return (1 / t) * v; }

/// @brief Computes the dot product of two vectors
/// @param u The first vector
/// @param v The second vector
/// @return The scalar dot product u . v
inline double dot(const vec3& u, const vec3& v)
{
	return u.e[0] * v.e[0] + u.e[1] * v.e[1] + u.e[2] * v.e[2];
}

/// @brief Computes the cross product of two vectors
/// @param u The first vector
/// @param v The second vector
/// @return The vector u x v, perpendicular to both u and v
/// @note Not commutative
inline vec3 cross(const vec3& u, const vec3& v)
{
	return vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
				u.e[2] * v.e[0] - u.e[0] * v.e[2],
				u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}

/// @brief Returns a unit-length vector in the same direction as v
/// @param v The vector to normalise
/// @return v scaled to length 1
inline vec3 unit_vector(const vec3& v) { return v / v.length(); }

#endif // VEC3_H
