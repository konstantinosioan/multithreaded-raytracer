#ifndef CAMERA_H
#define CAMERA_H

#include "colour.h"
#include "hittable.h"
#include "interval.h"
#include "material.h"
#include "ray.h"
#include "rtweekend.h"
#include "vec3.h"

#include <iostream>

/// @brief Casts rays into a scene and writes out the resulting image
/// @note Set the public parameters directly, then call render(); the
///       camera works out the rest of its geometry itself
class Camera
{
  public:
	double aspect_ratio{1.0};  // Ratio of image width over height
	int image_width{100};	   // Rendered image width in pixel count
	int samples_per_pixel{10}; // Count of random samples for each pixel
	int max_depth{10};		   // Maximum number of ray bounces into scene

	/// @brief Renders the scene to stdout as a PPM image
	/// @param world The scene to render
	void render(const Hittable& world)
	{
		initialise();

		std::cout << "P3\n"
				  << image_width << ' ' << image_height << '\n'
				  << MAX_COLOUR_VALUE << '\n';

		// Each pixel is written out left to right, and each row of pixels is
		// written out top to bottom
		for (int j{0}; j < image_height; ++j)
		{
			// Progress indicator used to identify a run that's stalled out due
			// to an infinite loop or any other problem
			std::clog << "\rScanlines remaining: " << image_height - j << ' '
					  << std::flush;

			for (int i{0}; i < image_width; ++i)
			{
				colour pixel_colour{0, 0, 0};

				for (int sample{0}; sample < samples_per_pixel; ++sample)
				{
					Ray r{get_ray(i, j)};
					pixel_colour += ray_colour(r, max_depth, world);
				}

				write_colour(std::cout, pixel_samples_scale * pixel_colour);
			}
		}

		std::clog << "\rDone.                 \n";
	}

  private:
	int image_height{};	  // Rendered image height
	point3 center{};	  // Camera center
	point3 pixel00_loc{}; // Location of pixel 0, 0
	Vec3 pixel_delta_u{}; // Offset to pixel to the right
	Vec3 pixel_delta_v{}; // Offset to pixel below

	// Colour scale factor for a sum of pixel samples
	double pixel_samples_scale{};

	/// @brief Works out the viewport and pixel grid from the public
	///        parameters, ready for rendering
	void initialise()
	{
		// Calculate the image height and ensure it's at least 1
		image_height = static_cast<int>(image_width / aspect_ratio);
		image_height = (image_height < 1) ? 1 : image_height;

		pixel_samples_scale = 1.0 / samples_per_pixel;

		center = point3(0, 0, 0);

		// Determine viewport dimensions
		constexpr double focal_length{1.0};
		constexpr double viewport_height{2.0};
		double viewport_width{
			viewport_height *
			(static_cast<double>(image_width) / image_height)};

		// Calculate the vectors across the horizontal and down the vertical viewport edges
		Vec3 viewport_u{viewport_width, 0, 0};
		Vec3 viewport_v{0, -viewport_height, 0};

		// calculate the horizontal and vertical delta vectors from pixel to pixel
		pixel_delta_u = viewport_u / image_width;
		pixel_delta_v = viewport_v / image_height;

		// calculate the location of the upper left pixel
		point3 viewport_upper_left{center - Vec3(0, 0, focal_length) -
								   viewport_u / 2 - viewport_v / 2};
		pixel00_loc =
			viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
	}

	/// @brief Constructs a camera ray originating from the origin and
	///        directed at randomly sampled point around pixel location i, j
	/// @param i The pixel's column index
	/// @param j The pixel's row index
	/// @return A ray from the camera center toward the sampled point
	Ray get_ray(int i, int j) const
	{
		Vec3 offset{sample_square()};
		point3 pixel_sample{pixel00_loc + ((i + offset.x()) * pixel_delta_u) +
							((j + offset.y()) * pixel_delta_v)};
		point3 ray_origin{center};
		Vec3 ray_direction{pixel_sample - ray_origin};

		return Ray{ray_origin, ray_direction};
	}

	/// @brief Returns the vector to a random point in the
	///        [-.5, -.5]-[+.5, +.5] unit square
	Vec3 sample_square() const
	{
		return Vec3{random_double() - 0.5, random_double() - 0.5, 0};
	}

	/// @brief Computes the colour seen along a ray: the shaded surface colour
	///        if it hits something in world, otherwise the sky gradient
	/// @param r The ray to trace
	/// @param depth Remaining bounces; recursion stops at 0
	/// @param world The scene to test against
	/// @return The colour to write for this ray
	/// @note Capping depth darkens an over-deep ray instead of overflowing
	///       the stack
	colour ray_colour(const Ray& r, int depth, const Hittable& world) const
	{
		// If we've exceeded the ray bounce limit, no more light is gathered
		if (depth <= 0)
		{
			return colour{0, 0, 0};
		}

		HitRecord rec{};

		// Avoids shadow acne: a reflected ray can start just below the
		// surface due to rounding, re-hitting it at t near 0
		constexpr double shadow_acne_epsilon{0.001};

		if (world.hit(r, Interval(shadow_acne_epsilon, POSITIVE_INFINITY), rec))
		{
			Ray scattered{};
			colour attenuation{};

			if (rec.mat->scatter(r, rec, attenuation, scattered))
			{
				return attenuation * ray_colour(scattered, depth - 1, world);
			}

			return colour{0, 0, 0};
		}

		Vec3 unit_direction{unit_vector(r.direction())};
		double a{0.5 * (unit_direction.y() + 1.0)};

		return (1.0 - a) * colour(1.0, 1.0, 1.0) + a * colour(0.5, 0.7, 1.0);
	}
};

#endif // CAMERA_H
