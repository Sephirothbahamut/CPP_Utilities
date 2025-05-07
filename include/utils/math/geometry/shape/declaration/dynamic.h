#pragma once

#include "../../details/base_types.h"
#include "../../../../storage.h"

namespace utils::math::geometry::shape::dynamic
	{
	struct base;

	namespace details
		{
		template <utils::math::geometry::shape::concepts::shape shape_t, storage::type STORAGE_TYPE>
		struct templated_child;
		}
	}
