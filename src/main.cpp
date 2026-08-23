#include "camera.h"
#include "colour.h"
#include "hittable_list.h"
#include "material.h"
#include "sphere.h"
#include "vec3.h"

#include <memory>

/// @brief Renders a hardcoded scene of two diffuse spheres, one metal and
///        one hollow glass against a blue-to-white sky gradient, using a
///        simple pinhole camera with multi-sample antialiasing
int main()
{
	HittableList world{};

	auto material_ground{std::make_shared<Lambertian>(colour{0.8, 0.8, 0.0})};
	auto material_center{std::make_shared<Lambertian>(colour{0.1, 0.2, 0.5})};
	auto material_left{std::make_shared<Dielectric>(1.50)};
	auto material_bubble{std::make_shared<Dielectric>(1.00 / 1.50)};
	auto material_right{std::make_shared<Metal>(colour{0.8, 0.6, 0.2}, 0.0)};

	world.add(std::make_shared<Sphere>(point3{0.0, -100.5, -1.0}, 100.0,
									   material_ground));
	world.add(
		std::make_shared<Sphere>(point3{0.0, 0.0, -1.2}, 0.5, material_center));
	world.add(
		std::make_shared<Sphere>(point3{-1.0, 0.0, -1.0}, 0.5, material_left));
	world.add(std::make_shared<Sphere>(point3{-1.0, 0.0, -1.0}, 0.4,
									   material_bubble));
	world.add(
		std::make_shared<Sphere>(point3{1.0, 0.0, -1.0}, 0.5, material_right));

	Camera cam{};

	cam.aspect_ratio	  = 16.0 / 9.0;
	cam.image_width		  = 400;
	cam.samples_per_pixel = 100;
	cam.max_depth		  = 50;

	cam.render(world);
}
