#pragma once

#include "common.h"
#include "../shape/capsule.h"
#include "ab.h"

namespace utils::math::geometry::shape::generic
	{
	template <storage::type storage_type>
	utils_gpu_available constexpr auto& capsule<storage_type>::scale_self(this utils::concepts::non_const auto& self, const float& scaling) noexcept
		requires(!std::remove_cvref_t<decltype(self)>::storage_type.is_const())
		{
		self.ab.scale_self(scaling);
		self.radius *= scaling;
		return self;
		}

	template <storage::type storage_type>
	utils_gpu_available constexpr auto& capsule<storage_type>::scale_self(this utils::concepts::non_const auto& self, const utils::math::vec2f& scaling) noexcept
		requires(!std::remove_cvref_t<decltype(self)>::storage_type.is_const())
		{
		static_assert(false, "Operation not supported");
		}

	template <storage::type storage_type>
	utils_gpu_available constexpr auto& capsule<storage_type>::rotate_self(this utils::concepts::non_const auto& self, const angle::concepts::angle auto& rotation) noexcept
		requires(!std::remove_cvref_t<decltype(self)>::storage_type.is_const())
		{
		self.ab.rotate_self(rotation);
		return self;
		}

	template <storage::type storage_type>
	utils_gpu_available constexpr auto& capsule<storage_type>::translate_self(this utils::concepts::non_const auto& self, const vec2f& translation) noexcept
		requires(!std::remove_cvref_t<decltype(self)>::storage_type.is_const())
		{
		self.ab.translate_self(translation);
		return self;
		}
	}