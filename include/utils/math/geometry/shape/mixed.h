#pragma once

#include <span>
#include <variant>

#include "../details/base_types.h"
#include "../../../oop/disable_move_copy.h"

#include "ab.h"
#include "bezier.h"
#include "vertices.h"

namespace utils::math::geometry::shape
	{
	namespace generic
		{
		// TODO use extent for piece metadata's size, and calculate the vertices extent based on it

		/// <summary> Only use dynamic extent, static extent not supported (yet)</summary>
		template <storage::type storage_type, ends ENDS, size_t extent = std::dynamic_extent>
		struct mixed : vertices<storage_type, ENDS, extent>
			{
			using vertices_t = vertices<storage_type, ENDS, extent>;
			using vertices_t::size;
			using vertices_t::ends;
			using vertices_t::vertices;
			using vertices_t::operator[];
			using typename vertices_t::value_type;
			using typename vertices_t::const_aware_value_type;

			using self_t = polyline<storage_type, ends, extent>;
			using nonref_self_t = polyline<storage::type::create::owner(), ends, extent>;

			struct piece_metadata_t
				{
				enum class type_t : uint8_t { segment, bezier_3pt, bezier_4pt, bezier } type;
				uint8_t index;
				};

			utils::storage::multiple<storage::storage_type_for<piece_metadata_t, storage_type>, extent, true> pieces_metadata;

			template <bool is_view_const>
			struct pieces_view : std::ranges::view_interface<pieces_view<is_view_const>>, ::utils::oop::non_copyable, ::utils::oop::non_movable
				{
				using const_aware_mixed_t = std::conditional_t<is_view_const, const self_t, self_t>;
				const_aware_mixed_t& mixed_ref;

				inline static constexpr auto pieces_storage_type{storage::type::create::observer(is_view_const)};
				using edge_t       = utils::math::geometry::shape::generic::segment <pieces_storage_type                     >;
				using bezier_3pt_t = utils::math::geometry::shape::generic::bezier  <pieces_storage_type, 3                  >;
				using bezier_4pt_t = utils::math::geometry::shape::generic::bezier  <pieces_storage_type, 4                  >;
				using bezier_t     = utils::math::geometry::shape::generic::bezier  <pieces_storage_type, std::dynamic_extent>;

				//TODO iterator on variant_t
				using variant_t = std::variant<edge_t, bezier_3pt_t, bezier_4pt_t, bezier_t>;

				template <typename callback_t>
				utils_gpu_available constexpr void for_each(callback)
					{
					for (size_t index{0}; index < mixed_ref.pieces_metadata.size() - 1; index++)
						{
						call_at(index, callback);
						}
					}

				template <typename callback_t>
				utils_gpu_available constexpr void call_at(size_t index_piece, callback)
					{
					const piece_metadata_t piece_metadata{mixed_ref.pieces_metadata[index_piece]};
					const size_t index_vertex_begin{piece_metadata.index};
					const size_t index_vertex_last  
						{
						index_piece < (mixed_ref.pieces_metadata.size() - 1) ?
						mixed_ref.pieces_metadata[mixed_ref.pieces_metadata.size() - 1].index : 
						mixed_ref.ends_aware_size() - 1
						};

					switch (piece_metadata.type)
						{
						case piece_metadata_t::type_t::segment   : call_segments   (index_vertex_begin, index_vertex_last, callback); break;
						case piece_metadata_t::type_t::bezier_3pt: call_bezier_3pts(index_vertex_begin, index_vertex_last, callback); break;
						case piece_metadata_t::type_t::bezier_4pt: call_bezier_4pts(index_vertex_begin, index_vertex_last, callback); break;
						case piece_metadata_t::type_t::bezier    : call_bezier     (index_vertex_begin, index_vertex_last, callback); break;
						default: break;
						}
					}
				
				template <typename callback_t>
				utils_gpu_available constexpr void call_segments(size_t index_vertex_begin, size_t index_vertex_last, callback_t callback)
					{
					const size_t index_vertex_end{index_vertex_last + 1};
					const size_t vertices_count{index_vertex_end - index_vertex_begin};
					const size_t pieces_count{vertices_count - 1};

					for (size_t i{0}; i < pieces_count; i++)
						{
						const size_t index_a{index_vertex_begin + i};
						const size_t index_b{index_a + 1};
						const auto vertex_a{mixed_ref.ends_aware_access(index_a)};
						const auto vertex_b{mixed_ref.ends_aware_access(index_b)};
						const shape::segment segment{vertex_a, vertex_b};
						callback(segment);
						}
					}
				
				template <typename callback_t>
				utils_gpu_available constexpr void call_bezier_3pts(size_t index_vertex_begin, size_t index_vertex_last, callback_t callback)
					{
					const size_t index_vertex_end{index_vertex_last + 1};
					const size_t vertices_count{index_vertex_end - index_vertex_begin};
					const size_t pieces_count{(vertices_count - 1) / 2};
					assert((vertices_count - 1) % 2 == 0);

					for (size_t i{index_vertex_begin}; i < index_vertex_end; i += 2)
						{
						const size_t index_a{i};
						const size_t index_b{index_a + 1};
						const size_t index_c{index_b + 1};
						const auto vertex_a{mixed_ref.ends_aware_access(index_a)};
						const auto vertex_b{mixed_ref.ends_aware_access(index_b)};
						const auto vertex_c{mixed_ref.ends_aware_access(index_c)};
						const shape::const_observer::bezier<3> piece{vertex_a, vertex_b, vertex_c};
						callback(piece);
						}
					}

				template <typename callback_t>
				utils_gpu_available constexpr void call_bezier_4pts(size_t index_vertex_begin, size_t index_vertex_last, callback_t callback)
					{
					const size_t index_vertex_end{index_vertex_last + 1};
					const size_t vertices_count{index_vertex_end - index_vertex_begin};
					const size_t pieces_count{(vertices_count - 1) / 3};
					assert((vertices_count - 1) % 2 == 0);

					for (size_t i{index_vertex_begin}; i < index_vertex_end; i += 3)
						{
						const size_t index_a{index_vertex_begin + i};
						const size_t index_b{index_a + 1};
						const size_t index_c{index_c + 1};
						const size_t index_d{index_b + 1};
						const auto vertex_a{mixed_ref.ends_aware_access(index_a)};
						const auto vertex_b{mixed_ref.ends_aware_access(index_b)};
						const auto vertex_c{mixed_ref.ends_aware_access(index_c)};
						const auto vertex_d{mixed_ref.ends_aware_access(index_c)};
						const shape::const_observer::bezier<4> piece{vertex_a, vertex_b, vertex_c, vertex_d};
						callback(piece);
						}
					}

				template <typename callback_t>
				utils_gpu_available constexpr void call_bezier(size_t index_vertex_begin, size_t index_vertex_last, callback_t callback)
					{
					//TODO asser: generic bezier cannot be last piece in a closed mixed, no ends_aware_access
					const size_t index_vertex_end{index_vertex_last + 1};
					const size_t vertices_count{index_vertex_end - index_vertex_begin};
					
					shape::const_observer::bezier<std::dynamic_extent> curve{mixed_ref.begin() + index_vertex_begin, vertices_count};

					const shape::const_observer::bezier<4> piece{vertex_a, vertex_b, vertex_c, vertex_d};
					callback(piece);
					}
				};

			utils_gpu_available constexpr auto get_pieces() const noexcept { return pieces_view<true                   >{vertices_t::storage.begin(), vertices_t::storage.size()}; }
			utils_gpu_available constexpr auto get_pieces()       noexcept { return pieces_view<storage_type.is_const()>{vertices_t::storage.begin(), vertices_t::storage.size()}; }

			};
		}

	namespace concepts
		{
		template <typename T>
		concept mixed = std::derived_from<T, shape::generic::mixed<T::storage_type, T::ends, T::extent>>;
		}
	}
