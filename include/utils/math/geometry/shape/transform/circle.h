#pragma once

#include "common.h"
#include "../circle.h"
#include "point.h"

namespace utils::math::geometry::shape::generic
	{
	template <storage::type storage_type>
	utils_gpu_available constexpr auto& circle<storage_type>::scale_self(this auto& self, const float& scaling) noexcept
		requires(!std::remove_cvref_t<decltype(self)>::storage_type.is_const())
		{
		self.center.scale_self(scaling);
		self.radius *= scaling;
		return self;
		}

	template <storage::type storage_type>
	utils_gpu_available constexpr auto& circle<storage_type>::rotate_self(this auto& self, const angle::concepts::angle auto& rotation) noexcept
		requires(!std::remove_cvref_t<decltype(self)>::storage_type.is_const())
		{
		self.center.rotate_self(rotation);
		return self;
		}

	template <storage::type storage_type>
	utils_gpu_available constexpr auto& circle<storage_type>::translate_self(this auto& self, const vec2f& translation) noexcept
		requires(!std::remove_cvref_t<decltype(self)>::storage_type.is_const())
		{
		self.center.translate_self(translation);
		return self;
		}

	template <storage::type storage_type>
	utils_gpu_available constexpr auto  circle<storage_type>::scale         (this const auto& self, const float                    & scaling    ) noexcept { auto ret{shape::cast_storage<storage::type::create::owner()>(self)}; ret.scale_self    (scaling    ); return ret; }
	template <storage::type storage_type>
	utils_gpu_available constexpr auto  circle<storage_type>::rotate        (this const auto& self, const angle::base<float, 360.f>& rotation   ) noexcept { auto ret{shape::cast_storage<storage::type::create::owner()>(self)}; ret.rotate_self   (rotation   ); return ret; }
	template <storage::type storage_type>
	utils_gpu_available constexpr auto  circle<storage_type>::translate     (this const auto& self, const vec2f                    & translation) noexcept { auto ret{shape::cast_storage<storage::type::create::owner()>(self)}; ret.translate_self(translation); return ret; }
	template <storage::type storage_type>
	utils_gpu_available constexpr auto  circle<storage_type>::transform     (this const auto& self, const utils::math::transform2  & transform  ) noexcept { auto ret{shape::cast_storage<storage::type::create::owner()>(self)}; ret.transform_self(transform  ); return ret; }
	template <storage::type storage_type>
	utils_gpu_available constexpr auto& circle<storage_type>::transform_self(this       auto& self, const utils::math::transform2  & transform  ) noexcept
		requires(!std::remove_cvref_t<decltype(self)>::storage_type.is_const())
		{
		self.scale_self    (transform.scaling    );
		self.rotate_self   (transform.rotation   );
		self.translate_self(transform.translation);
		return self;
		}
	}