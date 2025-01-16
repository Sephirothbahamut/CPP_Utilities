#pragma once

#include <span>
#include <array>
#include <vector>
#include <optional>

#include "../../details/base_types.h"
#include "ab.h"
#include "point.h"
#include "../vertices.h"

// Note: exact distance for 4 control points here: https://github.com/nrtaylor/CubicSplineClosestPoint/tree/master
// TODO: understand math and get exact t parameter instead of just distance so we can get tangent and normal as well
// The current 4 control points solution is approximated and slow.

namespace utils::math::geometry::shape
	{
	namespace generic
		{
		template<storage::type STORAGE_TYPE, size_t EXTENT, geometry::ends::optional_ab OPTIONAL_ENDS>
		struct utils_oop_empty_bases bezier;
		}

	namespace concepts
		{
		template <typename T>
		concept bezier = concepts::shape<T> && std::derived_from<T, shape::generic::bezier<T::storage_type, T::extent, T::optional_ends>>;
		template <typename T>
		concept bezier_ends_aware = bezier<T> && T::optional_ends.has_value();
		}
	
	namespace owner 
		{
		template <size_t extent = std::dynamic_extent, geometry::ends::optional_ab optional_ends = ends::optional_ab::create::value(ends::ab::create::finite())>
		using bezier = shape::generic::bezier<storage::type::create::owner(), extent, optional_ends>;
		}
	namespace observer
		{
		template <size_t extent = std::dynamic_extent, geometry::ends::optional_ab optional_ends = ends::optional_ab::create::value(ends::ab::create::finite())>
		using bezier = shape::generic::bezier<storage::type::create::observer(), extent, optional_ends>;
		}
	namespace const_observer
		{
		template <size_t extent = std::dynamic_extent, geometry::ends::optional_ab optional_ends = ends::optional_ab::create::value(ends::ab::create::finite())>
		using bezier = shape::generic::bezier<storage::type::create::const_observer(), extent, optional_ends>;
		}
	}