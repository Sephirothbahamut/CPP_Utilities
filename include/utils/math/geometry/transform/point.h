#pragma once

#include "common.h"
#include "../shape/point.h"

namespace utils::math
	{
	template<typename T, size_t SIZE>
	utils_gpu_available constexpr auto& vec<T, SIZE>::scale_self(this utils::concepts::non_const auto& self, const float& scaling) noexcept
		requires(std::convertible_to<value_type, float> && extent == 2 && !std::remove_cvref_t<decltype(self)>::storage_type.is_const())
		{
		return self *= scaling;
		}

	template<typename T, size_t SIZE>
	utils_gpu_available constexpr auto& vec<T, SIZE>::scale_self(this utils::concepts::non_const auto& self, const utils::math::vec2f& scaling) noexcept
		requires(std::convertible_to<value_type, float> && extent == 2 && !std::remove_cvref_t<decltype(self)>::storage_type.is_const())
		{
		return self *= scaling;
		}

	template<typename T, size_t SIZE>
	utils_gpu_available constexpr auto& vec<T, SIZE>::rotate_self(this utils::concepts::non_const auto& self, const angle::degf& rotation) noexcept
		requires(std::convertible_to<value_type, float> && extent == 2 && !std::remove_cvref_t<decltype(self)>::storage_type.is_const())
		{
		self.operator+=(rotation);

		return self;
		}

	template<typename T, size_t SIZE>
	utils_gpu_available constexpr auto& vec<T, SIZE>::translate_self(this utils::concepts::non_const auto& self, const vec2f& translation) noexcept
		requires(std::convertible_to<value_type, float> && extent == 2 && !std::remove_cvref_t<decltype(self)>::storage_type.is_const())
		{
		return self += translation;
		}
	}