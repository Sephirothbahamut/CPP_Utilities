#pragma once

#include "common.h"
#include "../shape/circle.h"

namespace utils::math::geometry::shape::generic
	{
	template <storage::type storage_type>
	struct circle<storage_type>::sdf_proxy
		{
		using shape_t = circle<storage_type>; 

		sdf_proxy(const shape_t& shape, const vec2f& point) : shape{shape}, point{point} {};
		const shape_t& shape;
		const vec2f point;

		utils_gpu_available constexpr geometry::sdf::direction_signed_distance direction_signed_distance() const noexcept
			{
			const auto closest_with_signed_distance_value{closest_with_signed_distance()};
			return geometry::sdf::direction_signed_distance::create(closest_with_signed_distance_value, point);
			}

		utils_gpu_available constexpr vec2f closest_point() const noexcept
			{
			const auto point_to_centre                    {point - shape.centre};
			const auto point_to_centre_distance           {point_to_centre.get_length()};
			const auto vector_to_closest_point_from_centre{point_to_centre / point_to_centre_distance * shape.radius};

			const auto ret{shape.centre + vector_to_closest_point_from_centre};
			return ret;
			}

		utils_gpu_available constexpr float minimum_distance() const noexcept
			{
			const auto ret{signed_distance().absolute()};
			return ret;
			}

		utils_gpu_available constexpr geometry::sdf::side side() const noexcept
			{
			const auto ret{signed_distance().side()};
			return ret;
			}

		utils_gpu_available constexpr geometry::sdf::signed_distance signed_distance() const noexcept
			{
			const auto distance_to_centre{vec2f::distance(shape.centre, point)};
			const auto ret{distance_to_centre - shape.radius};
			return {ret};
			}

		utils_gpu_available constexpr geometry::sdf::closest_point_with_distance closest_with_distance() const noexcept
			{
			const auto closest{closest_point()};
			const auto distance{minimum_distance()};
			return {closest, distance};
			}

		utils_gpu_available constexpr geometry::sdf::closest_point_with_signed_distance closest_with_signed_distance() const noexcept
			{
			const auto point_to_centre                    {point - shape.centre};
			const auto point_to_centre_distance           {point_to_centre.get_length()};
			const auto vector_to_closest_point_from_centre{point_to_centre / point_to_centre_distance * shape.radius};

			const auto ret_closest_point{shape.centre + vector_to_closest_point_from_centre};
			const auto ret_distance     {point_to_centre_distance - shape.radius};
			return {ret_closest_point, ret_distance};
			}
		};
	}

namespace utils::math::geometry::shape::generic
	{
	template <storage::type storage_type>
	utils_gpu_available constexpr circle<storage_type>::sdf_proxy circle<storage_type>::sdf(const shape::point& point) const noexcept
		{
		return {*this, point};
		}
	}