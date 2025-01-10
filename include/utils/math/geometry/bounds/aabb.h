#pragma once

#include "common.h"
#include "../shape/aabb.h"

namespace utils::math
	{
	template<typename T>
	utils_gpu_available constexpr auto rect<T>::bounding_box() const noexcept
		{
		const auto ret{geometry::shape::cast_storage<storage::type::create::owner()>(*this)};
		return ret;
		}
	}