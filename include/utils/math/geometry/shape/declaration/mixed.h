#pragma once

#include "../../details/base_types.h"
#include "../../../../oop/disable_move_copy.h"

#include "ab.h"
#include "bezier.h"
#include "vertices.h"
#include "polyline.h"

namespace utils::math::geometry::shape
	{
	namespace details::mixed
		{
		//TODO check for all piece types, not just segment
		template <typename T>
		concept pieces_callable_without_index = requires(T t, geometry::shape::segment edge, size_t first, size_t last) { t(edge); };
		template <typename T>
		concept pieces_callable_with_index    = requires(T t, geometry::shape::segment edge, size_t first, size_t last) { t(edge, first, last); };
		template <typename T>
		concept pieces_callable = pieces_callable_without_index<T> || pieces_callable_with_index<T>;
		}

	namespace generic
		{
		/// <summary> 
		/// Only use finite or closed ends, infinite ends not supported (yet)
		/// </summary>
		template <storage::type STORAGE_TYPE, ends::closeable ENDS>
		struct mixed;
		}

	namespace concepts
		{
		template <typename T>
		concept mixed = concepts::shape<T> && std::derived_from<T, shape::generic::mixed<T::storage_type, T::ends>>;
		}

	namespace owner 
		{
		template <geometry::ends::closeable ends = geometry::ends::closeable::create::closed()>
		using mixed = shape::generic::mixed<storage::type::create::owner(), ends>;
		}
	namespace observer
		{
		template <geometry::ends::closeable ends = geometry::ends::closeable::create::closed()>
		using mixed = shape::generic::mixed<storage::type::create::observer(), ends>;
		}
	namespace const_observer
		{
		template <geometry::ends::closeable ends = geometry::ends::closeable::create::closed()>
		using mixed = shape::generic::mixed<storage::type::create::const_observer(), ends>;
		}

	template <concepts::mixed T, storage::type desired_storage_type>
	struct cast_storage_type<T, desired_storage_type>
		{
		using type = generic::mixed<desired_storage_type, T::ends>;
		};
	}