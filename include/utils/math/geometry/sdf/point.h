#pragma once

#include "common.h"
#include "../shape/point.h"

namespace utils::math
	{
	template<typename T, size_t SIZE>
	struct vec<T, SIZE>::sdf_proxy
		{
		using shape_t = vec<T, 2>;

		sdf_proxy(const shape_t& shape, const vec2f& point) : shape{shape}, point{point} {};
		const shape_t& shape;
		const vec2f point;

		utils_gpu_available constexpr geometry::sdf::direction_signed_distance direction_signed_distance() const noexcept
			{
			const auto closest_with_signed_distance_value{closest_with_signed_distance()};
			return geometry::sdf::direction_signed_distance::create(closest_with_signed_distance_value, point);
			}

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



	template<typename T, size_t SIZE>
	utils_gpu_available constexpr vec<T, SIZE>::sdf_proxy vec<T, SIZE>::sdf(const vec<float, 2>& point) const noexcept
		requires(std::convertible_to<value_type, float> && extent == 2)
		{
		static_assert(std::same_as<value_type, float> && SIZE == 2);
		return {*this, point};
		}
	}