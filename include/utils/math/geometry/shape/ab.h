#pragma once

#include "../details/base_types.h"
#include "../../../storage.h"
#include "../../../template/optional.h"
#include "point.h"

namespace utils::math::geometry::shape
	{
	namespace generic
		{
		template <storage::type STORAGE_TYPE, geometry::ends::optional_ab OPTIONAL_ENDS>
		struct utils_oop_empty_bases ab : geometry::piece_flag, geometry::shape_flag
			{
			inline static constexpr auto storage_type {STORAGE_TYPE };
			inline static constexpr auto optional_ends{OPTIONAL_ENDS};

			using self_t   = ab            <storage_type, optional_ends>;
			using vertex_t = generic::point<storage_type>;
			template <bool is_function_const>
			using vertex_observer = generic::point<storage::type::create::observer(is_function_const)>;
			using nonref_self_t = ab<storage::type::create::owner(), optional_ends>;

			utils_gpu_available constexpr ab(const utils::math::concepts::vec_size<2> auto& a, const utils::math::concepts::vec_size<2> auto& b) noexcept
				requires(storage_type.can_construct_from_const()) : 
				a{a}, b{b} {}
			
			utils_gpu_available constexpr ab(utils::math::concepts::vec_size<2> auto& a, utils::math::concepts::vec_size<2> auto& b) noexcept :
				a{a}, b{b} {}

			template <storage::type other_storage_type, geometry::ends::optional_ab other_optional_ends>
			utils_gpu_available constexpr ab(ab<other_storage_type, other_optional_ends>& other) noexcept
				requires(storage::constness_matching<self_t, ab<other_storage_type, other_optional_ends>>::compatible_constness) :
				a{other.a}, b{other.b} {}

			template <storage::type other_storage_type, geometry::ends::optional_ab other_optional_ends>
			utils_gpu_available constexpr ab(const ab<other_storage_type, other_optional_ends>& other) noexcept
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
			};

		template <storage::type storage_type, ends::ab ends>
		using ab_ends_aware = ab<storage_type, geometry::ends::optional_ab::create::value(ends)>;
		

		template <storage::type storage_type>
		using line = ab_ends_aware<storage_type, ends::ab::create::infinite()>;
		template <storage::type storage_type>
		using ray = ab_ends_aware<storage_type, ends::ab::create::default_(true, false)>;
		template <storage::type storage_type>
		using reverse_ray = ab_ends_aware<storage_type, ends::ab::create::default_(false, true)>;
		template <storage::type storage_type>
		using segment = ab_ends_aware<storage_type, ends::ab::create::finite()>;
		}

	namespace concepts
		{
		template <typename T> concept ab            = std::derived_from<T, shape::generic::ab<T::storage_type, T::optional_ends>>;
		template <typename T> concept ab_ends_aware = ab<T> && T::optional_ends.has_value();
		template <typename T> concept line          = ab_ends_aware<T> && std::same_as<T, shape::generic::line       <T::storage_type>>;
		template <typename T> concept ray           = ab_ends_aware<T> && std::same_as<T, shape::generic::ray        <T::storage_type>>;
		template <typename T> concept reverse_ray   = ab_ends_aware<T> && std::same_as<T, shape::generic::reverse_ray<T::storage_type>>;
		template <typename T> concept segment       = ab_ends_aware<T> && std::same_as<T, shape::generic::segment    <T::storage_type>>;
		}
	
	namespace owner 
		{
		template <geometry::ends::optional_ab optional_ends = geometry::ends::optional_ab::create::empty()>
		using ab          = shape::generic::ab         <storage::type::create::owner(), optional_ends>;
		using line        = shape::generic::line       <storage::type::create::owner()>;
		using ray         = shape::generic::ray        <storage::type::create::owner()>;
		using reverse_ray = shape::generic::reverse_ray<storage::type::create::owner()>;
		using segment     = shape::generic::segment    <storage::type::create::owner()>;
		}
	namespace observer
		{
		template <geometry::ends::optional_ab optional_ends = geometry::ends::optional_ab::create::empty()>
		using ab          = shape::generic::ab         <storage::type::create::observer(), optional_ends>;
		using line        = shape::generic::line       <storage::type::create::observer()>;
		using ray         = shape::generic::ray        <storage::type::create::observer()>;
		using reverse_ray = shape::generic::reverse_ray<storage::type::create::observer()>;
		using segment     = shape::generic::segment    <storage::type::create::observer()>;
		}
	namespace const_observer
		{
		template <geometry::ends::optional_ab optional_ends = geometry::ends::optional_ab::create::empty()>
		using ab          = shape::generic::ab         <storage::type::create::const_observer(), optional_ends>;
		using line        = shape::generic::line       <storage::type::create::const_observer()>;
		using ray         = shape::generic::ray        <storage::type::create::const_observer()>;
		using reverse_ray = shape::generic::reverse_ray<storage::type::create::const_observer()>;
		using segment     = shape::generic::segment    <storage::type::create::const_observer()>;
		}
	}

	static_assert(utils::math::geometry::shape::concepts::ab
		<
		utils::math::geometry::shape::ab<>
		>);
	static_assert(utils::math::geometry::shape::concepts::shape
		<
		utils::math::geometry::shape::ab<>
		>);