#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include "hittable.h"

#include <memory>
#include <utility>
#include <vector>

/// @brief A collection of hittable objects, itself hittable -
///        returns the closest intersection among everything it contains
class hittable_list final : public hittable
{
  public:
	std::vector<std::shared_ptr<hittable>> objects{};

	/// @brief Constructs an empty list
	hittable_list() = default;

	/// @brief Constructs a list containing a single object
	/// @param object The object to add
	explicit hittable_list(std::shared_ptr<hittable> object)
	{
		add(std::move(object));
	}

	/// @brief Removes all objects from the list
	void clear() { objects.clear(); }

	/// @brief Adds an object to the list
	/// @param object The object to add; ownership is shared with the caller
	void add(std::shared_ptr<hittable> object)
	{
		objects.push_back(std::move(object));
	}

	/// @brief Finds the closest intersection among every object in the list
	/// @param r The ray to test
	/// @param ray_tmin The lower bound of the acceptable hit range, exclusive
	/// @param ray_tmax The upper bound of the acceptable hit range, exclusive
	/// @param rec Output parameter, filled in with the closest hit if any exists
	/// @return True if the ray hits at least one object in the list
	bool hit(const ray& r, double ray_tmin, double ray_tmax,
			 hit_record& rec) const override
	{
		hit_record temp_rec{};
		bool hit_anything{false};
		double closest_so_far{ray_tmax};

		for (const auto& object : objects)
		{
			if (object->hit(r, ray_tmin, closest_so_far, temp_rec))
			{
				hit_anything   = true;
				closest_so_far = temp_rec.t;
				rec			   = temp_rec;
			}
		}

		return hit_anything;
	}
};

#endif // HITTABLE_LIST_H
