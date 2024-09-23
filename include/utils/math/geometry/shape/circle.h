#pragma once

#include "declaration/circle.h"

namespace utils::math::geometry::shape::generic
	{
	template <storage::type STORAGE_TYPE>
	struct utils_oop_empty_bases circle : utils::math::geometry::shape_flag
		{
		inline static constexpr auto storage_type{STORAGE_TYPE};

		using self_t        = circle<storage_type                  >;
		using nonref_self_t = circle<storage::type::create::owner()>;

		using vertex_t = generic::point<storage_type>;
		using radius_t = storage::single<storage::storage_type_for<float, storage_type>>;

		vertex_t centre;
		radius_t radius;
		circle(vertex_t centre, storage::concepts::can_construct_value_type<radius_t> auto radius) : centre{centre}, radius{radius} {}

		struct sdf_proxy;
		utils_gpu_available sdf_proxy sdf(const shape::point& point) const noexcept;
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