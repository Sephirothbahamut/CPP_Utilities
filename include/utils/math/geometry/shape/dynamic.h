#pragma once

#include "declaration/dynamic.h"

#include "../../../polymorphic_value.h"

namespace utils::math::geometry::shape::dynamic
	{
	struct base
		{
		struct sdf_proxy;
		virtual sdf_proxy sdf(const vec<float, 2>& point) const noexcept = 0;
		};

	namespace details
		{
		template <utils::math::geometry::shape::concepts::shape SHAPE_T, storage::type STORAGE_TYPE>
		struct templated_child : dynamic::base
			{
			using shape_t = SHAPE_T;
			storage::single<shape_t> shape;

			struct sdf_proxy;
			virtual dynamic::base::sdf_proxy sdf(const vec<float, 2>& point) const noexcept;
			};
		}
	}
