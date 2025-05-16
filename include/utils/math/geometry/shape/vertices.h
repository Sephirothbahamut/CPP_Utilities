#pragma once

#include "declaration/vertices.h"

namespace utils::math::geometry
	{
	template <storage::type storage_type, bool CLOSED, size_t extent>
	struct utils_oop_empty_bases ends_aware_vertices : vertices<storage_type, extent>
		{
		private:
			using multiple_t = vertices<storage_type, extent>;

		public:
			using vertices<storage_type, extent>::vertices;
			inline static constexpr auto closed{CLOSED};

			//using vertices<storage_type, extent>::operator=;

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

	/// <summary> Only purpose is to not rewrite identical constructors for multiple typess</summary>
	template <concepts::vertices VERTICES_T>
	struct vertices_as_field
		{
		using vertices_t = VERTICES_T;
		vertices_t vertices;

		utils_gpu_available constexpr vertices_as_field() = default;
		
		template <typename iterator_t>
		utils_gpu_available constexpr vertices_as_field(iterator_t first, size_t count) : vertices(first, count) {}

		template <storage::concepts::can_construct_value_type<typename vertices_t::inner_storage_t::value_type>  ...Args>
		utils_gpu_available constexpr vertices_as_field(Args&&... args) :
			vertices(utils::storage::construct_flag_data, std::forward<Args>(args)...) 
			{}
		template <storage::concepts::can_construct_value_type<typename vertices_t::inner_storage_t::value_type>  ...Args>
		utils_gpu_available constexpr vertices_as_field(utils::storage::construct_flag_data_t, Args&&... args) :
			vertices(utils::storage::construct_flag_data, std::forward<Args>(args)...) 
			{}
		
		utils_gpu_available constexpr vertices_as_field(size_t size) : vertices(size) {}
		
		utils_gpu_available constexpr vertices_as_field(const shape::concepts::has_vertices auto& other) noexcept
			requires(vertices_t::storage_type.can_construct_from_const()) : 
			vertices(other.vertices) {}
		
		utils_gpu_available constexpr vertices_as_field(      shape::concepts::has_vertices auto& other) noexcept
			requires(storage::constness_matching<vertices_t, typename decltype(other)::vertices_t>::compatible_constness) :
			vertices(other.vertices) {}
		
		utils_gpu_available constexpr vertices_as_field(const concepts::vertices auto& other) noexcept
			requires(vertices_t::storage_type.can_construct_from_const()) :
			vertices(other) {}

		#include "../transform/common_declaration.inline.h"
		};
	}