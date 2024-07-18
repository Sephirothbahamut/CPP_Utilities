#pragma once

#include <span>
#include <variant>

#include "../details/base_types.h"
#include "../../../oop/disable_move_copy.h"

#include "ab.h"
#include "bezier.h"
#include "vertices.h"
#include "polyline.h"

namespace utils::math::geometry::shape
	{
	namespace generic
		{
		/// <summary> 
		/// Only use finite or closed ends, infinite ends not supported (yet)
		/// </summary>
		template <storage::type STORAGE_TYPE, ends ENDS>
		struct mixed : utils::math::geometry::shape_flag
			{
			public:
				inline static constexpr storage::type storage_type{STORAGE_TYPE};
				inline static constexpr geometry::ends ends       {ENDS};

				using self_t        = mixed<storage_type, ends>;
				using nonref_self_t = mixed<storage::type::create::owner(), ends>;

				using vertices_t = geometry::ends_aware_vertices<storage_type, ends>;
				vertices_t vertices;

				struct piece_metadata_t
					{
					enum class type_t : uint8_t { segment, bezier_3pt, bezier_4pt, bezier } type;
					size_t end_index;
					};
				using pieces_metadata_t = utils::storage::multiple<storage::storage_type_for<piece_metadata_t, storage_type>, std::dynamic_extent, true>;
				pieces_metadata_t pieces_metadata;

			private:
				void add_or_update_metadata(piece_metadata_t::type_t type) noexcept
					{
					if (pieces_metadata.empty() || pieces_metadata[pieces_metadata.size() - 1].type != type)
						{
						pieces_metadata.storage.emplace_back(type, vertices.size());
						}
					else
						{
						pieces_metadata[pieces_metadata.size() - 1].end_index = vertices.size();
						}
					}

			public:
				mixed(const shape::point& first_point) noexcept : vertices{first_point} {}
				mixed() noexcept = default;

				void clear(const shape::point& first_point) noexcept
					requires(storage_type.is_owner())
					{
					vertices.storage.clear();
					pieces_metadata.storage.clear();

					vertices.storage.emplace_back(first_point);
					}

				void add_segment(const shape::point& point) noexcept
					requires(storage_type.is_owner())
					{
					vertices.storage.emplace_back(point);
					add_or_update_metadata(piece_metadata_t::type_t::segment);
					}
				void add_segments(const std::initializer_list<shape::point>& points) noexcept
					requires(storage_type.is_owner())
					{
					for (const auto& point : points)
						{
						vertices.storage.emplace_back(point);
						}
					add_or_update_metadata(piece_metadata_t::type_t::segment);
					}

				void add_bezier_3pt(const shape::point& b, const shape::point& c) noexcept
					requires(storage_type.is_owner())
					{
					vertices.storage.emplace_back(b);
					vertices.storage.emplace_back(c);
					add_or_update_metadata(piece_metadata_t::type_t::bezier_3pt);
					}
				void add_bezier_3pt(const std::initializer_list<shape::point>& points) noexcept
					requires(storage_type.is_owner())
					{
					assert((points.size() % 2) == 0);
					vertices.storage.reserve(vertices.storage.size() + points.size());
					for (const auto& point : points)
						{
						vertices.storage.emplace_back(point);
						}
					add_or_update_metadata(piece_metadata_t::type_t::bezier_3pt);
					}

				void add_bezier_4pt(const shape::point& b, const shape::point& c, const shape::point& d) noexcept
					requires(storage_type.is_owner())
					{
					vertices.storage.emplace_back(b);
					vertices.storage.emplace_back(c);
					add_or_update_metadata(piece_metadata_t::type_t::bezier_4pt);
					}
				void add_bezier_4pt(const std::initializer_list<shape::point>& points) noexcept
					requires(storage_type.is_owner())
					{
					assert((points.size() % 3) == 0);
					vertices.storage.reserve(vertices.storage.size() + points.size());
					for (const auto& point : points)
						{
						vertices.storage.emplace_back(point);
						}
					add_or_update_metadata(piece_metadata_t::type_t::bezier_4pt);
					}

				void add_bezier(const std::initializer_list<shape::point>& points) noexcept
					requires(storage_type.is_owner())
					{
					if (points.size() == 2) { add_bezier_3pt(points); return; }
					if (points.size() == 3) { add_bezier_4pt(points); return; }
					assert(points.size() > 3);

					vertices.storage.reserve(vertices.storage.size() + points.size());
					for (const auto& point : points)
						{
						vertices.storage.emplace_back(point);
						}
					pieces_metadata.storage.emplace_back(piece_metadata_t::type_t::bezier_4pt, vertices.size());
					}

				struct pieces_view : ::utils::oop::non_copyable, ::utils::oop::non_movable
					{
					public:
						pieces_view(const self_t& mixed) : mixed_ref{mixed} {}
						const self_t& mixed_ref;

						using edge_t       = utils::math::geometry::shape::segment;
						using bezier_3pt_t = utils::math::geometry::shape::bezier <3                  >;
						using bezier_4pt_t = utils::math::geometry::shape::bezier <4                  >;
						using bezier_t     = utils::math::geometry::shape::bezier <std::dynamic_extent>;

						utils_gpu_available constexpr size_t metadata_size() const noexcept
							{
							return mixed_ref.pieces_metadata.size();
							}
						utils_gpu_available constexpr size_t empty() const noexcept
							{
							return mixed_ref.pieces_metadata.empty();
							}

						utils_gpu_available constexpr void for_each(details::edges_callable auto callback) noexcept
							{
							if (metadata_size() == 0) { return; }

							size_t index_vertex{0};
							for (size_t index_metadata{0}; index_metadata < metadata_size(); index_metadata++)
								{
								const piece_metadata_t metadata{mixed_ref.pieces_metadata[index_metadata]};

								switch (metadata.type)
									{
									case piece_metadata_t::type_t::segment   : call_segments   (index_vertex, metadata.end_index, callback); break;
									case piece_metadata_t::type_t::bezier_3pt: call_bezier_3pts(index_vertex, metadata.end_index, callback); break;
									case piece_metadata_t::type_t::bezier_4pt: call_bezier_4pts(index_vertex, metadata.end_index, callback); break;
									case piece_metadata_t::type_t::bezier    : call_bezier     (index_vertex, metadata.end_index, callback); break;
									}
								index_vertex = metadata.end_index;
								}
							if (mixed_ref.ends.is_closed())
								{
								const shape::segment piece{mixed_ref.vertices[index_vertex - 1], mixed_ref.vertices[0]};
								call(callback, piece, index_vertex - 1);
								}
							}

					private:
						template <details::edges_callable callback_t>
						void call(callback_t callback, const auto& piece, size_t index) noexcept
							{
							if constexpr (details::edges_callable_with_index<callback_t>)
								{
								callback(piece, index);
								}
							else if constexpr (details::edges_callable_without_index<callback_t>)
								{
								callback(piece);
								}
							};

						template <typename callback_t>
						utils_gpu_available constexpr void call_segments(size_t index_vertex_begin, size_t index_vertex_end, callback_t callback) noexcept
							{
							const size_t vertices_count{index_vertex_end - index_vertex_begin};
							const size_t pieces_count{vertices_count - 1};

							for (size_t i{0}; i < pieces_count; i++)
								{
								const size_t index_a{index_vertex_begin + i};
								const size_t index_b{index_a + 1};
								const auto vertex_a{mixed_ref.vertices[index_a]};
								const auto vertex_b{mixed_ref.vertices[index_b]};
								const shape::segment piece{vertex_a, vertex_b};
								call(callback, piece, i);
								}
							}
				
						template <typename callback_t>
						utils_gpu_available constexpr void call_bezier_3pts(size_t index_vertex_begin, size_t index_vertex_end, callback_t callback) noexcept
							{
							const size_t vertices_count{index_vertex_end - index_vertex_begin};
							const size_t pieces_count{(vertices_count - 1) / 2};
							assert((vertices_count - 1) % 2 == 0);

							for (size_t i{index_vertex_begin}; i < index_vertex_end; i += 2)
								{
								const size_t index_a{i};
								const size_t index_b{index_a + 1};
								const size_t index_c{index_b + 1};
								const auto vertex_a{mixed_ref.vertices[index_a]};
								const auto vertex_b{mixed_ref.vertices[index_b]};
								const auto vertex_c{mixed_ref.vertices[index_c]};
								const shape::bezier<3> piece{.vertices{vertex_a, vertex_b, vertex_c}};
								call(callback, piece, i);
								}
							}

						template <typename callback_t>
						utils_gpu_available constexpr void call_bezier_4pts(size_t index_vertex_begin, size_t index_vertex_end, callback_t callback)
							{
							const size_t vertices_count{index_vertex_end - index_vertex_begin};
							const size_t pieces_count{(vertices_count - 1) / 3};
							assert((vertices_count - 1) % 3 == 0);

							for (size_t i{index_vertex_begin}; i < index_vertex_end; i += 3)
								{
								const size_t index_a{index_vertex_begin + i};
								const size_t index_b{index_a + 1};
								const size_t index_c{index_c + 1};
								const size_t index_d{index_b + 1};
								const auto vertex_a{mixed_ref.vertices[index_a]};
								const auto vertex_b{mixed_ref.vertices[index_b]};
								const auto vertex_c{mixed_ref.vertices[index_c]};
								const auto vertex_d{mixed_ref.vertices[index_d]};
								const shape::bezier<4> piece{.vertices{vertex_a, vertex_b, vertex_c, vertex_d}};
								call(callback, piece, i);
								}
							}

						template <typename callback_t>
						utils_gpu_available constexpr void call_bezier(size_t index_vertex_begin, size_t index_vertex_last, callback_t callback)
							{
							const size_t index_vertex_end{index_vertex_last + 1};
							const size_t vertices_count{index_vertex_end - index_vertex_begin};
					
							shape::const_observer::bezier<std::dynamic_extent> piece{.vertices{mixed_ref.vertices.begin() + index_vertex_begin, vertices_count}};
							call(callback, piece, index_vertex_begin);
							}
					};

				utils_gpu_available constexpr auto get_pieces() const noexcept { return pieces_view{*this}; }

			};
		}

	namespace concepts
		{
		template <typename T>
		concept mixed = std::derived_from<T, shape::generic::mixed<T::storage_type, T::ends>>;
		}

	namespace owner 
		{
		template <geometry::ends ends = geometry::ends::create::closed()>
		using mixed = shape::generic::mixed<storage::type::create::owner(), ends>;
		}
	namespace observer
		{
		template <geometry::ends ends = geometry::ends::create::closed()>
		using mixed = shape::generic::mixed<storage::type::create::observer(), ends>;
		}
	namespace const_observer
		{
		template <geometry::ends ends = geometry::ends::create::closed()>
		using mixed = shape::generic::mixed<storage::type::create::const_observer(), ends>;
		}
	}

	static_assert(utils::math::geometry::shape::concepts::shape
		<
		utils::math::geometry::shape::mixed<>
		>);
	static_assert(utils::math::geometry::shape::concepts::mixed
		<
		utils::math::geometry::shape::mixed<>
		>);
	static_assert(utils::math::geometry::shape::concepts::has_vertices
		<
		utils::math::geometry::shape::mixed<>
		>);