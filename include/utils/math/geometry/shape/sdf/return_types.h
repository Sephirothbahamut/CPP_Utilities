#pragma once

#include "../../interactions/base_types.h"

namespace utils::math::geometry::sdf
	{
	using namespace interactions::return_types;
	/*
	using interactions::return_types::side;

	struct signed_distance
		{
		float value{std::numeric_limits<float>::infinity()};
		utils_gpu_available constexpr float           absolute () const noexcept { return math::abs( value); }
		utils_gpu_available constexpr side            side     () const noexcept { return          { value}; }
		utils_gpu_available constexpr signed_distance operator-() const noexcept { return          {-value}; }
		utils_gpu_available constexpr auto operator<=>(const signed_distance& other) const noexcept { return value <=> other.value; }
		utils_gpu_available constexpr auto operator== (const signed_distance& other) const noexcept { return value ==  other.value; }

		//operations from https://www.ronja-tutorials.com/post/035-2d-sdf-combination/

		utils_gpu_available static constexpr signed_distance merge(const signed_distance& a, const signed_distance& b) noexcept
			{
			return {utils::math::min(a.value, b.value)};
			}
		utils_gpu_available static constexpr signed_distance intersect(const signed_distance& a, const signed_distance& b) noexcept
			{
			return {utils::math::max(a.value, b.value)};
			}
		utils_gpu_available static constexpr signed_distance subtract(const signed_distance& a, const signed_distance& b) noexcept
			{
			return {intersect(a, -b)};
			}
		utils_gpu_available static constexpr signed_distance lerp(const signed_distance& a, const signed_distance& b, float t) noexcept
			{
			return {utils::math::lerp(a.value, b.value, t)};
			}
		utils_gpu_available static constexpr signed_distance interpolate(const signed_distance& a, const signed_distance& b, float t) noexcept
			{
			return {lerp(a, b, t)};
			}
		utils_gpu_available static constexpr signed_distance round_merge(const signed_distance& a, const signed_distance& b, float radius) noexcept
			{
			const utils::math::vec2f intersection_space
				{
				utils::math::min(a.value - radius, 0.f),
				utils::math::min(b.value - radius, 0.f)
				};
			const float inside_distance{-intersection_space.get_length()};
			const float simple_union{merge(a, b).value};
			const float outside_distance{utils::math::max(simple_union, radius)};
			const float ret{inside_distance + outside_distance};
			return {ret};
			}

		};

	struct closest_point_with_signed_distance;
	struct gradient_signed_distance
		{
		// https://iquilezles.org/articles/distgradfunctions2d/
		signed_distance distance;
		utils::math::vec2f gradient;

		utils_gpu_available static constexpr gradient_signed_distance create(const closest_point_with_signed_distance& closest_point_with_signed_distance, const shape::concepts::point auto& point) noexcept;

		utils_gpu_available static constexpr gradient_signed_distance merge_absolute(const gradient_signed_distance& a, const gradient_signed_distance& b) noexcept
			{
			return a.distance.absolute() < b.distance.absolute() ? a : b;
			}
		utils_gpu_available static constexpr gradient_signed_distance merge(const gradient_signed_distance& a, const gradient_signed_distance& b) noexcept
			{
			return a.distance.value < b.distance.value ? a : b;
			}
		utils_gpu_available static constexpr gradient_signed_distance merge_smooth(const gradient_signed_distance& a, const gradient_signed_distance& b, float smoothness) noexcept
			{
			smoothness *= 4.f;
			const float h{utils::math::max(smoothness - utils::math::abs(a.distance.value - b.distance.value), 0.f)};
			const float m{0.25f * h * h / smoothness};
			const float n{0.50f * h / smoothness};
			const signed_distance ret_distance{utils::math::min(a.distance.value, b.distance.value) - m};
			const vec2f ret_gradient{utils::math::lerp(a.gradient, b.gradient, (a.distance.value < b.distance.value) ? n : 1.f - n)};
			return {ret_distance, ret_gradient};
			}
		};

	struct closest_point_with_signed_distance
		{
		vec2f closest{0.f, 0.f};
		signed_distance distance{utils::math::constants::finf};

		utils_gpu_available static constexpr closest_point_with_signed_distance pick_closest(const closest_point_with_signed_distance& a, const closest_point_with_signed_distance& b) noexcept
			{
			return a.distance.absolute() < b.distance.absolute() ? a : b;
			}
		utils_gpu_available constexpr bool set_to_closest(const closest_point_with_signed_distance& other) noexcept
			{
			if (other.distance.absolute() < distance.absolute()) 
				{
				(*this) = other;
				return true;
				}
			return false;
			}

		utils_gpu_available static constexpr closest_point_with_signed_distance merge(const closest_point_with_signed_distance& a, const closest_point_with_signed_distance& b) noexcept
			{
			return a.distance.value < b.distance.value ? a : b;
			}
		};

	utils_gpu_available constexpr gradient_signed_distance gradient_signed_distance::create(const closest_point_with_signed_distance& closest_point_with_signed_distance, const shape::concepts::point auto& point) noexcept
		{
		return
			{
			closest_point_with_signed_distance.distance,
			(closest_point_with_signed_distance.closest - point).normalize()
			};
		}


	struct closest_point_with_distance
		{
		vec2f closest{0.f, 0.f};
		float distance{utils::math::constants::finf};

		utils_gpu_available static constexpr closest_point_with_distance pick_closest(const closest_point_with_distance& a, const closest_point_with_distance& b) noexcept
			{
			return a.distance < b.distance ? a : b;
			}
		utils_gpu_available constexpr closest_point_with_distance& set_to_closest(const closest_point_with_distance& other) noexcept
			{
			if (other.distance < distance) { (*this) = other; }
			return *this;
			}
		};*/
	}