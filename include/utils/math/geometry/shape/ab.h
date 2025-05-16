#pragma once

#include "declaration/ab.h"

namespace utils::math::geometry::shape::generic
	{
	template <storage::type STORAGE_TYPE, geometry::ends::optional_ab OPTIONAL_ENDS>
	struct utils_oop_empty_bases ab : geometry::piece_flag, geometry::shape_flag
		{
		inline static constexpr auto storage_type {STORAGE_TYPE };
		inline static constexpr auto optional_ends{OPTIONAL_ENDS};

		using self_t                = ab<storage_type, optional_ends>;
		using owner_self_t          = ab<storage::type::create::owner         (), optional_ends>;
		using observer_self_t       = ab<storage::type::create::observer      (), optional_ends>;
		using const_observer_self_t = ab<storage::type::create::const_observer(), optional_ends>;

		const_observer_self_t create_observer() const noexcept { return {*this}; }
		      observer_self_t create_observer()       noexcept { return {*this}; }

		using vertex_t = generic::point<storage_type>;
		template <bool is_function_const>
		using vertex_observer = generic::point<storage::type::create::observer(is_function_const)>;

		utils_gpu_available constexpr ab() noexcept = default;

		utils_gpu_available constexpr ab(const utils::math::vec2f& a, const utils::math::vec2f& b) noexcept
			requires(storage_type.can_construct_from_const()) : 
			a{a}, b{b} {}
			
		utils_gpu_available constexpr ab(utils::math::vec2f& a, utils::math::vec2f& b) noexcept :
			a{a}, b{b} {}

		utils_gpu_available constexpr ab(concepts::ab auto& other) noexcept
			requires(storage::constness_matching<self_t, decltype(other)>::compatible_constness) :
			a{other.a}, b{other.b} {}

		utils_gpu_available constexpr ab(const concepts::ab auto& other) noexcept
			requires(storage_type.can_construct_from_const()) :
			a{other.a}, b{other.b} {}

		vertex_t a;
		vertex_t b;

		utils_gpu_available constexpr const vertex_t closest_vertex(const concepts::point auto& other) const noexcept { return shape::point::distance2(other, a) < shape::point::distance2(other, b) ? a : b; }
		utils_gpu_available constexpr       vertex_t closest_vertex(      concepts::point auto& other)       noexcept { return shape::point::distance2(other, a) < shape::point::distance2(other, b) ? a : b; }

		template <ends::ab ends>
		utils_gpu_available constexpr float length2() const noexcept { return ends.is_finite() ? shape::point::distance2(a, b) : utils::math::constants::finf; }
		template <ends::ab ends>
		utils_gpu_available constexpr float length () const noexcept { return ends.is_finite() ? shape::point::distance (a, b) : utils::math::constants::finf; }
				
		utils_gpu_available constexpr float length2() const noexcept requires(optional_ends.has_value()) { return length2<optional_ends.value()>(); }
		utils_gpu_available constexpr float length () const noexcept requires(optional_ends.has_value()) { return length <optional_ends.value()>(); }

		/// <summary> Vector from a towards b. </summary>
		utils_gpu_available constexpr vec2f a_to_b() const noexcept { return b - a; }
		/// <summary> Vector from a towards b. </summary>
		utils_gpu_available constexpr vec2f b_to_a() const noexcept { return a - b; }
		/// <summary> Unit vector from a towards b. </summary>
		utils_gpu_available constexpr vec2f forward() const noexcept { return a_to_b().normalize(); }
		/// <summary> Unit vector from a towards b. </summary>
		utils_gpu_available constexpr vec2f backward() const noexcept { return b_to_a().normalize(); }
		/// <summary> Unit vector perpendicular on the left from a to b. </summary>
		utils_gpu_available constexpr vec2f perpendicular_right() const noexcept { return forward().perpendicular_right(); }
		/// <summary> Unit vector perpendicular on the right from a to b. </summary>
		utils_gpu_available constexpr vec2f perpendicular_left () const noexcept { return forward().perpendicular_left (); }

		/// <summary> Projecting point to the line that goes through a-b, at what percentage of the segment a-b it lies. < 0 is before a, > 1 is after b, proportionally to the a-b distance </summary>
		template <ends::ab ends>
		utils_gpu_available constexpr float projected_percent(const concepts::point auto& point) const noexcept
			{
			//from shadertoy version, mathematically equivalent I think maybe perhaps, idk, i'm not into maths
			//const utils::math::vec2f b_a{b   - a};
			//const utils::math::vec2f p_a{point - a};
			//return utils::math::vec2f::dot(p_a, b_a) / utils::math::vec2f::dot(b_a, b_a);

			//previous version, mathematically equivalent I think maybe perhaps, idk, i'm not into maths
			//http://csharphelper.com/blog/2016/09/find-the-shortest-distance-between-a-point-and-a-line-segment-in-c/
			const vec2f delta{b - a};
			const auto ret{((point.x() - a.x()) * delta.x() + (point.y() - a.y()) * delta.y()) / (delta.x() * delta.x() + delta.y() * delta.y())};
			if constexpr (ends.is_a_finite()) { if (ret < 0.f) { return 0.f; } }
			if constexpr (ends.is_b_finite()) { if (ret > 1.f) { return 1.f; } }
			return ret;
			}
		utils_gpu_available constexpr float projected_percent(const concepts::point auto& point) const noexcept
			requires(optional_ends.has_value()) 
			{
			return projected_percent<optional_ends.value()>(point); 
			}

		utils_gpu_available constexpr float some_significant_name_ive_yet_to_figure_out(const concepts::point auto& point) const noexcept
			{
			//signed distance from line in proportion to the distance between a and b, idk, i'm not a math guy
			//enough alone to get the sign for side, but needs to be divided by (a-b).length to get the signed distance
			const float ret{((b.x() - a.x()) * (point.y() - a.y())) - ((point.x() - a.x()) * (b.y() - a.y()))};

			//Note: it's not the dot product
			//const auto a_to_point(point - a);
			//const float dot{utils::math::vec2f::dot(a_to_b(), a_to_point)};

			return -ret;
			}

		utils_gpu_available constexpr vec2f value_at(float t) const noexcept { return utils::math::lerp(a, b, t); }

		template <ends::ab ends>
		utils_gpu_available constexpr vec2f closest_point_at(float t) const noexcept 
			{
			if constexpr (ends.is_a_finite()) { if (t <= 0.f) { return a; } }
			if constexpr (ends.is_b_finite()) { if (t >= 1.f) { return b; } }
			return value_at(t);
			}
		utils_gpu_available constexpr vec2f closest_point_at(float t) const noexcept 
			requires(optional_ends.has_value()) 
			{
			return closest_point_at<optional_ends.value()>(t); 
			}

		struct sdf_proxy;
		utils_gpu_available constexpr sdf_proxy sdf(const vec<float, 2>& point) const noexcept;

		utils_gpu_available constexpr auto bounding_box() const noexcept;
		utils_gpu_available constexpr auto bounding_circle() const noexcept;
		#include "../transform/common_declaration.inline.h"
		};
	}

