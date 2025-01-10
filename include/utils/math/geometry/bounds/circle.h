#pragma once

#include "common.h"
#include "../shape/circle.h"

namespace utils::math::geometry::shape::generic
	{
	template <storage::type storage_type>
	utils_gpu_available constexpr auto circle<storage_type>::bounding_box() const noexcept
		{
		const auto ret{geometry::shape::aabb::create::from_ul_dr(centre - radius, centre + radius)};
		return ret;
		}
	}