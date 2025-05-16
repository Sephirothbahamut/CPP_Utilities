#pragma once

#include "declaration/capsule.h"
#include "ab.h"

namespace utils::math::geometry::shape::generic
	{
	template <storage::type STORAGE_TYPE>
	struct utils_oop_empty_bases capsule : utils::math::geometry::shape_flag
		{
		inline static constexpr auto storage_type{STORAGE_TYPE};

		using self_t                = capsule<storage_type                           >;
		using owner_self_t          = capsule<storage::type::create::owner         ()>;
		using observer_self_t       = capsule<storage::type::create::observer      ()>;
		using const_observer_self_t = capsule<storage::type::create::const_observer()>;
		
		const_observer_self_t create_observer() const noexcept { return {*this}; }
		      observer_self_t create_observer()       noexcept { return {*this}; }

		using edge_t   = generic::segment<storage_type>;
		using radius_t = storage::single<storage::storage_type_for<float, storage_type>>;

		edge_t ab;
		radius_t radius;
		

		utils_gpu_available constexpr capsule() noexcept = default;

		utils_gpu_available constexpr capsule(      concepts::ab auto& ab, storage::concepts::can_construct_value_type<radius_t> auto& radius) :
			ab{ab}, radius{radius} {}

		utils_gpu_available constexpr capsule(const concepts::ab auto& ab, const storage::concepts::can_construct_value_type<radius_t> auto& radius)
			requires(storage_type.can_construct_from_const()) :
			ab{ab}, radius{radius} {}

		utils_gpu_available constexpr capsule(const concepts::capsule auto& other) 
			requires(storage_type.can_construct_from_const()) :
			ab{other.ab}, radius{other.radius} {}

		utils_gpu_available constexpr capsule(      concepts::capsule auto& other) 
			requires(storage::constness_matching<self_t, decltype(other)>::compatible_constness) :
			ab{other.ab}, radius{other.radius} {}

		struct sdf_proxy;
		utils_gpu_available constexpr sdf_proxy sdf(const vec<float, 2>& point) const noexcept;
		utils_gpu_available constexpr auto bounding_box() const noexcept;
		utils_gpu_available constexpr auto bounding_circle() const noexcept;
		#include "../transform/common_declaration.inline.h"
		};
	}

static_assert(utils::math::geometry::shape::concepts::capsule
	<
	utils::math::geometry::shape::capsule
	>);
static_assert(utils::math::geometry::shape::concepts::shape
	<
	utils::math::geometry::shape::capsule
	>);