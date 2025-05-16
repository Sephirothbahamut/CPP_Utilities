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
		
		utils_gpu_available constexpr vec2f closest_point() const noexcept
			{
			//TODO this is just to do
			return {std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity()};
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
			const auto ab_distance{shape.ab.sdf(point).minimum_distance()};
			const auto ret{ab_distance - shape.radius};
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
			//TODO this is just to do
			return {};
			}
		
		utils_gpu_available constexpr geometry::sdf::direction_signed_distance direction_signed_distance() const noexcept
			{
			const auto tmp{shape.ab.sdf(point).direction_signed_distance()};
			const float distance{tmp.distance.absolute() - shape.radius};
			const auto direction{distance < 0.f ? -tmp.direction : tmp.direction};
			//const vec2f ba{shape.ab.b - shape.ab.a};
			//const vec2f pa{point      - shape.ab.a};
			//
			//const auto dot_pa_ba{utils::math::vec2f::dot(pa, ba)};
			//const auto dot_ba_ba{utils::math::vec2f::dot(ba, ba)};
			//const float h{std::clamp(dot_pa_ba / dot_ba_ba, 0.f, 1.f)};
			//const vec2f q{pa - ba * h};
			//const float distance{q.get_length() - shape.radius};
			//const auto  direction{-q / distance};
			return {distance, direction};
			}
		};
	}

namespace utils::math::geometry::shape::generic
	{
	template <storage::type storage_type>
	utils_gpu_available constexpr capsule<storage_type>::sdf_proxy capsule<storage_type>::sdf(const shape::point& point) const noexcept
		{
		return {*this, point};
		}
	}