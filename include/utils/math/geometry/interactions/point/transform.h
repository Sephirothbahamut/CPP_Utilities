#pragma once

#include "base.h"

namespace utils::math::geometry::interactions
	{
	utils_gpu_available constexpr auto& scale_self    (shape::concepts::point auto& point, const float                      & scaling    ) noexcept { return point *= scaling    ; }
	utils_gpu_available constexpr auto& rotate_self   (shape::concepts::point auto& point, const angle::concepts::angle auto& rotation   ) noexcept { return point += rotation   ; }
	utils_gpu_available constexpr auto& translate_self(shape::concepts::point auto& point, const vec2f                      & translation) noexcept { return point += translation; }
	utils_gpu_available constexpr auto& transform_self(shape::concepts::point auto& point, const utils::math::transform2    & transform) noexcept
		{
		return translate_self(rotate_self(scale_self(point, transform.scaling), transform.rotation), transform.translation);
		}
	}