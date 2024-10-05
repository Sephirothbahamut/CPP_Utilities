#pragma once

#include "return_types.h"
#include "../mixed.h"
#include "bezier.h"
#include "ab.h"

namespace utils::math::geometry::shape::generic
	{
	template <storage::type storage_type, geometry::ends::closeable ends>
	struct mixed<storage_type, ends>::sdf_proxy
		{
		using shape_t = mixed<storage_type, ends>;
		#include "common.inline.h"
		
		utils_gpu_available constexpr vec2f closest_point() const noexcept
			{
			return closest_with_distance(shape, point).closest;
			}

		utils_gpu_available constexpr float minimum_distance() const noexcept
			{
			float ret{utils::math::constants::finf};

			auto pieces_view{shape.get_pieces()};
			pieces_view.for_each([this, &ret](const auto& piece, size_t index)
				{
				ret = utils::math::min(ret, minimum_distance(piece, point));
				});
			return ret;
			}
	
		utils_gpu_available constexpr geometry::sdf::closest_point_with_distance closest_with_distance() const noexcept
			{
			geometry::sdf::closest_point_with_distance ret;
			shape.get_pieces().for_each([this, &ret](const auto& piece)
				{
				ret.set_to_closest(closest_with_distance(piece, point));
				});
			return ret;
			}
	
	
		utils_gpu_available constexpr geometry::sdf::closest_point_with_signed_distance closest_with_signed_distance() const noexcept
			{
			geometry::sdf::closest_point_with_signed_distance current;
			const auto& cc{current.closest };
			const auto& cd{current.distance};

			bool current_is_vertex{false};
			size_t current_index  {0};

			shape.get_pieces().for_each([&](const auto& candidate, size_t first_index, size_t last_index)
				{
				const auto candidate_values{candidate.sdf(point).closest_with_signed_distance()};
				if(candidate_values.distance.absolute() < current.distance.absolute())
					{
					current = candidate_values;
					current_is_vertex = current.closest == shape.vertices.ends_aware_access(last_index);
					if(current_is_vertex)
						{
						current_index = last_index;
						}
					}
				});

			if constexpr (shape.ends.is_closed())
				{
				if (current.closest == shape.vertices[0] && current_index == 0)
					{
					current_is_vertex = true;
					}
				}

			const bool closed_or_not_last_nor_first{shape.ends.is_closed() || (current_index < shape.vertices.size() - 1 && current_index > 0)};
			if (closed_or_not_last_nor_first && current_is_vertex)
				{
				const vec2f point_a{shape.vertices.ends_aware_access(current_index > 0 ? current_index - 1 : shape.vertices.size() - 1)};
				const vec2f point_b{shape.vertices.ends_aware_access(current_index    )};
				const vec2f point_c{shape.vertices.ends_aware_access(current_index + 1)};
		
				const shape::line line_a{point_a, point_b};
				const shape::line line_b{point_b, point_c};
		
				const float distance_a{line_a.sdf(point).minimum_distance()};
				const float distance_b{line_b.sdf(point).minimum_distance()};
		
				const bool               return_first{distance_a > distance_b};
				const geometry::sdf::side side{(return_first ? line_a : line_b).sdf(point).side()};
		
				current.distance = geometry::sdf::signed_distance{current.distance.absolute() * side};
				}

			return current;
			}


		//
		//utils_gpu_available constexpr geometry::sdf::side side() const noexcept
		//	{
		//	return closest_with_signed_distance(shape, point).distance.side();
		//	}
		//
		//utils_gpu_available constexpr geometry::sdf::signed_distance signed_distance() const noexcept
		//	{
		//	return closest_with_signed_distance(shape, point).distance;
		//	}
		};
	}

namespace utils::math::geometry::shape::generic
	{
	template <storage::type storage_type, geometry::ends::closeable ends>
	mixed<storage_type, ends>::sdf_proxy mixed<storage_type, ends>::sdf(const shape::point& point) const noexcept
		{
		return {*this, point};
		}
	}