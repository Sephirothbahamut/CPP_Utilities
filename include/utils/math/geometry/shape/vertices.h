#pragma once

#include <span>
#include <array>
#include <vector>
#include <optional>

#include "../details/base_types.h"
#include "../../../oop/disable_move_copy.h"
#include "point.h"

namespace utils::math::geometry
	{
	template <storage::type storage_type, size_t extent = std::dynamic_extent>
	using vertices = utils::storage::multiple<storage::storage_type_for<geometry::shape::point, storage_type>, extent, true>;

	template <storage::type storage_type, geometry::ends ends = geometry::ends::create::open(), size_t extent = std::dynamic_extent>
	struct ends_aware_vertices : utils::storage::multiple<storage::storage_type_for<geometry::shape::point, storage_type>, extent, true>
		{
		private:
			using multiple_t = utils::storage::multiple<storage::storage_type_for<geometry::shape::point, storage_type>, extent, true>;

		public:
			using multiple_t::multiple;

			template <bool ends_aware = true>
			utils_gpu_available constexpr const auto& ends_aware_access(const size_t index) const noexcept
				{
				const size_t remapped_index{ends_aware_index<ends_aware>(index)};
				return multiple_t::operator[](remapped_index);
				}

			template <bool ends_aware = true>
			utils_gpu_available constexpr auto& ends_aware_access(const size_t index) noexcept
				{
				const size_t remapped_index{ends_aware_index<ends_aware>(index)};
				return multiple_t::operator[](remapped_index);
				}

			template <bool ends_aware = true>
			utils_gpu_available constexpr size_t ends_aware_index(const size_t index) const noexcept
				{
				if constexpr (ends_aware && ends.is_closed())
					{
					return index % multiple_t::size();
					}
				else { return index; }
				}

			template <bool ends_aware = true>
			utils_gpu_available constexpr size_t ends_aware_size() const noexcept
				{
				if constexpr (ends_aware && ends.is_closed())
					{
					return multiple_t::size() + 1;
					}
				else { return multiple_t::size(); }
				}
		};


	namespace concepts
		{
		template <typename T>
		concept vertices = std::derived_from<std::remove_cvref_t<T>, geometry::vertices<std::remove_cvref_t<T>::storage_type, std::remove_cvref_t<T>::extent>>;
		}

	namespace shape
		{
		namespace concepts
			{
			template <typename T>
			concept has_vertices = 
				requires(T t)
					{
						{ t.vertices } -> geometry::concepts::vertices;
					};
			}
		}
	}