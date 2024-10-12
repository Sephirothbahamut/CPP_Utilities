#pragma once

#include "declaration/aabb.h"


static_assert(utils::math::geometry::shape::concepts::aabb
	<
	utils::math::geometry::shape::aabb
	>);
static_assert(utils::math::geometry::shape::concepts::aabb
	<
	utils::math::geometry::shape::observer::aabb
	>);
static_assert(utils::math::geometry::shape::concepts::shape
	<
	utils::math::geometry::shape::aabb
	>);


