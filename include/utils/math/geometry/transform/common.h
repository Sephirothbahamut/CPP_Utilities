#pragma once

#include "../details/base_types.h"
#include "../../vec.h"
#include "../../angle.h"
#include "../../transform2.h"

namespace utils::math::geometry
	{
	utils_gpu_available constexpr auto  shape_flag::scale         (this const auto& self, const float                    & scaling    ) noexcept { auto ret{shape::cast_storage<storage::type::create::owner()>(self)}; ret.scale_self    (scaling    ); return ret; }
	utils_gpu_available constexpr auto  shape_flag::rotate        (this const auto& self, const angle::base<float, 360.f>& rotation   ) noexcept { auto ret{shape::cast_storage<storage::type::create::owner()>(self)}; ret.rotate_self   (rotation   ); return ret; }
	utils_gpu_available constexpr auto  shape_flag::translate     (this const auto& self, const vec2f                    & translation) noexcept { auto ret{shape::cast_storage<storage::type::create::owner()>(self)}; ret.translate_self(translation); return ret; }
	utils_gpu_available constexpr auto  shape_flag::transform     (this const auto& self, const utils::math::transform2  & transform  ) noexcept { auto ret{shape::cast_storage<storage::type::create::owner()>(self)}; ret.transform_self(transform  ); return ret; }
	utils_gpu_available constexpr auto& shape_flag::transform_self(this       auto& self, const utils::math::transform2  & transform  ) noexcept
		requires(!std::remove_cvref_t<decltype(self)>::storage_type.is_const())
		{
		self.scale_self    (transform.scaling    );
		self.rotate_self   (transform.rotation   );
		self.translate_self(transform.translation);
		return self;
		}
	}