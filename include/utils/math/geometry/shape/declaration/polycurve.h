#pragma once

#include "../../details/base_types.h"
#include "bezier.h"
#include "vertices.h"

namespace utils::math::geometry::shape
	{
	namespace details::polycurve
		{
		template <typename T, typename piece_t>
		concept pieces_callable_without_index = requires(T t, piece_t piece, size_t index) { t(piece); };
		template <typename T, typename piece_t>
		concept pieces_callable_with_index    = requires(T t, piece_t piece, size_t index) { t(piece, index); };
		template <typename T, typename piece_t>
		concept pieces_callable = pieces_callable_without_index<T, piece_t> || pieces_callable_with_index<T, piece_t>;
		}

	namespace generic
		{
		template <size_t piece_vertices_count, storage::type STORAGE_TYPE, geometry::ends::closeable ENDS = ends::closeable::create::open(), size_t EXTENT = std::dynamic_extent>
		struct polycurve;
		}

	
	namespace concepts
		{
		template <typename T, size_t piece_vertices_count> 
		concept polycurve_of = concepts::shape<T> && std::derived_from<T, shape::generic::polycurve<piece_vertices_count, T::storage_type, T::ends, T::extent>>;
		template <typename T> concept polycurve = polycurve_of<T, T::piece_vertices_count>;
		}
	
	namespace owner 
		{
		template <size_t piece_vertices_count, geometry::ends::closeable ends = geometry::ends::closeable::create::open(), size_t extent = std::dynamic_extent>
		using polycurve = shape::generic::polycurve<piece_vertices_count, storage::type::create::owner(), ends, extent>;
		}
	namespace observer
		{
		template <size_t piece_vertices_count, geometry::ends::closeable ends = geometry::ends::closeable::create::open(), size_t extent = std::dynamic_extent>
		using polycurve = shape::generic::polycurve<piece_vertices_count, storage::type::create::observer(), ends, extent>;
		}
	namespace const_observer
		{
		template <size_t piece_vertices_count, geometry::ends::closeable ends = geometry::ends::closeable::create::open(), size_t extent = std::dynamic_extent>
		using polycurve = shape::generic::polycurve<piece_vertices_count, storage::type::create::const_observer(), ends, extent>;
		}
	}