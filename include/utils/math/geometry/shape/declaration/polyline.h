#pragma once

#include "../../details/base_types.h"
#include "ab.h"
#include "vertices.h"

namespace utils::math::geometry::shape
	{
	namespace details::polyline
		{
		template <typename T>
		concept edges_callable_without_index = requires(T t, shape::segment edge, size_t index) { t(edge); };
		template <typename T>
		concept edges_callable_with_index    = requires(T t, shape::segment edge, size_t index) { t(edge, index); };
		template <typename T>
		concept edges_callable = edges_callable_without_index<T> || edges_callable_with_index<T>;
		}

	namespace generic
		{
		template <storage::type STORAGE_TYPE, geometry::ends::closeable ENDS = ends::closeable::create::open(), size_t EXTENT = std::dynamic_extent>
		struct polyline;

		template <storage::type storage_type, size_t extent = std::dynamic_extent>
		using polygon = polyline<storage_type, ends::closeable::create::closed(), extent>;
		}

	
	namespace concepts
		{
		template <typename T> 
		concept polyline = concepts::shape<T> && std::derived_from<T, shape::generic::polyline<T::storage_type, T::ends, T::extent>>;
		template <typename T> 
		concept polygon  = polyline<T> && T::ends == ends::closeable::create::closed();
		}
	
	namespace owner 
		{
		template <geometry::ends::closeable ends = geometry::ends::closeable::create::open(), size_t extent = std::dynamic_extent>
		using polyline = shape::generic::polyline<storage::type::create::owner(), ends, extent>;
		template <size_t extent = std::dynamic_extent>
		using polygon = shape::generic::polygon<storage::type::create::owner(), extent>;
		}
	namespace observer
		{
		template <geometry::ends::closeable ends = geometry::ends::closeable::create::open(), size_t extent = std::dynamic_extent>
		using polyline = shape::generic::polyline<storage::type::create::observer(), ends, extent>;
		template <size_t extent = std::dynamic_extent>
		using polygon = shape::generic::polygon<storage::type::create::observer(), extent>;
		}
	namespace const_observer
		{
		template <geometry::ends::closeable ends = geometry::ends::closeable::create::open(), size_t extent = std::dynamic_extent>
		using polyline = shape::generic::polyline<storage::type::create::const_observer(), ends, extent>;
		template <size_t extent = std::dynamic_extent>
		using polygon = shape::generic::polygon<storage::type::create::const_observer(), extent>;
		}
	}