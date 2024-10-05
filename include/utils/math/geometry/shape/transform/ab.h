#pragma once

#include "common.h"
#include "../ab.h"
#include "point.h"

namespace utils::math::geometry::shape::generic
	{
	template <storage::type storage_type, geometry::ends::optional_ab optional_ends>
	utils_gpu_available constexpr auto& ab<storage_type, optional_ends>::scale_self(this auto& self, const float& scaling) noexcept
		requires(!std::remove_cvref_t<decltype(self)>::storage_type.is_const())
		{
		self.a.translate_self(scaling);
		self.b.translate_self(scaling);
		return self;
		}

	template <storage::type storage_type, geometry::ends::optional_ab optional_ends>
	utils_gpu_available constexpr auto& ab<storage_type, optional_ends>::rotate_self(this auto& self, const angle::concepts::angle auto& rotation) noexcept
		requires(!std::remove_cvref_t<decltype(self)>::storage_type.is_const())
		{
		self.a.translate_self(rotation);
		self.b.translate_self(rotation);
		return self;
		}

	template <storage::type storage_type, geometry::ends::optional_ab optional_ends>
	utils_gpu_available constexpr auto& ab<storage_type, optional_ends>::translate_self(this auto& self, const vec2f& translation) noexcept
		requires(!std::remove_cvref_t<decltype(self)>::storage_type.is_const())
		{
		self.a.translate_self(translation);
		self.b.translate_self(translation);
		return self;
		}
	}