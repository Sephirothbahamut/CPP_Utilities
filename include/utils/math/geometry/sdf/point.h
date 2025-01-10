#pragma once

#include "common.h"
#include "../shape/point.h"

namespace utils::math::details
	{
	template<typename T, template <typename, size_t> class unspecialized_derived_T>
	struct vec_sized_specialization<T, 2, unspecialized_derived_T>::sdf_proxy
		{
		using shape_t = vec<T, 2>;
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

	template<typename T, template <typename, size_t> class unspecialized_derived_T>
	vec_sized_specialization<T, 2, unspecialized_derived_T>::sdf_proxy vec_sized_specialization<T, 2, unspecialized_derived_T>::sdf(const vec<float, 2>& point) const noexcept 
		{
		//self() instead of this because vec_sized_specialization is a CRTP parent of the actual vec.
		return {self(), point};
		}
	}