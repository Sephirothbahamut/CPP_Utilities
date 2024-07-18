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

		const size_t segments_index_start{polyline.ends.is_a_infinite() ? 1 : 0};
		const size_t segments_index_end  {edges.size() - ((polyline.ends.is_b_infinite() || polyline.ends.is_b_infinite()) ? 1 : 0)};

		edges.for_each([&](const auto& candidate, size_t index)
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

		const shape::line        edge   {edges.ends_aware_access(current_index)};
		const shape::point       closest{edge.value_at          (current_t    )};
		const return_types::side side   {interactions::side     (edge, point  )};
		const return_types::closest_point_with_signed_distance ret{closest, current_distance * side};
		return ret;
		}


	//utils_gpu_available constexpr return_types::closest_point_with_signed_distance closest_with_signed_distance(const shape::concepts::polyline auto& polyline, const vec2f& point) noexcept
	//	//requires(polyline.ends.is_closed())
	//	{
	//	float current_distance{utils::math::constants::finf};
	//	float current_t{0.f};
	//	size_t current_index{0};
	//
	//
	//	const size_t segments_index_start{                               polyline.ends.is_a_infinite() ? 1 : 0};
	//	const size_t segments_index_end  {polyline.vertices.size() - 1 - polyline.ends.is_b_infinite() ? 1 : 0};
	//
	//	for (size_t index{segments_index_start}; index < segments_index_end; index++)
	//		{
	//		const shape::segment candidate{polyline.vertices[index], polyline.vertices[index + 1]};
	//		float candidate_t{interactions::closest_t(candidate, point)};
	//		float candidate_distance{interactions::minimum_distance(candidate, point)};
	//		if (candidate_distance < current_distance)
	//			{
	//			current_t = candidate_t;
	//			current_distance = candidate_distance;
	//			current_index = index;
	//			}
	//		}
	//
	//	if constexpr (polyline.ends.is_closed())
	//		{
	//		const size_t index{polyline.vertices.size() - 1};
	//		const shape::segment candidate{polyline.vertices[index], polyline.vertices[0]};
	//		float candidate_t{interactions::closest_t(candidate, point)};
	//		float candidate_distance{interactions::minimum_distance(candidate, point)};
	//		if (candidate_distance < current_distance || (candidate_distance == current_distance && current_index == 0))
	//			{
	//			current_t = candidate_t;
	//			current_distance = candidate_distance;
	//			current_index = index;
	//			}
	//		}
	//
	//	assert(current_t >= 0.f);
	//	if (current_t < 1.f || (polyline.ends.is_open() && current_index == polyline.vertices.size() - 1))
	//		{
	//		const size_t index_b{polyline.ends.is_closed() ? polyline.vertices.ends_aware_index(current_index + 1) : (current_index + 1)};
	//		const shape::segment piece{polyline.vertices[current_index], polyline.vertices[index_b]};
	//		const auto ret{interactions::closest_with_signed_distance(piece, point)};
	//		return ret;
	//		}
	//	else
	//		{
	//		const size_t       index_a {polyline.ends.is_closed() ? polyline.vertices.ends_aware_index(current_index    ) : (current_index    )};
	//		const size_t       index_b {polyline.ends.is_closed() ? polyline.vertices.ends_aware_index(current_index + 1) : (current_index + 1)};
	//		const size_t       index_c {polyline.ends.is_closed() ? polyline.vertices.ends_aware_index(current_index + 2) : (current_index + 2)};
	//		const shape::point vertex_a{polyline.vertices[index_a]};
	//		const shape::point vertex_b{polyline.vertices[index_b]};
	//		const shape::point vertex_c{polyline.vertices[index_c]};
	//
	//		const shape::line first {vertex_a, vertex_b};
	//		const shape::line second{vertex_b, vertex_c};
	//
	//		const return_types::signed_distance signed_distance_first {interactions::signed_distance(first , point)};
	//		const return_types::signed_distance signed_distance_second{interactions::signed_distance(second, point)};
	//
	//		const bool return_first{signed_distance_first.absolute() > signed_distance_second.absolute()};
	//		const auto side{return_first ? signed_distance_first.side() : signed_distance_second.side()};
	//		return return_types::closest_point_with_signed_distance{vertex_b, return_types::signed_distance{current_distance * side}};
	//		}
	//	}

