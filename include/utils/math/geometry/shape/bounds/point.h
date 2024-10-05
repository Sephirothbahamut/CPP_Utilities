#pragma once

#include "common.h"
#include "../point.h"

namespace utils::math::details
	{
	template<typename T, template <typename, size_t> class unspecialized_derived_T>
	utils_gpu_available constexpr auto vec_sized_specialization<T, 2, unspecialized_derived_T>::bounding_box() const noexcept
		{
		const auto ret{geometry::shape::aabb::create::from_possize(*this, utils::math::vec2f{0.f, 0.f})};
		return ret;
		}
	}