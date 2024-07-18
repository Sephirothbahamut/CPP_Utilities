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
	
	//utils_gpu_available constexpr return_types::closest_point_with_signed_distance closest_with_signed_distance(const shape::concepts::mixed auto& mixed, const vec2f& point) noexcept
	//	{
	//	return_types::closest_point_with_signed_distance ret;
	//	size_t closest_index{0};
	//
	//	mixed.for_each([&](const auto& piece)
	//		{
	//		const auto candidate{closest_with_signed_distance(candidate_edge, point)};
	//		const auto candidate_distance{}
	//		});
	//
	//	//const auto pieces{polyline.get_pieces()};
	//	//if (pieces.empty()) { return {}; }
	//	//
	//	//return_types::closest_point_with_signed_distance ret;
	//	//
	//	//size_t closest_index{0};
	//	//
	//	//for (size_t i{0}; i < edges.size() - 1; i++)
	//	//	{
	//	//	const auto candidate_piece{pieces[i]};
	//	//	const auto candidate{closest_with_signed_distance(candidate_edge, point)};
	//	//	const auto candidate_distance{candidate.distance.absolute()};
	//	//	const auto current_distance  {ret.distance.absolute()};
	//	//	if (candidate_distance < current_distance)
	//	//		{
	//	//		ret = candidate;
	//	//		closest_index = i;
	//	//		}
	//	//	}
	//	//
	//	//const size_t last_edge_index{edges.size() - 1};
	//	//using last_edge_t = std::conditional_t<polyline.ends.is_b_infinite(), shape::ray, shape::segment>;
	//	//const last_edge_t last_edge{edges[last_edge_index]};
	//	//const auto last_candidate{closest_with_signed_distance(last_edge, point)};
	//	//
	//	//// in closed shapes if first and last are equidistant to the point, use the last edge as closest
	//	//// so we have the correct first-second edge pair for the corner case evaluated later 
	//	//// to achieve that, the previous edges check for distance.absolute() <, while the last one checks for <=, so it can replace the closest index
	//	//// (getting correct side when two edges are equidistant from the point)
	//	//
	//	//const bool last_replace{[&]()
	//	//	{
	//	//	if constexpr (polyline.ends.is_closed())
	//	//		{
	//	//		return last_candidate.distance.absolute() <= ret.distance.absolute();
	//	//		}
	//	//	else
	//	//		{
	//	//		return last_candidate.distance.absolute() < ret.distance.absolute();
	//	//		}
	//	//	}()};
	//	//
	//	//if (last_replace)
	//	//	{
	//	//	ret = last_candidate;
	//	//	closest_index = last_edge_index;
	//	//	}
	//	//
	//	//const bool closed_or_not_last{polyline.ends.is_closed() || closest_index != edges.size() - 1};
	//	//
	//	//if (closed_or_not_last)
	//	//	{
	//	//	const auto end_of_closest{edges[closest_index].b};
	//	//	const bool closest_matches_b{ret.closest == end_of_closest};
	//	//	if (closest_matches_b)
	//	//		{
	//	//		//Common closest means the closest is a corner, we have to check for the weird regions (see "side_corner_case_visualization.png")
	//	//		//pick the farthest infinite line (not segment) to get the correct side
	//	//		const shape::line line_previous{edges[closest_index                                            ]};
	//	//		const shape::line line_next    {edges[polyline.raw_index_to_ends_aware_index(closest_index + 1)]};
	//	//
	//	//		// Angle based version
	//	//		// Slower
	//	//		//const auto tangent_previous_from_end_backwards{-line_previous.tangent_to  ()};
	//	//		//const auto tangent_next_from_begin_forward    {-line_next    .tangent_from()};
	//	//		//const auto angle_from{tangent_previous_from_end_backwards.angle()};
	//	//		//const auto angle_to  {tangent_next_from_begin_forward    .angle()};
	//	//		//
	//	//		//const auto point_angle{(point - ret.closest).angle()};
	//	//		//
	//	//		//const auto side{point_angle.within(angle_from, angle_to) ? 1.f : -1.f};
	//	//
	//	//		// Infinite distance version
	//	//		const return_types::signed_distance line_signed_distance_previous{signed_distance(line_previous, point)};
	//	//		const return_types::signed_distance line_signed_distance_next    {signed_distance(line_next    , point)};
	//	//		
	//	//		const auto side{line_signed_distance_previous.absolute() > line_signed_distance_next.absolute() ? line_signed_distance_previous.side() : line_signed_distance_next.side()};
	//	//		
	//	//		ret.distance = {ret.distance.absolute() * side};
	//	//		}
	//	//	}
	//	//
	//	//return ret;
	//	}
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