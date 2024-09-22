#pragma once

#include "return_types.h"
#include "../shape/mixed.h"

namespace utils::math
	{
	template <typename T>
	struct rect<T>::sdf_proxy
		{
		using shape_t = rect<T>;
		#include "common.inline.h"

		utils_gpu_available constexpr vec2f closest_point() const noexcept
			{
			if (point.x() <= shape.ll())
				{
				if (point.y() <= shape.up()) { return shape.ul(); }
				if (point.y() >= shape.dw()) { return shape.dl(); }
				return {shape.ll(), point.y()};
				}
			else if (point.x() >= shape.rr())
				{
				if (point.y() <= shape.up()) { return shape.ur(); }
				if (point.y() >= shape.dw()) { return shape.dr(); }
				return {shape.rr(), point.y()};
				}
			else if (point.y() <= shape.up()) 
				{
				return {point.x(), shape.up()};
				}
			else if (point.y() >= shape.dw())
				{
				return {point.x(), shape.dw()};
				}

			const float distance_ll{std::abs(point.x() - shape.ll())};
			const float distance_rr{std::abs(point.x() - shape.rr())};
			const float distance_up{std::abs(point.y() - shape.up())};
			const float distance_dw{std::abs(point.y() - shape.dw())};
			const float min_distance_horizontal{std::min(distance_ll, distance_rr)};
			const float min_distance_vertical  {std::min(distance_up, distance_dw)};

			if (min_distance_horizontal < min_distance_vertical)
				{
				return {distance_ll < distance_rr ? shape.ll() : shape.rr(), point.y()};
				}
			else
				{
				return {point.x(), distance_up < distance_dw ? shape.up() : shape.dw()};
				}
			}

		utils_gpu_available constexpr float minimum_distance() const noexcept
			{
			return signed_distance().absolute();
			}

		utils_gpu_available constexpr geometry::sdf::side side() const noexcept
			{
			if (point.x() > shape.ll() && point.x() < shape.rr() && point.y() > shape.up() && point.y() < shape.dw()) { return geometry::sdf::side::create::inside (); }
			if (point.x() < shape.ll() || point.x() > shape.rr() || point.y() < shape.up() || point.y() > shape.dw()) { return geometry::sdf::side::create::outside(); }
			return geometry::sdf::side::create::coincident();
			}

		utils_gpu_available constexpr geometry::sdf::signed_distance signed_distance() const noexcept
			{
			const vec2f point_from_center_ur_quadrant{utils::math::abs(point - shape.centre())};
			const vec2f corner_from_center{shape.ur() - shape.centre()};
			const vec2f distances{point_from_center_ur_quadrant - corner_from_center};
			return {utils::math::max(distances, {0.f}).get_length() + utils::math::min(utils::math::max(distances.x(), distances.y()), 0.f)};
			}

		utils_gpu_available constexpr geometry::sdf::closest_point_with_distance closest_with_distance() const noexcept
			{
			const auto closest{closest_point(shape, point)};
			return {closest, minimum_distance()};
			}

		utils_gpu_available constexpr geometry::sdf::closest_point_with_signed_distance closest_with_signed_distance() const noexcept
			{
			//TODO test, I had it return positive both inside and outside
			const auto closest{closest_point()};
			return {closest, minimum_distance() * side()};
			}
		};
	}

namespace utils::math
	{
	template <typename T>
	utils_gpu_available rect<T>::sdf_proxy rect<T>::sdf(const vec2f& point) const noexcept requires(std::same_as<value_type, float>)
		{
		return {*this, point};
		}
	}