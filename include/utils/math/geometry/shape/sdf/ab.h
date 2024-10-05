#pragma once
#include "return_types.h"
#include "../ab.h"

namespace utils::math::geometry::shape::generic
	{
	template <storage::type storage_type, geometry::ends::optional_ab optional_ends>
	struct ab<storage_type, optional_ends>::sdf_proxy
		{
		using shape_t = ab<storage_type, optional_ends>;
		#include "common.inline.h"

		template <ends::ab ends>
		utils_gpu_available constexpr float closest_t() const noexcept
			{
			return shape.projected_percent<ends>(point);
			}
		utils_gpu_available constexpr float closest_t() const noexcept
			requires (shape::concepts::ab_ends_aware<shape_t>)
			{
			return shape.projected_percent(point);
			}

		template <ends::ab ends>
		utils_gpu_available constexpr vec2f closest_point() const noexcept
			{
			const vec2f delta{shape.b - shape.a};
			const float t{shape.projected_percent(point)};
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
				const float t{shape.projected_percent<ends::ab::create::infinite()>(point)};
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
			return minimum_distance<shape.optional_ends.value()>();
			}

		utils_gpu_available constexpr geometry::sdf::side side() const noexcept
			{
			return {shape.some_significant_name_ive_yet_to_figure_out(point)};
			}

		template <ends::ab ends>
		utils_gpu_available constexpr geometry::sdf::signed_distance signed_distance() const noexcept
			{
			if constexpr (ends.is_a_finite() || ends.is_b_finite())
				{
				const float t{shape.projected_percent<ends>(point)};
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
		};
	}

namespace utils::math::geometry::shape::generic
	{
	template <storage::type storage_type, geometry::ends::optional_ab optional_ends>
	ab<storage_type, optional_ends>::sdf_proxy ab<storage_type, optional_ends>::sdf(const shape::point& point) const noexcept
		{
		return {*this, point};
		}
	}