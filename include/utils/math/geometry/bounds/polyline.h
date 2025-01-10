#pragma once

#include "common.h"
#include "../shape/polyline.h"

namespace utils::math::geometry::shape::generic
	{
	template <storage::type storage_type, geometry::ends::closeable ends, size_t extent>
	utils_gpu_available constexpr auto polyline<storage_type, ends, extent>::bounding_box() const noexcept
		{
		const auto ret{shape::aabb::create::from_vertices(vertices)};
		return ret;
		}
	}