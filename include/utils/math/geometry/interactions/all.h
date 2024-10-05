#pragma once

#include "point.h"
#include "aabb.h"
#include "ab.h"
#include "vertices.h"
#include "polyline.h"
#include "bezier.h"
#include "mixed.h"

namespace utils::math::geometry::interactions
	{
	return_types::gradient_signed_distance gradient_signed_distance(const shape::concepts::shape auto& shape, const shape::concepts::point auto& point) noexcept
		{
		const auto closest_with_signed_distance{interactions::closest_with_signed_distance(shape, point)};
		return return_types::gradient_signed_distance::create(closest_with_signed_distance, point);
		}

	template <shape::concepts::shape T> utils_gpu_available constexpr auto scale    (const T& shape, const float                    & scaling    ) noexcept { typename shape::cast_storage_type<T, storage::type::create::owner()>::type ret{shape}; return interactions::scale_self    (ret, scaling    ); }
	template <shape::concepts::shape T> utils_gpu_available constexpr auto rotate   (const T& shape, const angle::base<float, 360.f>& rotation   ) noexcept { typename shape::cast_storage_type<T, storage::type::create::owner()>::type ret{shape}; return interactions::rotate_self   (ret, rotation   ); }
	template <shape::concepts::shape T> utils_gpu_available constexpr auto translate(const T& shape, const vec2f                    & translation) noexcept { typename shape::cast_storage_type<T, storage::type::create::owner()>::type ret{shape}; return interactions::translate_self(ret, translation); }
	template <shape::concepts::shape T> utils_gpu_available constexpr auto transform(const T& shape, const utils::math::transform2  & transform  ) noexcept 
		{
		return translate(rotate(scale(shape, transform.scaling), transform.rotation), transform.translation);
		}
	
	utils_gpu_available constexpr auto& transform_self(shape::concepts::shape auto& shape, const utils::math::transform2& transform) noexcept
		{
		return translate_self(rotate_self(scale_self(shape, transform.scaling), transform.rotation), transform.translation);
		}
	}