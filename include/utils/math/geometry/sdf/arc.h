#pragma once

#include "common.h"
#include "../shape/arc.h"
#include "circle.h"
#include "ab.h"

namespace utils::math::geometry::shape::generic
	{
	template <storage::type storage_type>
	struct arc<storage_type>::sdf_proxy
		{
		using shape_t = arc<storage_type>; 

		sdf_proxy(const shape_t& shape, const vec2f& point) : shape{shape}, point{point} {};
		const shape_t& shape;
		const vec2f point;

		utils_gpu_available constexpr geometry::sdf::direction_signed_distance direction_signed_distance() const noexcept
			{
			const auto closest_with_signed_distance_value{closest_with_signed_distance()};
			return geometry::sdf::direction_signed_distance::create(closest_with_signed_distance_value, point);
			}

		utils_gpu_available constexpr bool angle_in_arc() const noexcept
			{
			const vec2f centre_to_point{point - shape.centre};
			const auto angle{centre_to_point.angle()};
			const auto start_angle{shape.start_angle.value()};
			const auto end_angle{start_angle + shape.aperture_angle.value()};
			return angle.within(start_angle, end_angle);
			}

		utils_gpu_available constexpr utils::math::angle::degf closest_angle() const noexcept
			{
			const vec2f centre_to_point{point - shape.centre};
			const auto angle{centre_to_point.angle()};
			const auto start_angle{shape.start_angle.value()};
			const auto end_angle{start_angle + shape.aperture_angle.value()};
			if (angle.within(start_angle, end_angle))
				{
				return angle;
				}
			else
				{
				const auto start_distance{math::angle::degf::min_distance(angle, start_angle)};
				const auto end_distance  {math::angle::degf::min_distance(angle, end_angle)};
				return std::abs(start_distance) < std::abs(end_distance) ? start_angle : end_angle;
				}
			}

		utils_gpu_available constexpr vec2f closest_point() const noexcept
			{
			return shape.centre + vec2f::create::from_angle(closest_angle(), shape.radius);
			}

		utils_gpu_available constexpr float minimum_distance() const noexcept
			{
			const auto closest{closest_point()};
			const auto distance{vec2f::distance(closest, point)};
			return distance;
			}

		utils_gpu_available constexpr geometry::sdf::side side() const noexcept
			{
			if (angle_in_arc())
				{
				return shape::circle{shape.centre, shape.radius}.sdf(point).side();
				}
			else
				{
				const auto tmp_angle{closest_angle()};
				const auto tmp_point{closest_point()};
				const auto clockwise_angle{tmp_angle + utils::math::angle::degf{90.f}};
				const shape::line line{tmp_point, tmp_point + vec2f::create::from_angle(clockwise_angle, 1.f)};
				return line.sdf(point).side();
				}
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
			const auto distance{vec2f::distance(closest, point)};
			return {closest, distance};
			}

		utils_gpu_available constexpr geometry::sdf::closest_point_with_signed_distance closest_with_signed_distance() const noexcept
			{
			const auto closest{closest_point()};
			const auto distance{vec2f::distance(closest, point)};
			const geometry::sdf::signed_distance ret_distance{distance * side()};
			return {closest, ret_distance};
			}
		};
	}

namespace utils::math::geometry::shape::generic
	{
	template <storage::type storage_type>
	utils_gpu_available constexpr arc<storage_type>::sdf_proxy arc<storage_type>::sdf(const shape::point& point) const noexcept
		{
		return {*this, point};
		}
	}