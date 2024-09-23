#pragma once

#include "return_types.h"
#include "../shape/point.h"

namespace utils::math
	{
	template <typename T, size_t extent>
	struct vec<T, extent>::sdf_proxy
		{
		using shape_t = vec<T, extent>;
		#include "common.inline.h"
		
		utils_gpu_available constexpr geometry::shape::point closest_point() const noexcept
			{
			return point; 
			}

		utils_gpu_available constexpr float minimum_distance() const noexcept
			{
			return vec2f::distance(shape, point);
			}

		utils_gpu_available constexpr geometry::sdf::side side() const noexcept
			{
			return (shape == point) ? geometry::sdf::side::create::coincident() : geometry::sdf::side::create::outside();
			}

		utils_gpu_available constexpr geometry::sdf::signed_distance signed_distance() const noexcept
			{
			return {minimum_distance()};
			}

		utils_gpu_available constexpr geometry::sdf::closest_point_with_distance closest_with_distance() const noexcept
			{
			return {shape, minimum_distance()};
			}

		utils_gpu_available constexpr geometry::sdf::closest_point_with_signed_distance closest_with_signed_distance() const noexcept
			{
			return {shape, minimum_distance()};
			}
		};
	}

namespace utils::math
	{
	template <typename T, size_t extent>
	vec<T, extent>::sdf_proxy vec<T, extent>::sdf(const vec<float, 2>& point) const noexcept 
		requires(std::same_as<value_type, float> && extent == 2)
		{
		return {*this, point};
		}
	}