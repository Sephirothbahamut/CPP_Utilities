#pragma once

#include "base.h"
#include "../point.h"
#include "../ab.h"

namespace utils::math::geometry::interactions
	{
	utils_gpu_available constexpr shape::point closest_point(const shape::concepts::polyline auto& polyline, const vec2f& point) noexcept
		{
		return closest_with_distance(polyline, point).closest;
		}

	utils_gpu_available constexpr float minimum_distance(const shape::concepts::polyline auto& polyline, const vec2f& point) noexcept
		{
		float ret{utils::math::constants::finf};
		polyline.get_edges().for_each([&point, &ret](const auto& edge)
			{
			const auto candidate{minimum_distance(edge, point)};
			ret = std::min(ret, candidate);
			});
		return ret;
		}

	utils_gpu_available constexpr return_types::closest_point_with_distance closest_with_distance(const shape::concepts::polyline auto& polyline, const vec2f& point) noexcept
		{
		return_types::closest_point_with_distance ret;
		polyline.get_edges().for_each([&point, &ret](const auto& edge)
			{
			const auto candidate{closest_with_distance(edge, point)};
			ret.set_to_closest(candidate);
			});
		return ret;
		}

	utils_gpu_available constexpr utils::math::vec2f vertex_at(const shape::concepts::polyline auto& polyline, size_t index)
		{
		return polyline.vertices[index];
		}
	utils_gpu_available constexpr utils::math::vec2f tanget_at(const shape::concepts::polyline auto& polyline, size_t index)
		{
		const auto a{polyline.vertices[index    ]};
		const auto b{polyline.vertices[index + 1]};
		return b - a;
		}

	utils_gpu_available constexpr return_types::closest_point_with_signed_distance closest_with_signed_distance(const shape::concepts::polyline auto& polyline, const vec2f& point) noexcept
		{
		float current_distance{utils::math::constants::finf};
		size_t current_index{0};
		float current_t{0.f};

		const auto edges{polyline.get_edges()};

		edges.for_each([&point, &current_distance, &current_index, &current_t](const auto& candidate, size_t index)
			{
			const float candidate_t       {interactions::closest_t       (candidate, point)};
			const float candidate_distance{interactions::minimum_distance(candidate, point)};
			if (candidate_distance < current_distance)
				{
				current_t        = candidate_t;
				current_distance = candidate_distance;
				current_index    = index;
				}
			});

		if constexpr (polyline.ends.is_closed())
			{
			if (current_index == 0 && current_t == 0.f)
				{
				current_index = edges.size() - 1; 
				current_t     = 1.f;
				}
			}
		const bool closed_or_not_last{polyline.ends.is_closed() || (current_index < edges.size() - 1)};
		if (current_t >= 1.f && closed_or_not_last)
			{
			const shape::point point_a{edges.second_last_point_at<true>(current_index)};
			const shape::point point_b{edges.last_point_at       <true>(current_index)};
			const shape::point point_c{edges.second_point_at     <true>(current_index + 1)};
		
			const shape::line line_a{point_a, point_b};
			const shape::line line_b{point_b, point_c};
		
			const float distance_a{interactions::minimum_distance(line_a, point)};
			const float distance_b{interactions::minimum_distance(line_b, point)};
		
			const bool               return_first{distance_a > distance_b};
			const return_types::side side{interactions::side(return_first ? line_a : line_b, point)};
			const shape::point       closest{line_a.value_at(current_t)};

			const return_types::closest_point_with_signed_distance ret{closest, return_types::signed_distance{current_distance * side}};

			return ret;
			}

		const shape::segment     edge   {edges.ends_aware_access(current_index)};
		const shape::point       closest{edge.value_at          (current_t    )};
		const return_types::side side   {interactions::side(edge, point  )};
		const return_types::closest_point_with_signed_distance ret{closest, current_distance * side};
		return ret;
		}

	utils_gpu_available constexpr return_types::side side(const shape::concepts::polyline auto& polyline, const vec2f& point) noexcept
		requires(polyline.ends.is_open())
		{
		return closest_with_signed_distance(polyline, point).distance.side();
		}

	utils_gpu_available constexpr return_types::signed_distance signed_distance(const shape::concepts::polyline auto& polyline, const vec2f& point) noexcept
		requires(polyline.ends.is_open())
		{
		return closest_with_signed_distance(polyline, point).distance;
		}
	}