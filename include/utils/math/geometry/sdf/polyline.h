#pragma once

#include "common.h"
#include "../shape/polyline.h"
#include "ab.h"

namespace utils::math::geometry::shape::generic
	{
	template <storage::type storage_type, geometry::ends::closeable ends, size_t extent>
	struct polyline<storage_type, ends, extent>::sdf_proxy
		{
		using shape_t = polyline<storage_type, ends, extent>;

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
			shape.get_edges().for_each([this, &ret](const auto& edge)
				{
				const auto candidate{minimum_distance()};
				ret = std::min(ret, candidate);
				});
			return ret;
			}

		utils_gpu_available constexpr geometry::sdf::closest_point_with_distance closest_with_distance() const noexcept
			{
			geometry::sdf::closest_point_with_distance ret;
			shape.get_edges().for_each([this, &ret](const auto& edge)
				{
				const auto candidate{closest_with_distance()};
				ret.set_to_closest(candidate);
				});
			return ret;
			}

		utils_gpu_available constexpr utils::math::vec2f vertex_at(const shape::concepts::shape auto& shape, size_t index)
			{
			return shape.vertices[index];
			}
		utils_gpu_available constexpr utils::math::vec2f tanget_at(const shape::concepts::shape auto& shape, size_t index)
			{
			const auto a{shape.vertices[index    ]};
			const auto b{shape.vertices[index + 1]};
			return b - a;
			}

		utils_gpu_available constexpr geometry::sdf::closest_point_with_signed_distance closest_with_signed_distance() const noexcept
			{
			float current_distance{utils::math::constants::finf};
			size_t current_index{0};
			float current_t{0.f};

			const auto edges{shape.get_edges()};

			edges.for_each([this, &current_distance, &current_index, &current_t](const auto& candidate, size_t index)
				{
				const float candidate_t       {candidate.sdf(point).closest_t       ()};
				const float candidate_distance{candidate.sdf(point).minimum_distance()};
				if (candidate_distance < current_distance)
					{
					current_t        = candidate_t;
					current_distance = candidate_distance;
					current_index    = index;
					}
				});

			if constexpr (shape.ends.is_closed())
				{
				if (current_index == 0 && current_t == 0.f)
					{
					current_index = edges.size() - 1; 
					current_t     = 1.f;
					}
				}
			const bool closed_or_not_last{shape.ends.is_closed() || (current_index < edges.size() - 1)};
			if (current_t >= 1.f && closed_or_not_last)
				{
				const vec2f point_a{edges.template second_last_point_at<true>(current_index)};
				const vec2f point_b{edges.template last_point_at       <true>(current_index)};
				const vec2f point_c{edges.template second_point_at     <true>(current_index + 1)};
		
				const shape::line line_a{point_a, point_b};
				const shape::line line_b{point_b, point_c};
		
				const float distance_a{line_a.sdf(point).minimum_distance()};
				const float distance_b{line_b.sdf(point).minimum_distance()};
		
				const bool                return_first{distance_a > distance_b};
				const geometry::sdf::side side{(return_first ? line_a : line_b).sdf(point).side()};
				const vec2f               closest{line_a.value_at(current_t)};

				const geometry::sdf::closest_point_with_signed_distance ret{closest, geometry::sdf::signed_distance{current_distance * side}};

				return ret;
				}

			const shape::segment      edge   {edges.ends_aware_access(current_index)};
			const vec2f               closest{edge.value_at          (current_t    )};
			const geometry::sdf::side side   {edge.sdf(point).side()};
			const geometry::sdf::closest_point_with_signed_distance ret{closest, current_distance * side};
			return ret;
			}

		utils_gpu_available constexpr geometry::sdf::side side() const noexcept
			//requires(shape.ends.is_open())
			{
			return closest_with_signed_distance().distance.side();
			}

		utils_gpu_available constexpr geometry::sdf::signed_distance signed_distance() const noexcept
			//requires(shape.ends.is_open())
			{
			return {closest_with_signed_distance().distance};
			}
		};
	}

namespace utils::math::geometry::shape::generic
	{
	template <storage::type storage_type, geometry::ends::closeable ends, size_t extent>
	utils_gpu_available constexpr polyline<storage_type, ends, extent>::sdf_proxy polyline<storage_type, ends, extent>::sdf(const shape::point& point) const noexcept
		{
		return {*this, point};
		}
	}