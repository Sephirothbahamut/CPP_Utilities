#pragma once

#include "declaration/circle.h"

namespace utils::math::geometry::shape::generic
	{
	template <storage::type STORAGE_TYPE>
	struct utils_oop_empty_bases circle : utils::math::geometry::shape_flag
		{
		inline static constexpr auto storage_type{STORAGE_TYPE};

		using self_t       = circle<storage_type                  >;
		using owner_self_t = circle<storage::type::create::owner()>;
		using observer_self_t       = circle<storage::type::create::observer      ()>;
		using const_observer_self_t = circle<storage::type::create::const_observer()>;
		
		const_observer_self_t create_observer() const noexcept { return {*this}; }
		      observer_self_t create_observer()       noexcept { return {*this}; }

		using vertex_t = generic::point<storage_type>;
		using radius_t = storage::single<storage::storage_type_for<float, storage_type>>;

		vertex_t centre;
		radius_t radius;

		#ifdef __INTELLISENSE__
		//Intellisense for some reason doesn't enjoy the concept centric constructors here, lacking this constructor makes intellisense crash in longer `.cpp`s
		utils_gpu_available constexpr circle(const shape::point& centre, float radius);
		#endif

		utils_gpu_available constexpr circle() noexcept = default;
		utils_gpu_available constexpr circle(      concepts::point auto& centre, storage::concepts::can_construct_value_type<radius_t> auto& radius) :
			centre{centre}, radius{radius} {}

		utils_gpu_available constexpr circle(const concepts::point auto& centre, const storage::concepts::can_construct_value_type<radius_t> auto& radius)
			requires(storage_type.can_construct_from_const()) :
			centre{centre}, radius{radius} {}

		utils_gpu_available constexpr circle(const concepts::circle auto& other) requires(storage_type.can_construct_from_const()): 
			centre{other.centre}, radius{other.radius} {}

		utils_gpu_available constexpr circle(      concepts::circle auto& other) requires(storage::constness_matching<self_t, decltype(other)>::compatible_constness) :
			centre{other.centre}, radius{other.radius} {}
		struct sdf_proxy;
		utils_gpu_available constexpr sdf_proxy sdf(const vec<float, 2>& point) const noexcept;
		utils_gpu_available constexpr auto bounding_box() const noexcept;
		utils_gpu_available constexpr auto bounding_circle() const noexcept;
		#include "../transform/common_declaration.inline.h"
		};
	}

static_assert(utils::math::geometry::shape::concepts::circle
	<
	utils::math::geometry::shape::circle
	>);
static_assert(utils::math::geometry::shape::concepts::shape
	<
	utils::math::geometry::shape::circle
	>);