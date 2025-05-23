#pragma once

#include "../../../concepts.h"
#include "../../../compilation/gpu.h"

#include "../../math.h"
#include "../../angle.h"
#include "../../transform2.h"

#include "../shape/point.h"
#include "../shape/aabb.h"

#include "../../../details/warnings_pre.inline.h"

namespace utils::math::geometry::sdf
	{
	class side
		{
		public:
			constexpr side() = default; // No need to specify utils_gpu_available for defaults
			utils_gpu_available constexpr side(float value) : _value{value < -math::constants::epsilonf ? -1.f : value > math::constants::epsilonf ? 1.f : 0.f} {}

			struct create : ::utils::oop::non_constructible
				{
				utils_gpu_available static consteval side left      () noexcept { return {-1.f}; }
				utils_gpu_available static consteval side right     () noexcept { return { 1.f}; }
				utils_gpu_available static consteval side coincident() noexcept { return { 0.f}; }

				utils_gpu_available static consteval side inside () noexcept { return left (); }
				utils_gpu_available static consteval side outside() noexcept { return right(); }
				};

			utils_gpu_available constexpr bool is_left      (float epsilon = 0.f) const noexcept { return _value <  -epsilon; }
			utils_gpu_available constexpr bool is_coincident(float epsilon = 0.f) const noexcept { return _value >= -epsilon && _value <= epsilon; }
			utils_gpu_available constexpr bool is_right     (float epsilon = 0.f) const noexcept { return _value >   epsilon; }

			utils_gpu_available constexpr bool is_outside(float epsilon = 0.f) const noexcept { return  is_right  (epsilon); }
			utils_gpu_available constexpr bool is_inside (float epsilon = 0.f) const noexcept { return !is_outside(epsilon); }

			utils_gpu_available constexpr bool is_exactly_left      () const noexcept { return is_left(0.f); }
			utils_gpu_available constexpr bool is_exactly_coincident() const noexcept { return is_coincident(0.f); }
			utils_gpu_available constexpr bool is_exactly_right     () const noexcept { return is_right(0.f); }
			utils_gpu_available constexpr bool is_approx_left       () const noexcept { return is_left(utils::math::constants::epsilonf); }
			utils_gpu_available constexpr bool is_approx_coincident () const noexcept { return is_coincident(utils::math::constants::epsilonf); }
			utils_gpu_available constexpr bool is_approx_right      () const noexcept { return is_right(utils::math::constants::epsilonf); }

			utils_gpu_available constexpr bool is_exactly_inside () const noexcept { return is_inside (0.f); }
			utils_gpu_available constexpr bool is_exactly_outside() const noexcept { return is_outside(0.f); }
			utils_gpu_available constexpr bool is_approx_inside  () const noexcept { return is_inside (utils::math::constants::epsilonf); }
			utils_gpu_available constexpr bool is_approx_outside () const noexcept { return is_outside(utils::math::constants::epsilonf); }

			
			utils_gpu_available constexpr float value    () const noexcept { return   _value; }
			/// <summary> Returns non-zero value.</summary>
			utils_gpu_available constexpr float sign     () const noexcept { return _value < 0.f ? -1.f : 1.f; }
			utils_gpu_available constexpr side  operator-() const noexcept { return {-_value}; }

		private:
			float _value{0.f};
		};
	utils_gpu_available constexpr float  operator* (const float& f, const side& side) noexcept { return f * side.sign(); }
	utils_gpu_available constexpr float& operator*=(      float& f, const side& side) noexcept { return f = f * side.sign(); }
	utils_gpu_available constexpr bool   operator==(const side & a, const side& b   ) noexcept { return (a.is_left() && b.is_left()) || (a.is_right() && b.is_right()) || (a.is_approx_coincident() && b.is_approx_coincident()); }
	utils_gpu_available constexpr bool   operator==(const float& f, const side& side) noexcept { return (math::sign(f) == math::sign(side.value())) || (f == 0.f && side.value() == 0.f); }

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
	struct direction_signed_distance
		{
		// https://iquilezles.org/articles/distgradfunctions2d/
		signed_distance distance;
		utils::math::vec2f direction;

		utils_gpu_available static constexpr direction_signed_distance create(const closest_point_with_signed_distance& closest_point_with_signed_distance, const shape::concepts::point auto& point) noexcept;

		utils_gpu_available static constexpr direction_signed_distance merge_absolute(const direction_signed_distance& a, const direction_signed_distance& b) noexcept
			{
			return a.distance.absolute() < b.distance.absolute() ? a : b;
			}
		utils_gpu_available static constexpr direction_signed_distance merge(const direction_signed_distance& a, const direction_signed_distance& b) noexcept
			{
			return a.distance.value < b.distance.value ? a : b;
			}
		utils_gpu_available static constexpr direction_signed_distance merge_smooth(const direction_signed_distance& a, const direction_signed_distance& b, float smoothness) noexcept
			{
			smoothness *= 4.f;
			const float h{utils::math::max(smoothness - utils::math::abs(a.distance.value - b.distance.value), 0.f)};
			const float m{0.25f * h * h / smoothness};
			const float n{0.50f * h / smoothness};
			const signed_distance ret_distance{utils::math::min(a.distance.value, b.distance.value) - m};
			const vec2f ret_direction{utils::math::lerp(a.direction, b.direction, (a.distance.value < b.distance.value) ? n : 1.f - n)};
			return {ret_distance, ret_direction};
			}

		utils_gpu_available constexpr direction_signed_distance& merge_self(const direction_signed_distance& other) noexcept
			{
			*this = merge(*this, other);
			return *this;
			}
		utils_gpu_available constexpr direction_signed_distance& merge_self_absolute(const direction_signed_distance& other) noexcept
			{
			*this = merge_absolute(*this, other);
			return *this;
			}
		utils_gpu_available constexpr direction_signed_distance& merge_self_smooth(const direction_signed_distance& other, float smoothness) noexcept
			{
			*this = merge_smooth(*this, other, smoothness);
			return *this;
			}

		utils_gpu_available constexpr direction_signed_distance  operator-() const noexcept { return {-distance, direction}; }//TODO should invert direction direction?
		utils_gpu_available constexpr direction_signed_distance& operator-=(float value) noexcept { *this = *this - value; return *this; }
		utils_gpu_available constexpr direction_signed_distance  operator-(float value) const noexcept 
			{
			const auto new_distance{distance.value - value};
			const auto new_direction{std::signbit(distance.value) != std::signbit(new_distance) ? -direction : direction};
			return {new_distance, new_direction};
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

	utils_gpu_available constexpr direction_signed_distance direction_signed_distance::create(const closest_point_with_signed_distance& closest_point_with_signed_distance, const shape::concepts::point auto& point) noexcept
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
		};
	}

#include "../../../details/warnings_post.inline.h"