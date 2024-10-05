#pragma once

#include "common.h"
#include "../point.h"

namespace utils::math::details
	{
	template<typename T, template <typename, size_t> class unspecialized_derived_T>
	utils_gpu_available constexpr auto& vec_sized_specialization<T, 2, unspecialized_derived_T>::scale_self(this auto& self, const float& scaling) noexcept
		requires(!std::remove_cvref_t<decltype(self)>::storage_type.is_const())
		{
		return self *= scaling;
		}

	template<typename T, template <typename, size_t> class unspecialized_derived_T>
	utils_gpu_available constexpr auto& vec_sized_specialization<T, 2, unspecialized_derived_T>::rotate_self(this auto& self, const angle::concepts::angle auto& rotation) noexcept
		requires(!std::remove_cvref_t<decltype(self)>::storage_type.is_const())
		{
		return self += rotation;
		}

	template<typename T, template <typename, size_t> class unspecialized_derived_T>
	utils_gpu_available constexpr auto& vec_sized_specialization<T, 2, unspecialized_derived_T>::translate_self(this auto& self, const vec2f& translation) noexcept
		requires(!std::remove_cvref_t<decltype(self)>::storage_type.is_const())
		{
		return self += translation;
		}
	}