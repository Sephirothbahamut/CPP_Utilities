#pragma once

#include <span>
#include <array>
#include <vector>
#include <optional>

#include "../../details/base_types.h"
#include "../../../../oop/disable_move_copy.h"
#include "point.h"

namespace utils::math::geometry
	{
	template <storage::type storage_type, size_t extent = std::dynamic_extent>
	using vertices = utils::storage::multiple<storage::storage_type_for<geometry::shape::point, storage_type>, extent, true>;

	template <storage::type storage_type, bool CLOSED, size_t extent = std::dynamic_extent>
	struct ends_aware_vertices;

	namespace concepts
		{
		template <typename T>
		concept vertices = std::derived_from<std::remove_cvref_t<T>, geometry::vertices<std::remove_cvref_t<T>::storage_type, std::remove_cvref_t<T>::extent>>;
		template <typename T>
		concept ends_aware_vertices = std::derived_from<std::remove_cvref_t<T>, geometry::ends_aware_vertices<std::remove_cvref_t<T>::storage_type, std::remove_cvref_t<T>::closed, std::remove_cvref_t<T>::extent>>;
		}

	namespace shape
		{
		namespace concepts
			{
			template <typename T>
			concept has_vertices = shape<T> &&
				requires(T t)
					{
						{ t.vertices } -> geometry::concepts::vertices;
					};
			}
		}
	}