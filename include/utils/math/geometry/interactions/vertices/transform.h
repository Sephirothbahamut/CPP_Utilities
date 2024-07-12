#pragma once

#include "base.h"
#include "../point.h"

namespace utils::math::geometry::interactions
	{
	utils_gpu_available constexpr typename auto& scale_self    (shape::concepts::has_vertices auto& shape, const float                      & scaling    ) noexcept { for(auto& vertex : shape.vertices) { scale_self    (vertex, scaling    ); } return shape; }
	utils_gpu_available constexpr typename auto& rotate_self   (shape::concepts::has_vertices auto& shape, const angle::concepts::angle auto& rotation   ) noexcept { for(auto& vertex : shape.vertices) { rotate_self   (vertex, rotation   ); } return shape; }
	utils_gpu_available constexpr typename auto& translate_self(shape::concepts::has_vertices auto& shape, const vec2f                      & translation) noexcept { for(auto& vertex : shape.vertices) { translate_self(vertex, translation); } return shape; }
	utils_gpu_available constexpr typename auto& transform_self(shape::concepts::has_vertices auto& shape, const transform2                 & transform  ) noexcept { for(auto& vertex : shape.vertices) { transform_self(vertex, transform  ); } return shape; }
	}