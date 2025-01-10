#pragma once

#include "common.h"
#include "../shape/vertices.h"
#include "point.h"

namespace utils::math::geometry
	{
	template <concepts::vertices vertices_t>
	utils_gpu_available constexpr auto& vertices_as_field<vertices_t>::scale_self(this auto& self, const float& scaling) noexcept
		requires(!std::remove_cvref_t<decltype(self)>::storage_type.is_const())
		{
		for (auto& vertex : self.vertices)
			{
			vertex.scale_self(scaling);
			}
		return self;
		}

	template <concepts::vertices vertices_t>
	utils_gpu_available constexpr auto& vertices_as_field<vertices_t>::rotate_self(this auto& self, const angle::concepts::angle auto& rotation) noexcept
		requires(!std::remove_cvref_t<decltype(self)>::storage_type.is_const())
		{
		for (auto& vertex : self.vertices)
			{
			vertex.rotate_self(rotation);
			}
		return self;
		}

	template <concepts::vertices vertices_t>
	utils_gpu_available constexpr auto& vertices_as_field<vertices_t>::translate_self(this auto& self, const vec2f& translation) noexcept
		requires(!std::remove_cvref_t<decltype(self)>::storage_type.is_const())
		{
		for (auto& vertex : self.vertices)
			{
			vertex.translate_self(translation);
			}
		return self;
		}
	}