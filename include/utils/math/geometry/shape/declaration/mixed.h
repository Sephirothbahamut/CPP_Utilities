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
		concept pieces_callable_without_index =
			requires(T t, geometry::shape::segment   edge) { t(edge); } ||
			requires(T t, geometry::shape::bezier<3> edge) { t(edge); } ||
			requires(T t, geometry::shape::bezier<4> edge) { t(edge); };
		template <typename T>
		concept pieces_callable_with_index    =
			requires(T t, geometry::shape::segment   edge, size_t first, size_t last) { t(edge, first, last); } ||
			requires(T t, geometry::shape::bezier<3> edge, size_t first, size_t last) { t(edge, first, last); } ||
			requires(T t, geometry::shape::bezier<4> edge, size_t first, size_t last) { t(edge, first, last); }; 
		template <typename T>
		concept pieces_callable = pieces_callable_without_index<T> || pieces_callable_with_index<T>;
		template <typename T>
		concept pieces_pair_callable_without_index =
			requires(T t, geometry::shape::segment   a, geometry::shape::segment   b) { t(a, b); } ||
			requires(T t, geometry::shape::segment   a, geometry::shape::bezier<3> b) { t(a, b); } ||
			requires(T t, geometry::shape::segment   a, geometry::shape::bezier<4> b) { t(a, b); } ||
			requires(T t, geometry::shape::bezier<3> a, geometry::shape::bezier<3> b) { t(a, b); } ||
			requires(T t, geometry::shape::bezier<3> a, geometry::shape::bezier<4> b) { t(a, b); } ||
			requires(T t, geometry::shape::bezier<4> a, geometry::shape::bezier<4> b) { t(a, b); } ||
			requires(T t, geometry::shape::bezier<3> a, geometry::shape::segment   b) { t(a, b); } ||
			requires(T t, geometry::shape::bezier<4> a, geometry::shape::segment   b) { t(a, b); } ||
			requires(T t, geometry::shape::bezier<4> a, geometry::shape::bezier<3> b) { t(a, b); };
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
	}