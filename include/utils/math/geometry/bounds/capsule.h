#pragma once

#include "common.h"
#include "../shape/capsule.h"
#include "ab.h"

namespace utils::math::geometry::shape::generic
	{
	template <storage::type storage_type>
	utils_gpu_available constexpr auto capsule<storage_type>::bounding_box() const noexcept
		{
		shape::aabb tmp{ab.bounding_box()};
		tmp.ll() -= radius;
		tmp.up() -= radius;
		tmp.rr() += radius;
		tmp.dw() += radius;
		return tmp;
		}
	}