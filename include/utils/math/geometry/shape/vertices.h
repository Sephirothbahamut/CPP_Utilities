#pragma once

#include "declaration/vertices.h"

namespace utils::math::geometry
	{
	template <storage::type storage_type, bool CLOSED, size_t extent>
	struct ends_aware_vertices : utils::storage::multiple<storage::storage_type_for<geometry::shape::point, storage_type>, extent, true>
		{
		private:
			using multiple_t = utils::storage::multiple<storage::storage_type_for<geometry::shape::point, storage_type>, extent, true>;

		public:
			using multiple_t::multiple;
			inline static constexpr auto closed{CLOSED};

			template <bool closed>
			utils_gpu_available constexpr const auto& ends_aware_access(const size_t index) const noexcept
				{
				const size_t remapped_index{ends_aware_index<closed>(index)};
				return multiple_t::operator[](remapped_index);
				}
			utils_gpu_available constexpr const auto& ends_aware_access(const size_t index) const noexcept { return ends_aware_access<closed>(index); }

			template <bool closed = true>
			utils_gpu_available constexpr auto& ends_aware_access(const size_t index) noexcept
				{
				const size_t remapped_index{ends_aware_index<closed>(index)};
				return multiple_t::operator[](remapped_index);
				}
			utils_gpu_available constexpr auto& ends_aware_access(const size_t index) noexcept { return ends_aware_access<closed>(index); }

			template <bool closed = true>
			utils_gpu_available constexpr size_t ends_aware_index(const size_t index) const noexcept
				{
				if constexpr (closed)
					{
					return index % multiple_t::size();
					}
				else { return index; }
				}
			utils_gpu_available constexpr size_t ends_aware_index(const size_t index) const noexcept { return ends_aware_index<closed>(index); }
		};
	}