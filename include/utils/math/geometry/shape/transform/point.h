#pragma once

#include "common.h"
#include "../point.h"

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

	template<typename T, size_t size>
	utils_gpu_available constexpr auto  vec<T, size>::scale         (this const auto& self, const float                    & scaling    ) noexcept { auto ret{geometry::shape::cast_storage<storage::type::create::owner()>(self)}; ret.scale_self    (scaling    ); return ret; }
	template<typename T, size_t size>
	utils_gpu_available constexpr auto  vec<T, size>::rotate        (this const auto& self, const angle::base<float, 360.f>& rotation   ) noexcept { auto ret{geometry::shape::cast_storage<storage::type::create::owner()>(self)}; ret.rotate_self   (rotation   ); return ret; }
	template<typename T, size_t size>
	utils_gpu_available constexpr auto  vec<T, size>::translate     (this const auto& self, const vec2f                    & translation) noexcept { auto ret{geometry::shape::cast_storage<storage::type::create::owner()>(self)}; ret.translate_self(translation); return ret; }
	template<typename T, size_t size>
	utils_gpu_available constexpr auto  vec<T, size>::transform     (this const auto& self, const utils::math::transform2  & transform  ) noexcept { auto ret{geometry::shape::cast_storage<storage::type::create::owner()>(self)}; ret.transform_self(transform  ); return ret; }
	template<typename T, size_t size>
	utils_gpu_available constexpr auto& vec<T, size>::transform_self(this       auto& self, const utils::math::transform2  & transform  ) noexcept
		requires(!std::remove_cvref_t<decltype(self)>::storage_type.is_const())
		{
		self.scale_self    (transform.scaling    );
		self.rotate_self   (transform.rotation   );
		self.translate_self(transform.translation);
		return self;
		}
	}