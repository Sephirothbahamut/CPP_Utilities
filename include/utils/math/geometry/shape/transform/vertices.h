#pragma once

#include "common.h"
#include "../vertices.h"
#include "point.h"

namespace utils::math::geometry
	{
	template <concepts::vertices vertices_t>
	utils_gpu_available constexpr auto& vertices_as_field<vertices_t>::scale_self(this auto& self, const float& scaling) noexcept
		requires(!std::remove_cvref_t<decltype(self)>::storage_type.is_const())
		{
		for (auto& vertex : self.vertices)
			{
			vertex.scale_self(scaling);
			}
		return self;
		}

	template <concepts::vertices vertices_t>
	utils_gpu_available constexpr auto& vertices_as_field<vertices_t>::rotate_self(this auto& self, const angle::concepts::angle auto& rotation) noexcept
		requires(!std::remove_cvref_t<decltype(self)>::storage_type.is_const())
		{
		for (auto& vertex : self.vertices)
			{
			vertex.rotate_self(rotation);
			}
		return self;
		}

	template <concepts::vertices vertices_t>
	utils_gpu_available constexpr auto& vertices_as_field<vertices_t>::translate_self(this auto& self, const vec2f& translation) noexcept
		requires(!std::remove_cvref_t<decltype(self)>::storage_type.is_const())
		{
		for (auto& vertex : self.vertices)
			{
			vertex.translate_self(translation);
			}
		return self;
		}

	template <concepts::vertices vertices_t>
	utils_gpu_available constexpr auto  vertices_as_field<vertices_t>::scale         (this const auto& self, const float                    & scaling    ) noexcept { auto ret{shape::cast_storage<storage::type::create::owner()>(self)}; ret.scale_self    (scaling    ); return ret; }
	template <concepts::vertices vertices_t>
	utils_gpu_available constexpr auto  vertices_as_field<vertices_t>::rotate        (this const auto& self, const angle::base<float, 360.f>& rotation   ) noexcept { auto ret{shape::cast_storage<storage::type::create::owner()>(self)}; ret.rotate_self   (rotation   ); return ret; }
	template <concepts::vertices vertices_t>
	utils_gpu_available constexpr auto  vertices_as_field<vertices_t>::translate     (this const auto& self, const vec2f                    & translation) noexcept { auto ret{shape::cast_storage<storage::type::create::owner()>(self)}; ret.translate_self(translation); return ret; }
	template <concepts::vertices vertices_t>
	utils_gpu_available constexpr auto  vertices_as_field<vertices_t>::transform     (this const auto& self, const utils::math::transform2  & transform  ) noexcept { auto ret{shape::cast_storage<storage::type::create::owner()>(self)}; ret.transform_self(transform  ); return ret; }
	template <concepts::vertices vertices_t>
	utils_gpu_available constexpr auto& vertices_as_field<vertices_t>::transform_self(this       auto& self, const utils::math::transform2  & transform  ) noexcept
		requires(!std::remove_cvref_t<decltype(self)>::storage_type.is_const())
		{
		self.scale_self    (transform.scaling    );
		self.rotate_self   (transform.rotation   );
		self.translate_self(transform.translation);
		return self;
		}
	}