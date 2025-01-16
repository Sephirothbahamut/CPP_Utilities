#pragma once

#include "common.h"
#include "../shape/point.h"

namespace utils::math
	{
	template<typename T, size_t SIZE>
	utils_gpu_available constexpr auto vec<T, SIZE>::bounding_box() const noexcept
		requires(std::convertible_to<value_type, float>&& extent == 2)
		{
		const auto ret{geometry::shape::aabb::create::from_possize(*this, utils::math::vec2f{0.f, 0.f})};
		return ret;
		}
	}