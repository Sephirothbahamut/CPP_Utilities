#pragma once

#include "declaration/dynamic.h"

#include "../../../polymorphic_value.h"

namespace utils::math::geometry::shape::dynamic
	{
	struct base
		{
		struct sdf_proxy;
		virtual dynamic::base::sdf_proxy sdf(const vec<float, 2>& point) const noexcept = 0;
		virtual shape::aabb bounding_box() const noexcept = 0;
		};

	namespace details
		{
		template <utils::math::geometry::shape::concepts::shape SHAPE_T>
		struct templated_child : dynamic::base
			{
			using shape_t = SHAPE_T;
			shape_t shape;

			templated_child(const shape_t& shape) : shape{shape} {}

			struct sdf_proxy;
			virtual dynamic::base::sdf_proxy sdf(const vec<float, 2>& point) const noexcept;

			virtual shape::aabb bounding_box() const noexcept { return shape.bounding_box(); }
			};
		}
	}

#include "../sdf/dynamic.h" //Since it's templated the .sdf(...) function definition must be known by all who include "dynamic.h" even if they don't need it.