/*	utils_gpu_available constexpr return_types::closest_point_with_signed_distance closest_with_signed_distance(const shape::concepts::polyline auto& polyline, const vec2f& point) noexcept
		requires(polyline.ends.is_open())
		{
		return_types::closest_point_with_signed_distance ret;

		bool closest_is_piece_end{false};
		size_t closest_index{0};

		const auto pieces{polyline.get_edges()};

		size_t index{0};
		pieces.for_each([&](const auto& piece)
			{
			const auto candidate{closest_with_signed_distance(piece, point)};
			const auto candidate_distance{piece.distance.absolute()};
			const auto current_distance{ret.distance.absolute()};
			if (candidate_distance < current_distance)
				{
				closest_index = index;
				closest_is_piece_end = candidate == piece.end_vertex();
				ret = candidate;
				}

			index++;
			});

		if (closest_is_piece_end)
			{

			}

		//if constexpr (polyline.ends.is_closed())
		//	{
		//	if (closest_is_piece_end && ret.closest == polyline.vertices[0])
		//		{
		//		closest_index = pieces.size() - 1;
		//		}
		//	}
		//
		//if (closest_is_piece_end)
		//	{
		//
		//
		//	}
		return ret;

		//const auto edges{polyline.get_edges()};
		//if (edges.empty()) { return {}; }
		//
		//return_types::closest_point_with_signed_distance ret;
		//
		//// First edge, special case for infinite start polylines
		//if constexpr (polyline.ends.is_a_infinite())
		//	{
		//	const shape::reverse_ray start_ray{edges[0].a, edges[0].b};
		//	ret = closest_with_signed_distance(start_ray, point);
		//	}
		//else
		//	{
		//	const auto candidate_edge{edges[0]};
		//	ret = closest_with_signed_distance(candidate_edge, point);
		//	}
		//
		//size_t closest_index{0};
		//
		//for (size_t i{1}; i < edges.size() - 1; i++)
		//	{
		//	const auto candidate_edge{edges[i]};
		//	const auto candidate{closest_with_signed_distance(candidate_edge, point)};
		//	const auto candidate_distance{candidate.distance.absolute()};
		//	const auto current_distance  {ret.distance.absolute()};
		//	if (candidate_distance < current_distance)
		//		{
		//		ret = candidate;
		//		closest_index = i;
		//		}
		//	}
		//
		//const size_t last_edge_index{edges.size() - 1};
		//using last_edge_t = std::conditional_t<polyline.ends.is_b_infinite(), shape::ray, shape::segment>;
		//const last_edge_t last_edge{edges[last_edge_index]};
		//const auto last_candidate{closest_with_signed_distance(last_edge, point)};
		//
		//// in closed shapes if first and last are equidistant to the point, use the last edge as closest
		//// so we have the correct first-second edge pair for the corner case evaluated later 
		//// to achieve that, the previous edges check for distance.absolute() <, while the last one checks for <=, so it can replace the closest index
		//// (getting correct side when two edges are equidistant from the point)
		//
		//const bool last_replace{[&]()
		//	{
		//	if constexpr (polyline.ends.is_closed())
		//		{
		//		return last_candidate.distance.absolute() <= ret.distance.absolute();
		//		}
		//	else
		//		{
		//		return last_candidate.distance.absolute() < ret.distance.absolute();
		//		}
		//	}()};
		//
		//if (last_replace)
		//	{
		//	ret = last_candidate;
		//	closest_index = last_edge_index;
		//	}
		//
		//const bool closed_or_not_last{polyline.ends.is_closed() || closest_index != edges.size() - 1};
		//
		//if (closed_or_not_last)
		//	{
		//	const auto end_of_closest{edges[closest_index].b};
		//	const bool closest_matches_b{ret.closest == end_of_closest};
		//	if (closest_matches_b)
		//		{
		//		//Common closest means the closest is a corner, we have to check for the weird regions (see "side_corner_case_visualization.png")
		//		//pick the farthest infinite line (not segment) to get the correct side
		//		const shape::line line_previous{edges[closest_index                                            ]};
		//		const shape::line line_next    {edges[polyline.raw_index_to_ends_aware_index(closest_index + 1)]};
		//
		//		// Angle based version
		//		// Slower
		//		//const auto tangent_previous_from_end_backwards{-line_previous.tangent_to  ()};
		//		//const auto tangent_next_from_begin_forward    {-line_next    .tangent_from()};
		//		//const auto angle_from{tangent_previous_from_end_backwards.angle()};
		//		//const auto angle_to  {tangent_next_from_begin_forward    .angle()};
		//		//
		//		//const auto point_angle{(point - ret.closest).angle()};
		//		//
		//		//const auto side{point_angle.within(angle_from, angle_to) ? 1.f : -1.f};
		//
		//		// Infinite distance version
		//		const return_types::signed_distance line_signed_distance_previous{signed_distance(line_previous, point)};
		//		const return_types::signed_distance line_signed_distance_next    {signed_distance(line_next    , point)};
		//		
		//		const auto side{line_signed_distance_previous.absolute() > line_signed_distance_next.absolute() ? line_signed_distance_previous.side() : line_signed_distance_next.side()};
		//		
		//		ret.distance = {ret.distance.absolute() * side};
		//		}
		//	}
		// return ret;
		}*/

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