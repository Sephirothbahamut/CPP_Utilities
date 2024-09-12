#pragma once

#include "base.h"
#include "../point.h"
#include "../bezier.h"
#include "../ab.h"

namespace utils::math::geometry::interactions
	{
	utils_gpu_available constexpr shape::point closest_point(const shape::concepts::mixed auto& mixed, const vec2f& point) noexcept
		{
		return closest_with_distance(mixed, point).closest;
		}

	utils_gpu_available constexpr float minimum_distance(const shape::concepts::mixed auto& mixed, const vec2f& point) noexcept
		{
		float ret{utils::math::constants::finf};

		auto pieces_view{mixed.get_pieces()};
		pieces_view.for_each([&point, &ret](const auto& piece, size_t index)
			{
			ret = utils::math::min(ret, minimum_distance(piece, point));
			});
		return ret;
		}
	
	utils_gpu_available constexpr return_types::closest_point_with_distance closest_with_distance(const shape::concepts::mixed auto& mixed, const vec2f& point) noexcept
		{
		return_types::closest_point_with_distance ret;
		mixed.get_pieces().for_each([&point, &ret](const auto& piece)
			{
			ret.set_to_closest(closest_with_distance(piece, point));
			});
		return ret;
		}
	
	
	utils_gpu_available constexpr return_types::closest_point_with_signed_distance closest_with_signed_distance(const shape::concepts::mixed auto& mixed, const vec2f& point) noexcept
		{
		return_types::closest_point_with_signed_distance current;
		const auto& cc{current.closest };
		const auto& cd{current.distance};

		bool current_is_vertex{false};
		size_t current_index  {0};

		mixed.get_pieces().for_each([&](const auto& candidate, size_t first_index, size_t last_index)
			{
			const auto candidate_values{interactions::closest_with_signed_distance(candidate, point)};
			if(candidate_values.distance.absolute() < current.distance.absolute())
				{
				current = candidate_values;
				current_is_vertex = current.closest == mixed.vertices.ends_aware_access(last_index);
				if(current_is_vertex)
					{
					current_index = last_index;
					}
				}
			});

		if constexpr (mixed.ends.is_closed())
			{
			if (current.closest == mixed.vertices[0] && current_index == 0)
				{
				current_is_vertex = true;
				}
			}

		const bool closed_or_not_last_nor_first{mixed.ends.is_closed() || (current_index < mixed.vertices.size() - 1 && current_index > 0)};
		if (closed_or_not_last_nor_first && current_is_vertex)
			{
			const shape::point point_a{mixed.vertices.ends_aware_access(current_index > 0 ? current_index - 1 : mixed.vertices.size() - 1)};
			const shape::point point_b{mixed.vertices.ends_aware_access(current_index    )};
			const shape::point point_c{mixed.vertices.ends_aware_access(current_index + 1)};
		
			const shape::line line_a{point_a, point_b};
			const shape::line line_b{point_b, point_c};
		
			const float distance_a{interactions::minimum_distance(line_a, point)};
			const float distance_b{interactions::minimum_distance(line_b, point)};
		
			const bool               return_first{distance_a > distance_b};
			const return_types::side side{interactions::side(return_first ? line_a : line_b, point)};
		
			current.distance = return_types::signed_distance{current.distance.absolute() * side};
			}

		return current;
		}


	//
	//utils_gpu_available constexpr return_types::side side(const shape::concepts::mixed auto& mixed, const vec2f& point) noexcept
	//	{
	//	return closest_with_signed_distance(mixed, point).distance.side();
	//	}
	//
	//utils_gpu_available constexpr return_types::signed_distance signed_distance(const shape::concepts::mixed auto& mixed, const vec2f& point) noexcept
	//	{
	//	return closest_with_signed_distance(mixed, point).distance;
	//	}
	}