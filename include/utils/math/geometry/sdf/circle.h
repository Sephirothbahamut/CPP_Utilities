#pragma once

#include "common.h"
#include "../shape/circle.h"

namespace utils::math::geometry::shape::generic
	{
	template <storage::type storage_type>
	struct circle<storage_type>::sdf_proxy
		{
		using shape_t = circle<storage_type>;
		#include "common.inline.h"

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
			return ret;
			}

		utils_gpu_available constexpr geometry::sdf::closest_point_with_distance closest_with_distance() const noexcept
			{
			return closest_with_signed_distance().absolute();
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
	circle<storage_type>::sdf_proxy circle<storage_type>::sdf(const shape::point& point) const noexcept
		{
		return {*this, point};
		}
	}