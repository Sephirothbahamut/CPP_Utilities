#pragma once

#include "common.h"
#include "../aabb.h"
#include "point.h"

namespace utils::math
	{
	template<typename T>
	utils_gpu_available constexpr auto& rect<T>::scale_self(this auto& self, const float& scaling) noexcept
		requires(!std::remove_cvref_t<decltype(self)>::storage_type.is_const())
		{
		for (auto& value : self)
			{
			value *= scaling;
			}
		return self;
		}

	template<typename T>
	utils_gpu_available constexpr auto& rect<T>::rotate_self(this auto& self, const angle::concepts::angle auto& rotation) noexcept
		requires(!std::remove_cvref_t<decltype(self)>::storage_type.is_const())
		{
		return self;
		}

	template<typename T>
	utils_gpu_available constexpr auto& rect<T>::translate_self(this auto& self, const vec2f& translation) noexcept
		requires(!std::remove_cvref_t<decltype(self)>::storage_type.is_const())
		{
		self.ll() += translation.x();
		self.rr() += translation.x();
		self.up() += translation.y();
		self.dw() += translation.y();
		return self;
		}

	template<typename T>
	utils_gpu_available constexpr auto  rect<T>::scale         (this const auto& self, const float                    & scaling    ) noexcept { rect<float> ret{self}; ret.scale_self    (scaling    ); return ret; }
	template<typename T>
	utils_gpu_available constexpr auto  rect<T>::rotate        (this const auto& self, const angle::base<float, 360.f>& rotation   ) noexcept { rect<float> ret{self}; ret.rotate_self   (rotation   ); return ret; }
	template<typename T>
	utils_gpu_available constexpr auto  rect<T>::translate     (this const auto& self, const vec2f                    & translation) noexcept { rect<float> ret{self}; ret.translate_self(translation); return ret; }
	template<typename T>
	utils_gpu_available constexpr auto  rect<T>::transform     (this const auto& self, const utils::math::transform2  & transform  ) noexcept { rect<float> ret{self}; ret.transform_self(transform  ); return ret; }
	template<typename T>
	utils_gpu_available constexpr auto& rect<T>::transform_self(this       auto& self, const utils::math::transform2  & transform  ) noexcept
		requires(!std::remove_cvref_t<decltype(self)>::storage_type.is_const())
		{
		self.scale_self    (transform.scaling    );
		self.rotate_self   (transform.rotation   );
		self.translate_self(transform.translation);
		return self;
		}
	}