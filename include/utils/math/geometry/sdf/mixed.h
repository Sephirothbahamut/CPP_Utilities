#pragma once

#include "common.h"
#include "../shape/mixed.h"
#include "bezier.h"
#include "ab.h"

namespace utils::math::geometry::shape::generic
	{
	template <storage::type storage_type, geometry::ends::closeable ends>
	struct mixed<storage_type, ends>::sdf_proxy
		{
		using shape_t = mixed<storage_type, ends>;

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
			return closest_with_distance().closest;
			}

		utils_gpu_available constexpr float minimum_distance() const noexcept
			{
			float ret{utils::math::constants::finf};

			shape.get_pieces().for_each([this, &ret](const auto& piece, size_t first_index, size_t last_index)
				{
				ret = utils::math::min(ret, piece.sdf(point).minimum_distance());
				});
			return ret;
			}
	
		utils_gpu_available constexpr geometry::sdf::closest_point_with_distance closest_with_distance() const noexcept
			{
			geometry::sdf::closest_point_with_distance ret;
			shape.get_pieces().for_each([this, &ret](const auto& piece)
				{
				ret.set_to_closest(piece.sdf(point).closest_with_distance());
				});
			return ret;
			}
	
	
		utils_gpu_available constexpr geometry::sdf::closest_point_with_signed_distance closest_with_signed_distance() const noexcept
			{
			
			//if constexpr (ends.is_closed())
			//	{//Intersection count, actually performs worse because I didn't math out bezier/line intersection, and with segmentation ad good resolution it gets way slower :|
			//	geometry::sdf::closest_point_with_distance current;
			//	bool inside{false};
			//	shape.get_pieces().for_each([&](const auto& candidate)
			//		{
			//		const geometry::sdf::closest_point_with_distance candidate_values{candidate.sdf(point).closest_with_distance()};
			//		current.set_to_closest(candidate_values);
			//		if (candidate.sdf(point).closed_piece_side_check()) { inside = !inside; }
			//		});
			//	return geometry::sdf::closest_point_with_signed_distance{.closest{current.closest}, .distance{current.distance * (inside ? -1.f : 1.f)}};
			//	}
			//
			//if constexpr (ends.is_open())
				{
				geometry::sdf::closest_point_with_signed_distance current;

				const auto& cc{current.closest };
				const auto& cd{current.distance};

				bool current_is_vertex{false};
				size_t current_index  {0};

				size_t index{0};
				shape.get_pieces().for_each([&](const auto& candidate, size_t first_index, size_t last_index)
					{
					const auto candidate_values{candidate.sdf(point).closest_with_signed_distance()};
					if (candidate_values.distance.absolute() < current.distance.absolute())
						{
						current = candidate_values;
						current_is_vertex = current.closest == shape.vertices.ends_aware_access(last_index);
						current_index = last_index;
						}
					index++;
					});

				if constexpr (shape.ends.is_closed())
					{
					if (current.closest == shape.vertices[0])// && current_index == 0)
						{
						current_is_vertex = true;
						current_index = 0;
						}
					}

				if (current_is_vertex)
					{
					const bool closed_or_not_last_nor_first{shape.ends.is_closed() || (current_index < shape.vertices.size() - 1 && current_index > 0)};
					if (closed_or_not_last_nor_first)
						{
						const vec2f point_a{shape.vertices.ends_aware_access(current_index > 0 ? current_index - 1 : shape.vertices.size() - 1)};
						const vec2f point_b{shape.vertices.ends_aware_access(current_index    )};
						const vec2f point_c{shape.vertices.ends_aware_access(current_index + 1)};
		
						const shape::line line_a{point_a, point_b};
						const shape::line line_b{point_b, point_c};
		
						const float distance_a{line_a.sdf(point).minimum_distance()};
						const float distance_b{line_b.sdf(point).minimum_distance()};
		
						const bool                return_first{distance_a > distance_b};
						const geometry::sdf::side side{(return_first ? line_a : line_b).sdf(point).side()};
		
						current.distance = geometry::sdf::signed_distance{current.distance.absolute() * side};
						}
					}

				return current;
				}
			}

		utils_gpu_available constexpr geometry::sdf::side side() const noexcept
			{
			return closest_with_signed_distance().distance.side();
			}
		
		utils_gpu_available constexpr geometry::sdf::signed_distance signed_distance() const noexcept
			{
			return closest_with_signed_distance().distance;
			}
		};
	}

namespace utils::math::geometry::shape::generic
	{
	template <storage::type storage_type, geometry::ends::closeable ends>
	utils_gpu_available constexpr mixed<storage_type, ends>::sdf_proxy mixed<storage_type, ends>::sdf(const shape::point& point) const noexcept
		{
		return {*this, point};
		}
	}