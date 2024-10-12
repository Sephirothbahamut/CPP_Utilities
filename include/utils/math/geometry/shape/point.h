#pragma once

#include "declaration/point.h"

static_assert(utils::math::geometry::shape::concepts::point
	<
	utils::math::geometry::shape::point
	>);
static_assert(utils::math::geometry::shape::concepts::shape
	<
	utils::math::geometry::shape::point
	>);
