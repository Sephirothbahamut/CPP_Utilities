#pragma once

#include "common.h"
#include "../shape/ab.h"

namespace utils::math::geometry::shape::generic
	{
	template <storage::type storage_type, geometry::ends::optional_ab optional_ends>
	utils_gpu_available constexpr auto ab<storage_type, optional_ends>::bounding_box() const noexcept
		{
		const auto ret{geometry::shape::aabb::create::from_vertices(a, b)};
		return ret;
		}
	}