static_assert(utils::math::geometry::shape::concepts::ab
	<
	utils::math::geometry::shape::ab<>
	>);
static_assert(utils::math::geometry::shape::concepts::shape
	<
	utils::math::geometry::shape::ab<>
	>);







///////// Old interactions, kept for reference for intersections with other edges
//namespace utils::math::geometry::interactions
//	{
//	namespace return_types
//		{
//		struct percentages
//			{
//			float b_to_reach_a{utils::math::constants::finf};
//			float a_to_reach_b{utils::math::constants::finf};
//			};
//		}
//
//	return_types::percentages percentages(const shape::concepts::ab auto& ab_a, const shape::concepts::ab auto& ab_b) noexcept
//		{//https://stackoverflow.com/questions/563198/how-do-you-detect-where-two-line-segments-intersect
//		const vec2f s1{ab_a.a_to_b()};
//		const vec2f s2{ab_b.a_to_b()};
//	
//		const float b_percentage_to_reach_a{(-s1.y() * (ab_a.a.x() - ab_b.a.x()) + s1.x() * (ab_a.a.y() - ab_b.a.y())) / (-s2.x() * s1.y() + s1.x() * s2.y())};
//		const float a_percentage_to_reach_b{( s2.x() * (ab_a.a.y() - ab_b.a.y()) - s2.y() * (ab_a.a.x() - ab_b.a.x())) / (-s2.x() * s1.y() + s1.x() * s2.y())};
//		
//		return {b_percentage_to_reach_a, a_percentage_to_reach_b};
//		}
//
//	utils_gpu_available constexpr bool intersects(const shape::concepts::line auto& a, const shape::concepts::line auto& b) noexcept
//		{
//		return a.forward().angle() != b.forward().angle();
//		}
//	
//	template <shape::concepts::ab a_t, shape::concepts::ab b_t>
//	utils_gpu_available constexpr bool intersects(const a_t& a, const b_t& b) noexcept
//		{
//		const auto percents{percentages(a, b)};
//		if constexpr (a_t::ends.is_a_finite()) { if (percents.a_to_reach_b < 0.f) { return false; } }
//		if constexpr (a_t::ends.is_b_finite()) { if (percents.a_to_reach_b > 1.f) { return false; } }
//		if constexpr (b_t::ends.is_a_finite()) { if (percents.b_to_reach_a < 0.f) { return false; } }
//		if constexpr (b_t::ends.is_b_finite()) { if (percents.b_to_reach_a > 1.f) { return false; } }
//		return intersects(shape::line{a.a, a.b}, shape::line{b.a, b.b});
//		}
//
//	std::optional<vec2f> intersection(const shape::concepts::ab auto& a, const shape::concepts::ab auto& b) noexcept
//		{
//		if (!intersects(a, b)) { return std::nullopt; }
//
//		const vec2f s1{a.a_to_b()};
//		const vec2f s2{b.a_to_b()};
//		const auto percents{percentages(a, b)};
//
//		return vec2f{a.x() + (percents.a_to_reach_b * s1.x()), a.y() + (percents.a_to_reach_b * s1.y())};
//		}
//
//	utils_gpu_available constexpr bool intersects(const shape::concepts::segment auto& a, const shape::concepts::line auto& b) noexcept
//		{
//		const auto point_side_a{side(b, a.a)};
//		const auto point_side_b{side(b, a.b)};
//		return (point_side_a != point_side_b) || (point_side_a == point_side_b && point_side_a.is_coincident());
//		}
//
//	utils_gpu_available constexpr bool intersects(const shape::concepts::segment auto& a, const shape::concepts::segment auto& b) noexcept
//		{
//		const auto a_intersects_b_line{intersects(a, shape::line{b.a, b.b})};
//		const auto b_intersects_a_line{intersects(b, shape::line{a.a, a.b})};
//		return a_intersects_b_line || b_intersects_a_line;
//		}
//	}