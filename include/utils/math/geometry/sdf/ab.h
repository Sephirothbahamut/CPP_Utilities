#pragma once

#include "common.h"
#include "../shape/ab.h"

namespace utils::math::geometry::shape::generic
	{
	template <storage::type storage_type, geometry::ends::optional_ab optional_ends>
	struct ab<storage_type, optional_ends>::sdf_proxy
		{
		using shape_t = ab<storage_type, optional_ends>;

		sdf_proxy(const shape_t& shape, const vec2f& point) : shape{shape}, point{point} {};
		const shape_t& shape;
		const vec2f point;

		utils_gpu_available constexpr geometry::sdf::direction_signed_distance direction_signed_distance() const noexcept
			{
			const auto closest_with_signed_distance_value{closest_with_signed_distance()};
			return geometry::sdf::direction_signed_distance::create(closest_with_signed_distance_value, point);
			}

		template <ends::ab ends>
		utils_gpu_available constexpr float closest_t() const noexcept
			{
			return shape.template projected_percent<ends>(point);
			}
		utils_gpu_available constexpr float closest_t() const noexcept
			requires (shape::concepts::ab_ends_aware<shape_t>)
			{
			return shape.template projected_percent<shape.optional_ends.value()>(point);
			}

		template <ends::ab ends>
		utils_gpu_available constexpr vec2f closest_point() const noexcept
			{
			const vec2f delta{shape.b - shape.a};
			const float t{shape.template projected_percent<ends::ab::create::infinite()>(point)};
			if constexpr (ends.is_a_finite()) { if (t <= 0.f) { return shape.a; } }
			if constexpr (ends.is_b_finite()) { if (t >= 1.f) { return shape.b; } }
			return {shape.a.x() + t * delta.x(), shape.a.y() + t * delta.y()};
			}
		utils_gpu_available constexpr auto closest_point() const noexcept
			requires(shape::concepts::ab_ends_aware<shape_t>)
			{
			return closest_point<shape.optional_ends.value()>();
			}

		template <ends::ab ends>
		utils_gpu_available constexpr float minimum_distance() const noexcept
			{
			if constexpr (ends.is_a_finite() || ends.is_b_finite())
				{
				const float t{shape.template projected_percent<ends::ab::create::infinite()>(point)};
				if constexpr (ends.is_a_finite()) { if (t <= 0.f) { return vec2f::distance(shape.a, point); } }
				if constexpr (ends.is_b_finite()) { if (t >= 1.f) { return vec2f::distance(shape.b, point); } }
				}
			const auto tmp_0{shape.some_significant_name_ive_yet_to_figure_out(point)};
			const auto tmp_1{shape.a_to_b()};
			const auto tmp_2{tmp_1.get_length()};
			const auto tmp_3{tmp_0 / tmp_2};
			return std::abs(tmp_3);
			}
		utils_gpu_available constexpr auto minimum_distance() const noexcept
			requires(shape::concepts::ab_ends_aware<shape_t>)
			{
			#ifdef __INTELLISENSE__
			//Intellisense doesn't see the above templated function for some reason
			return float{0.f};
			#else
			return minimum_distance<shape.optional_ends.value()>();
			#endif
			}

		utils_gpu_available constexpr geometry::sdf::side side() const noexcept
			{
			return {shape.some_significant_name_ive_yet_to_figure_out(point)};
			}

		utils_gpu_available constexpr bool intersects_for_closed_shape_pieces()  const noexcept
			{//https://www.shadertoy.com/view/wdBXRW
			utils::math::vec2f e{shape.b - shape.a};
			utils::math::vec2f w{point   - shape.a};

			const bool cond_a{point.y() >= shape.a.y()};
			const bool cond_b{point.y() <  shape.b.y()};
			const bool cond_c{e.x() * w.y() > e.y() * w.x()};
			return (cond_a && cond_b && cond_c) || (!cond_a && !cond_b && !cond_c);
			}

		template <ends::ab ends>
		utils_gpu_available constexpr geometry::sdf::signed_distance signed_distance() const noexcept
			{
			if constexpr (ends.is_a_finite() || ends.is_b_finite())
				{
				const float t{shape.template projected_percent<ends>(point)};
				if constexpr (ends.is_a_finite()) { if (t <= 0.f) { return {vec2f::distance(shape.a, point) * side().sign()}; } }
				if constexpr (ends.is_b_finite()) { if (t >= 1.f) { return {vec2f::distance(shape.b, point) * side().sign()}; } }
				}
			const auto tmp_0{shape.some_significant_name_ive_yet_to_figure_out(point)};
			const auto tmp_1{shape.a_to_b()};
			const auto tmp_2{tmp_1.get_length()};
			const auto tmp_3{tmp_0 / tmp_2};
			return {tmp_3};
			}
		utils_gpu_available constexpr auto signed_distance() const noexcept
			requires(shape::concepts::ab_ends_aware<shape_t>)
			{
			return signed_distance<shape.optional_ends.value()>();
			}

		template <ends::ab ends>
		utils_gpu_available constexpr geometry::sdf::closest_point_with_distance closest_with_distance() const noexcept
			{
			const auto closest{closest_point<ends>()};
			return {closest, minimum_distance<ends>()};
			}
		utils_gpu_available constexpr auto closest_with_distance() const noexcept
			requires(shape::concepts::ab_ends_aware<shape_t>)
			{
			return closest_with_distance<shape.optional_ends.value()>();
			}

		template <ends::ab ends>
		utils_gpu_available constexpr geometry::sdf::closest_point_with_signed_distance closest_with_signed_distance() const noexcept
			{
			const auto closest {closest_point  <ends>()};
			const auto distance{signed_distance<ends>()};
			return {closest, distance};
			}
		utils_gpu_available constexpr auto closest_with_signed_distance() const noexcept
			requires(shape::concepts::ab_ends_aware<shape_t>)
			{
			return closest_with_signed_distance<shape.optional_ends.value()>();
			}

		/// <summary> 
		/// Since this is meant to be used exclusively for closed shapes, ab is assumed to be a segment in any case.
		/// For open shapes, use regular side
		/// </summary>
		utils_gpu_available constexpr bool closed_piece_side_check() const noexcept
			{
			const utils::math::vec2f e{shape.b - shape.a};
			const utils::math::vec2f w{point   - shape.a};
			const std::array<bool, 3> conditions
				{
				point.y() >= shape.a.y(),
				point.y() <  shape.b.y(),
				e.x() * w.y() > e.y() * w.x()
				};
			return ((conditions[0] && conditions[1] && conditions[2]) || (!conditions[0] && !conditions[1] && !conditions[2]));
			}
		};
	}

namespace utils::math::geometry::shape::generic
	{
	template <storage::type storage_type, geometry::ends::optional_ab optional_ends>
	utils_gpu_available constexpr ab<storage_type, optional_ends>::sdf_proxy ab<storage_type, optional_ends>::sdf(const shape::point& point) const noexcept
		{
		return {*this, point};
		}
	}