#ifndef CAMERA_H
#define CAMERA_H

#include "colour.h"
#include "hittable.h"
#include "interval.h"
#include "material.h"
#include "ray.h"
#include "rtweekend.h"
#include "tile.h"
#include "vec3.h"

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <thread>
#include <vector>

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
	double vfov{90};		   // Vertical view angle (field of view)
	point3 lookfrom{0, 0, 0};  // Point camera is looking from
	point3 lookat{0, 0, -1};   // Point camera is looking at
	Vec3 vup{0, 1, 0};		   // Camera-relative "up" direction
	double defocus_angle{0};   // Variation angle of rays through each pixel
	int tile_size{32}; // Width and height of each tile of work, in pixels

	// Number of worker threads to render with
	int thread_count{static_cast<int>(std::thread::hardware_concurrency())};

	// Distance from camera lookfrom point to plane of perfect focus
	double focus_dist{10};

	/// @brief Renders the scene to stdout as a PPM image
	/// @param world The scene to render
	/// @note Workers claim tiles from a shared atomic counter, so the
	///       output is identical however many threads are used
	void render(const Hittable& world)
	{
		initialise();

		const int tile_count{static_cast<int>(std::size(tiles))};
		std::atomic<int> next_tile{0};
		std::vector<std::thread> workers{};

		std::clog << "Rendering with " << thread_count << " threads\n"
				  << std::flush;

		for (int worker{0}; worker < thread_count; ++worker)
		{
			workers.emplace_back(
				[this, &next_tile, &world, tile_count]()
				{
					while (true)
					{
						// fetch_add returns the value before incrementing,
						// so no two workers ever claim the same tile
						int tile_index{next_tile.fetch_add(1)};

						if (tile_index >= tile_count)
						{
							break;
						}

						render_tile(tile_index, world);
					}
				});
		}

		for (auto& worker : workers)
		{
			worker.join();
		}

		// Write the framebuffer out as PPM
		std::cout << "P3\n"
				  << image_width << ' ' << image_height << '\n'
				  << MAX_COLOUR_VALUE << '\n';

		// Each pixel is written out left to right, and each row of pixels is
		// written out top to bottom
		for (const colour& pixel : framebuffer)
		{
			write_colour(std::cout, pixel);
		}

		std::clog << "Done.\n";
	}

  private:
	int image_height{};				   // Rendered image height
	point3 center{};				   // Camera center
	point3 pixel00_loc{};			   // Location of pixel 0, 0
	Vec3 pixel_delta_u{};			   // Offset to pixel to the right
	Vec3 pixel_delta_v{};			   // Offset to pixel below
	std::vector<Tile> tiles{};		   // Tiles the image is divided into
	std::vector<colour> framebuffer{}; // Rendered pixels in row-major order

	// Camera frame basis vectors
	Vec3 u{};
	Vec3 v{};
	Vec3 w{};

	Vec3 defocus_disk_u{}; // Defocus disk horizontal radius
	Vec3 defocus_disk_v{}; // Defocus disk vertical radius

	// Colour scale factor for a sum of pixel samples
	double pixel_samples_scale{};

	/// @brief Works out the viewport, pixel grid and defocus disk from the
	///        public parameters, then divides the image into tiles and
	///        allocates the framebuffer, ready for rendering
	void initialise()
	{
		// Calculate the image height and ensure it's at least 1
		image_height = static_cast<int>(image_width / aspect_ratio);
		image_height = (image_height < 1) ? 1 : image_height;

		pixel_samples_scale = 1.0 / samples_per_pixel;

		center = lookfrom;

		// Determine viewport dimensions
		// Half the viewport height subtends half the fov at focus_dist
		double theta{degrees_to_radians(vfov)};
		double h{std::tan(theta / 2)};
		double viewport_height{2 * h * focus_dist};
		double viewport_width{
			viewport_height *
			(static_cast<double>(image_width) / image_height)};

		// Calculate the u, v, w unit basis vectors for the camera coordinate frame
		w = unit_vector(lookfrom - lookat);
		u = unit_vector(cross(vup, w));
		v = cross(w, u);

		// Calculate the vectors across the horizontal and down the vertical viewport edges
		// Vector across viewport horizontal edge
		Vec3 viewport_u{viewport_width * u};
		// Vector down viewport vertical edge
		Vec3 viewport_v{viewport_height * -v};

		// calculate the horizontal and vertical delta vectors from pixel to pixel
		pixel_delta_u = viewport_u / image_width;
		pixel_delta_v = viewport_v / image_height;

		// calculate the location of the upper left pixel
		point3 viewport_upper_left{center - focus_dist * w - viewport_u / 2 -
								   viewport_v / 2};
		pixel00_loc =
			viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

		// calculate the camera defocus disk basis vectors
		double defocus_radius{focus_dist *
							  std::tan(degrees_to_radians(defocus_angle / 2))};
		defocus_disk_u = u * defocus_radius;
		defocus_disk_v = v * defocus_radius;

		// divide the image into tiles of work
		// clear first since initialise() runs on every render() call
		tiles.clear();

		// A tile size below 1 would loop forever
		tile_size = (tile_size < 1) ? 1 : tile_size;

		// hardware_concurrency() reports 0 when it cannot detect the count
		thread_count = (thread_count < 1) ? 1 : thread_count;

		for (int y{0}; y < image_height; y += tile_size)
		{
			for (int x{0}; x < image_width; x += tile_size)
			{
				Tile tile{};

				tile.x_start = x;
				tile.x_end	 = std::min(x + tile_size, image_width);
				tile.y_start = y;
				tile.y_end	 = std::min(y + tile_size, image_height);

				tiles.push_back(tile);
			}
		}

		// allocate the output buffer, one colour per pixel
		framebuffer.assign(static_cast<std::size_t>(image_height) *
							   static_cast<std::size_t>(image_width),
						   colour{});
	}

	/// @brief Renders one tile's pixels into the framebuffer
	/// @param index Which tile to render, as a position in the tile list
	/// @param world The scene to test rays against
	/// @note Tiles never overlap, so no two calls write the same pixel
	void render_tile(int index, const Hittable& world)
	{
		const Tile& tile{tiles[static_cast<std::size_t>(index)]};

		// Seed before any sampling, so the tile renders the same either way
		seed_generator(index);

		for (int y{tile.y_start}; y < tile.y_end; ++y)
		{
			for (int x{tile.x_start}; x < tile.x_end; ++x)
			{
				colour pixel_colour{0, 0, 0};

				for (int sample{0}; sample < samples_per_pixel; ++sample)
				{
					Ray r{get_ray(x, y)};
					pixel_colour += ray_colour(r, max_depth, world);
				}

				framebuffer[static_cast<std::size_t>(y) *
								static_cast<std::size_t>(image_width) +
							static_cast<std::size_t>(x)] =
					pixel_samples_scale * pixel_colour;
			}
		}
	}

	/// @brief Constructs a camera ray directed at a randomly sampled point
	///        around pixel location i, j
	/// @param i The pixel's column index
	/// @param j The pixel's row index
	/// @return A ray toward the sampled point, starting from the defocus
	///         disk if defocus is enabled and the camera centre otherwise
	[[nodiscard]] Ray get_ray(int i, int j) const
	{
		Vec3 offset{sample_square()};
		point3 pixel_sample{pixel00_loc + ((i + offset.x()) * pixel_delta_u) +
							((j + offset.y()) * pixel_delta_v)};
		point3 ray_origin{(defocus_angle <= 0) ? center
											   : defocus_disk_sample()};
		Vec3 ray_direction{pixel_sample - ray_origin};

		return Ray{ray_origin, ray_direction};
	}

	/// @brief Returns the vector to a random point in the
	///        [-.5, -.5]-[+.5, +.5] unit square
	[[nodiscard]] static Vec3 sample_square()
	{
		return Vec3{random_double() - 0.5, random_double() - 0.5, 0};
	}

	/// @brief Returns a random point in the camera defocus disk
	[[nodiscard]] point3 defocus_disk_sample() const
	{
		point3 p{random_in_unit_disk()};
		return center + p[0] * defocus_disk_u + p[1] * defocus_disk_v;
	}

	/// @brief Computes the colour seen along a ray: the shaded surface colour
	///        if it hits something in world, otherwise the sky gradient
	/// @param r The ray to trace
	/// @param depth Remaining bounces; recursion stops at 0
	/// @param world The scene to test against
	/// @return The colour seen along this one ray, before averaging
	/// @note Capping depth darkens an over-deep ray instead of overflowing
	///       the stack
	[[nodiscard]] colour ray_colour(const Ray& r, int depth,
									const Hittable& world) const
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
