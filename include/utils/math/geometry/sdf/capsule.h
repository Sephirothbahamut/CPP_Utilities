#pragma once

#include "common.h"
#include "../shape/capsule.h"

namespace utils::math::geometry::shape::generic
	{
	template <storage::type storage_type>
	struct capsule<storage_type>::sdf_proxy
		{
		using shape_t = capsule<storage_type>;

		sdf_proxy(const shape_t& shape, const vec2f& point) : shape{shape}, point{point} {};
		const shape_t& shape;
		const vec2f point;
		
		//utils_gpu_available constexpr vec2f closest_point() const noexcept
		//	{
		//	}
		
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
			const auto ab_distance{shape.ab.sdf(point).minimum_distance()};
			const auto ret{ab_distance - shape.radius};
			return ret;
			}
		
		//utils_gpu_available constexpr geometry::sdf::closest_point_with_distance closest_with_distance() const noexcept
		//	{
		//	return closest_with_signed_distance().absolute();
		//	}
		
		//utils_gpu_available constexpr geometry::sdf::closest_point_with_signed_distance closest_with_signed_distance() const noexcept
		//	{
		//	const auto point_to_centre                    {point - shape.centre};
		//	const auto point_to_centre_distance           {point_to_centre.get_length()};
		//	const auto vector_to_closest_point_from_centre{point_to_centre / point_to_centre_distance * shape.radius};
		//
		//	const auto ret_closest_point{shape.centre + vector_to_closest_point_from_centre};
		//	const auto ret_distance     {point_to_centre_distance - shape.radius};
		//	return {ret_closest_point, ret_distance};
		//	}
		
		utils_gpu_available constexpr geometry::sdf::gradient_signed_distance gradient_signed_distance() const noexcept
			{
			const auto tmp{shape.ab.sdf(point).gradient_signed_distance()};
			const float distance{tmp.distance.absolute() - shape.radius};
			const auto gradient{distance < 0.f ? -tmp.gradient : tmp.gradient};
			//const vec2f ba{shape.ab.b - shape.ab.a};
			//const vec2f pa{point      - shape.ab.a};
			//
			//const auto dot_pa_ba{utils::math::vec2f::dot(pa, ba)};
			//const auto dot_ba_ba{utils::math::vec2f::dot(ba, ba)};
			//const float h{std::clamp(dot_pa_ba / dot_ba_ba, 0.f, 1.f)};
			//const vec2f q{pa - ba * h};
			//const float distance{q.get_length() - shape.radius};
			//const auto  gradient{-q / distance};
			return {distance, gradient};
			}
		};
	}

namespace utils::math::geometry::shape::generic
	{
	template <storage::type storage_type>
	capsule<storage_type>::sdf_proxy capsule<storage_type>::sdf(const shape::point& point) const noexcept
		{
		return {*this, point};
		}
	}