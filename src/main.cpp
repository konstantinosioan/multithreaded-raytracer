#include "camera.h"
#include "colour.h"
#include "hittable_list.h"
#include "material.h"
#include "rtweekend.h"
#include "sphere.h"
#include "timer.h"
#include "vec3.h"

#include <iostream>
#include <memory>
#include <utility>

/// @brief Renders the showcase scene: a field of randomly placed diffuse,
///        metal and glass spheres around three large ones, shot with a
///        defocused camera
int main()
{
	HittableList world{};

	auto ground_material{std::make_shared<Lambertian>(colour{0.5, 0.5, 0.5})};
	world.add(
		std::make_shared<Sphere>(point3{0, -1000, 0}, 1000, ground_material));

	for (int a{-11}; a < 11; ++a)
	{
		for (int b{-11}; b < 11; ++b)
		{
			double choose_mat{random_double()};
			point3 center{a + 0.9 * random_double(), 0.2,
						  b + 0.9 * random_double()};

			if ((center - point3{4, 0.2, 0}).length() > 0.9)
			{
				std::shared_ptr<Material> sphere_material{};

				if (choose_mat < 0.8)
				{
					// diffuse
					auto albedo{colour::random() * colour::random()};
					sphere_material = std::make_shared<Lambertian>(albedo);
				}
				else if (choose_mat < 0.95)
				{
					// metal
					auto albedo{colour::random(0.5, 1)};
					double fuzz{random_double(0, 0.5)};
					sphere_material = std::make_shared<Metal>(albedo, fuzz);
				}
				else
				{
					// glass
					sphere_material = std::make_shared<Dielectric>(1.5);
				}

				world.add(std::make_shared<Sphere>(center, 0.2,
												   std::move(sphere_material)));
			}
		}
	}

	auto material1{std::make_shared<Dielectric>(1.5)};
	world.add(std::make_shared<Sphere>(point3{0, 1, 0}, 1.0, material1));

	auto material2{std::make_shared<Lambertian>(colour{0.4, 0.2, 0.1})};
	world.add(std::make_shared<Sphere>(point3{-4, 1, 0}, 1.0, material2));

	auto material3{std::make_shared<Metal>(colour{0.7, 0.6, 0.5}, 0.0)};
	world.add(std::make_shared<Sphere>(point3{4, 1, 0}, 1.0, material3));

	Camera cam{};

	cam.aspect_ratio	  = 16.0 / 9.0;
	cam.image_width		  = 1200;
	cam.samples_per_pixel = 500;
	cam.max_depth		  = 50;
	cam.vfov			  = 20;
	cam.lookfrom		  = point3{13, 2, 3};
	cam.lookat			  = point3{0, 0, 0};
	cam.vup				  = Vec3{0, 1, 0};
	cam.defocus_angle	  = 0.6;
	cam.focus_dist		  = 10.0;

	Timer timer{};

	cam.render(world);

	std::clog << "Time elapsed: " << timer.elapsed() << " seconds\n";

	return 0;
}
