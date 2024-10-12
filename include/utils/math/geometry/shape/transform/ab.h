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

	template <storage::type storage_type, geometry::ends::optional_ab optional_ends>
	utils_gpu_available constexpr auto  ab<storage_type, optional_ends>::scale         (this const auto& self, const float                    & scaling    ) noexcept { auto ret{shape::cast_storage<storage::type::create::owner()>(self)}; ret.scale_self    (scaling    ); return ret; }
	template <storage::type storage_type, geometry::ends::optional_ab optional_ends>
	utils_gpu_available constexpr auto  ab<storage_type, optional_ends>::rotate        (this const auto& self, const angle::base<float, 360.f>& rotation   ) noexcept { auto ret{shape::cast_storage<storage::type::create::owner()>(self)}; ret.rotate_self   (rotation   ); return ret; }
	template <storage::type storage_type, geometry::ends::optional_ab optional_ends>
	utils_gpu_available constexpr auto  ab<storage_type, optional_ends>::translate     (this const auto& self, const vec2f                    & translation) noexcept { auto ret{shape::cast_storage<storage::type::create::owner()>(self)}; ret.translate_self(translation); return ret; }
	template <storage::type storage_type, geometry::ends::optional_ab optional_ends>
	utils_gpu_available constexpr auto  ab<storage_type, optional_ends>::transform     (this const auto& self, const utils::math::transform2  & transform  ) noexcept { auto ret{shape::cast_storage<storage::type::create::owner()>(self)}; ret.transform_self(transform  ); return ret; }
	template <storage::type storage_type, geometry::ends::optional_ab optional_ends>
	utils_gpu_available constexpr auto& ab<storage_type, optional_ends>::transform_self(this       auto& self, const utils::math::transform2  & transform  ) noexcept
		requires(!std::remove_cvref_t<decltype(self)>::storage_type.is_const())
		{
		self.scale_self    (transform.scaling    );
		self.rotate_self   (transform.rotation   );
		self.translate_self(transform.translation);
		return self;
		}
	}