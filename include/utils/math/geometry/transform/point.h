#pragma once

#include "common.h"
#include "../shape/point.h"

namespace utils::math
	{
	template<typename T, size_t size>
	utils_gpu_available constexpr auto& vec<T, size>::scale_self(const float& scaling) noexcept
		{
		if constexpr (std::same_as<value_type, float> && extent == 2)
			{
			return *this *= scaling;
			}
		}

	template<typename T, size_t size>
	utils_gpu_available constexpr auto& vec<T, size>::rotate_self(const angle::concepts::angle auto& rotation) noexcept
		{
		if constexpr (std::same_as<value_type, float> && extent == 2)
			{
			return *this += rotation;
			}
		}

	template<typename T, size_t size>
	utils_gpu_available constexpr auto& vec<T, size>::translate_self(const vec2f& translation) noexcept
		{
		if constexpr (std::same_as<value_type, float> && extent == 2)
			{
			return *this += translation;
			}
		}
	}