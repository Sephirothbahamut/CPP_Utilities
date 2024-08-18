#pragma once

#include "base.h"

namespace utils::math::geometry::interactions
	{
	template <ends::ab ends>
	utils_gpu_available constexpr float closest_t(const shape::concepts::ab auto& ab, const vec2f& point)
		{
		return ab.projected_percent<ends>(point);
		}
	utils_gpu_available constexpr float closest_t(const shape::concepts::ab_ends_aware auto& ab, const vec2f& point)
		{
		return ab.projected_percent(point);
		}

	template <ends::ab ends>
	utils_gpu_available constexpr shape::point closest_point(const shape::concepts::ab auto& ab, const vec2f& point) noexcept
		{
		const vec2f delta{ab.b - ab.a};
		const float t{ab.projected_percent(point)};
		if constexpr (ends.is_a_finite()) { if (t <= 0.f) { return ab.a; } }
		if constexpr (ends.is_b_finite()) { if (t >= 1.f) { return ab.b; } }
		return {ab.a.x() + t * delta.x(), ab.a.y() + t * delta.y()};
		}
	utils_gpu_available constexpr auto closest_point(const shape::concepts::ab_ends_aware auto& ab, const vec2f& point) noexcept
		{
		return closest_point<ab.optional_ends.value()>(ab, point);
		}

	template <ends::ab ends>
	utils_gpu_available constexpr float minimum_distance(const shape::concepts::ab auto& ab, const vec2f& point) noexcept
		{
		if constexpr (ends.is_a_finite() || ends.is_b_finite())
			{
			const float t{ab.projected_percent<ends::ab::create::infinite()>(point)};
			if constexpr (ends.is_a_finite()) { if (t <= 0.f) { return vec2f::distance(ab.a, point); } }
			if constexpr (ends.is_b_finite()) { if (t >= 1.f) { return vec2f::distance(ab.b, point); } }
			}
		const auto tmp_0{ab.some_significant_name_ive_yet_to_figure_out(point)};
		const auto tmp_1{ab.a_to_b()};
		const auto tmp_2{tmp_1.get_length()};
		const auto tmp_3{tmp_0 / tmp_2};
		return std::abs(tmp_3);
		}
	utils_gpu_available constexpr auto minimum_distance(const shape::concepts::ab_ends_aware auto& ab, const vec2f& point) noexcept
		{
		return minimum_distance<ab.optional_ends.value()>(ab, point);
		}

	utils_gpu_available constexpr return_types::side side(const shape::concepts::ab auto& ab, const vec2f& point) noexcept
		{
		return {ab.some_significant_name_ive_yet_to_figure_out(point)};
		}

	template <ends::ab ends>
	utils_gpu_available constexpr return_types::signed_distance signed_distance(const shape::concepts::ab auto& ab, const vec2f& point) noexcept
		{
		if constexpr (ends.is_a_finite() || ends.is_b_finite())
			{
			const float t{ab.projected_percent<ends>(point)};
			if constexpr (ends.is_a_finite()) { if (t <= 0.f) { return {vec2f::distance(ab.a, point) * side(ab, point).sign()}; } }
			if constexpr (ends.is_b_finite()) { if (t >= 1.f) { return {vec2f::distance(ab.b, point) * side(ab, point).sign()}; } }
			}
		const auto tmp_0{ab.some_significant_name_ive_yet_to_figure_out(point)};
		const auto tmp_1{ab.a_to_b()};
		const auto tmp_2{tmp_1.get_length()};
		const auto tmp_3{tmp_0 / tmp_2};
		return {tmp_3};
		}
	utils_gpu_available constexpr auto signed_distance(const shape::concepts::ab_ends_aware auto& ab, const vec2f& point) noexcept
		{
		return signed_distance<ab.optional_ends.value()>(ab, point);
		}

	template <ends::ab ends>
	utils_gpu_available constexpr return_types::closest_point_with_distance closest_with_distance(const shape::concepts::ab auto& ab, const vec2f& point) noexcept
		{
		const auto closest{closest_point<ends>(ab, point)};
		return {closest, minimum_distance<ends>(ab, point)};
		}
	utils_gpu_available constexpr auto closest_with_distance(const shape::concepts::ab_ends_aware auto& ab, const vec2f& point) noexcept
		{
		return closest_with_distance<ab.optional_ends.value()>(ab, point);
		}

	template <ends::ab ends>
	utils_gpu_available constexpr return_types::closest_point_with_signed_distance closest_with_signed_distance(const shape::concepts::ab auto& ab, const vec2f& point) noexcept
		{
		const auto closest {closest_point  <ends>(ab, point)};
		const auto distance{signed_distance<ends>(ab, point)};
		return {closest, distance};
		}
	utils_gpu_available constexpr auto closest_with_signed_distance(const shape::concepts::ab_ends_aware auto& ab, const vec2f& point) noexcept
		{
		return closest_with_signed_distance<ab.optional_ends.value()>(ab, point);
		}
	}