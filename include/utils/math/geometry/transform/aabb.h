#pragma once

#include "common.h"
#include "../shape/aabb.h"

namespace utils::math
	{
	template<typename T>
	utils_gpu_available constexpr auto& rect<T>::scale_self(this utils::concepts::non_const auto& self, const float& scaling) noexcept
		requires(!std::remove_cvref_t<decltype(self)>::storage_type.is_const())
		{
		for (auto& value : self)
			{
			value *= scaling;
			}
		return self;
		}

	template<typename T>
	utils_gpu_available constexpr auto& rect<T>::scale_self(this utils::concepts::non_const auto& self, const utils::math::vec2f& scaling) noexcept
		requires(!std::remove_cvref_t<decltype(self)>::storage_type.is_const())
		{
		self.ll() *= scaling.x();
		self.up() *= scaling.y();
		self.rr() *= scaling.x();
		self.dw() *= scaling.y();
		return self;
		}

	template<typename T>
	utils_gpu_available constexpr auto& rect<T>::rotate_self(this utils::concepts::non_const auto& self, const angle::concepts::angle auto& rotation) noexcept
		requires(!std::remove_cvref_t<decltype(self)>::storage_type.is_const())
		{
		return self;
		}

	template<typename T>
	utils_gpu_available constexpr auto& rect<T>::translate_self(this utils::concepts::non_const auto& self, const vec2f& translation) noexcept
		requires(!std::remove_cvref_t<decltype(self)>::storage_type.is_const())
		{
		self.ll() += translation.x();
		self.rr() += translation.x();
		self.up() += translation.y();
		self.dw() += translation.y();
		return self;
		}
	}