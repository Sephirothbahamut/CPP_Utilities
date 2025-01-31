#pragma once

#include "common.h"
#include "../shape/mixed.h"
#include "bezier.h"
#include "ab.h"

namespace utils::math::geometry::sdf
	{
	template <typename iterable_shapes>
		requires(shape::concepts::shape<typename iterable_shapes::value_type>)
	struct composite
		{
		const iterable_shapes& shapes;
		const vec2f point;

		geometry::sdf::direction_signed_distance direction_signed_distance() noexcept
			{
			const auto closest_with_signed_distance_value{closest_with_signed_distance()};
			return geometry::sdf::direction_signed_distance::create(closest_with_signed_distance_value, point);
			}
		
		utils_gpu_available constexpr vec2f closest_point() const noexcept
			{
			return closest_with_distance().closest;
			}

		utils_gpu_available constexpr float minimum_distance() const noexcept
			{
			float result{utils::math::constants::finf};
			for (const auto& shape : shapes)
				{
				result = std::min(result, shape.sdf(point).minimum_distance());
				}
			return result;
			}
	
		utils_gpu_available constexpr geometry::sdf::closest_point_with_distance closest_with_distance() const noexcept
			{
			geometry::sdf::closest_point_with_distance result;
			for (const auto& shape : shapes)
				{
				result.set_to_closest(shape.sdf(point).closest_with_distance());
				}
			return result;
			}
	
	
		utils_gpu_available constexpr geometry::sdf::closest_point_with_signed_distance closest_with_signed_distance() const noexcept
			{
			const auto closest_point_with_distance{closest_with_distance()};
			const auto s{side()};
			return geometry::sdf::closest_point_with_signed_distance{.closest{closest_point_with_distance.closest}, .distance{closest_point_with_distance.distance * s}};
			}

		utils_gpu_available constexpr geometry::sdf::side side() const noexcept
			{
			float side{1.f};
			for (const auto& shape : shapes)
				{
				side *= shape.sdf(point).side().sign();
				}
			return {side};
			}
		
		utils_gpu_available constexpr geometry::sdf::signed_distance signed_distance() const noexcept
			{
			return closest_with_signed_distance().distance;
			}
		};
	}