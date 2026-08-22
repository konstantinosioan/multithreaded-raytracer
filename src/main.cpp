#include "colour.h"
#include "hittable.h"
#include "hittable_list.h"
#include "ray.h"
#include "rtweekend.h"
#include "sphere.h"
#include "vec3.h"

#include <cmath>
#include <iostream>
#include <memory>

/// @brief Computes the colour seen along a ray: the shaded surface colour
///        if it hits something in world, otherwise the sky gradient
/// @param r The ray to trace
/// @param world The scene to test against
/// @return The colour to write for this ray
colour ray_colour(const ray& r, const hittable& world)
{
	hit_record rec{};

	if (world.hit(r, 0, POSITIVE_INFINITY, rec))
	{
		return 0.5 * (rec.normal + colour(1, 1, 1));
	}

	vec3 unit_direction{unit_vector(r.direction())};
	double a{0.5 * (unit_direction.y() + 1.0)};

	return (1.0 - a) * colour(1.0, 1.0, 1.0) + a * colour(0.5, 0.7, 1.0);
}

/// @brief Renders a hardcoded scene of two spheres shaded by their surface
///        normals against a blue-to-white sky gradient, using a simple
///        pinhole camera
int main()
{
	constexpr double aspect_ratio{16.0 / 9.0};
	constexpr int image_width{400};

	// calculate the image height and ensure it's at least 1
	int image_height{static_cast<int>(image_width / aspect_ratio)};
	image_height = (image_height < 1) ? 1 : image_height;

	// world
	hittable_list world{};
	world.add(std::make_shared<sphere>(point3(0, 0, -1), 0.5));
	world.add(std::make_shared<sphere>(point3(0, -100.5, -1), 100));

	// camera
	constexpr double focal_length{1.0};
	constexpr double viewport_height{2.0};
	double viewport_width{viewport_height *
						  (static_cast<double>(image_width) / image_height)};
	point3 camera_center{0, 0, 0};

	// calculate the vectors across the horizontal and down the vertical viewport edges
	vec3 viewport_u{viewport_width, 0, 0};
	vec3 viewport_v{0, -viewport_height, 0};

	// calculate the horizontal and vertical delta vectors from pixel to pixel
	vec3 pixel_delta_u{viewport_u / image_width};
	vec3 pixel_delta_v{viewport_v / image_height};

	// calculate the location of the upper left pixel
	point3 viewport_upper_left{camera_center - vec3(0, 0, focal_length) -
							   viewport_u / 2 - viewport_v / 2};
	point3 pixel00_loc{viewport_upper_left +
					   0.5 * (pixel_delta_u + pixel_delta_v)};

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
			point3 pixel_center{pixel00_loc + i * pixel_delta_u +
								j * pixel_delta_v};
			vec3 ray_direction{pixel_center - camera_center};
			ray r{camera_center, ray_direction};
			colour pixel_colour{ray_colour(r, world)};
			write_colour(std::cout, pixel_colour);
		}
	}

	std::clog << "\rDone.                 \n";
}
