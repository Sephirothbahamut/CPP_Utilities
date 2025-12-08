#pragma once

#include "declaration/arc.h"
#include "../../angle.h"

namespace utils::math::geometry::shape::generic
	{
	template <storage::type STORAGE_TYPE>
	struct utils_oop_empty_bases arc : utils::math::geometry::shape_flag
		{
		inline static constexpr auto storage_type{STORAGE_TYPE};

		using self_t                = arc<storage_type                  >;
		using owner_self_t          = arc<storage::type::create::owner()>;
		using observer_self_t       = arc<storage::type::create::observer      ()>;
		using const_observer_self_t = arc<storage::type::create::const_observer()>;
		
		const_observer_self_t create_observer() const noexcept { return {*this}; }
		      observer_self_t create_observer()       noexcept { return {*this}; }

		using vertex_t = generic::point<storage_type>;
		using radius_t = storage::single<storage::storage_type_for<float, storage_type>>;
		using angle_t  = storage::single<storage::storage_type_for<utils::math::angle::degf, storage_type>>;

		vertex_t centre;
		radius_t radius;
		angle_t  start_angle;
		angle_t  aperture_angle;

		#ifdef __INTELLISENSE__
		//Intellisense for some reason doesn't enjoy the concept centric constructors here, lacking this constructor makes intellisense crash in longer `.cpp`s
		utils_gpu_available constexpr arc(const shape::point& centre, float radius, const utils::math::angle::degf& start_angle, const utils::math::angle::degf& aperture_angle);
		#endif

		utils_gpu_available constexpr arc() noexcept = default;
		utils_gpu_available constexpr arc(concepts::point auto& centre, storage::concepts::can_construct_value_type<radius_t> auto& radius, math::angle::concepts::angle auto& start_angle, math::angle::concepts::angle auto& aperture_angle) :
			centre{centre}, radius{radius}, start_angle{start_angle}, aperture_angle{aperture_angle} {}

		utils_gpu_available constexpr arc(const concepts::point auto& centre, const storage::concepts::can_construct_value_type<radius_t> auto& radius, const math::angle::concepts::angle auto& start_angle, const math::angle::concepts::angle auto& aperture_angle)
			requires(storage_type.can_construct_from_const()) :
			centre{centre}, radius{radius}, start_angle{start_angle}, aperture_angle{aperture_angle} {}

		utils_gpu_available constexpr arc(const concepts::arc auto& other) requires(storage_type.can_construct_from_const()): 
			centre{other.centre}, radius{other.radius}, start_angle{other.start_angle}, aperture_angle{other.aperture_angle} {}

		utils_gpu_available constexpr arc(      concepts::arc auto& other) requires(storage::constness_matching<self_t, decltype(other)>::compatible_constness) :
			centre{other.centre}, radius{other.radius}, start_angle{other.start_angle}, aperture_angle{other.aperture_angle} {}
		struct sdf_proxy;
		utils_gpu_available constexpr sdf_proxy sdf(const vec<float, 2>& point) const noexcept;
		utils_gpu_available constexpr auto bounding_box() const noexcept;
		utils_gpu_available constexpr auto bounding_circle() const noexcept;
		#include "../transform/common_declaration.inline.h"
		};
	}

static_assert(utils::math::geometry::shape::concepts::arc
	<
	utils::math::geometry::shape::arc
	>);
static_assert(utils::math::geometry::shape::concepts::shape
	<
	utils::math::geometry::shape::arc
	>);