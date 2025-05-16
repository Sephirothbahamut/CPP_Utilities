#pragma once

#include <span>

#include "declaration/mixed.h"
#include "bezier.h"
#include "vertices.h"

#include "../interactions/ab_ab.h"
#include "../interactions/ab_bezier.h"
#include "../interactions/bezier_bezier.h"

namespace utils::math::geometry::shape::generic
	{
	/// <summary> 
	/// Only use finite or closed ends, infinite ends not supported (yet)
	/// Functions that assemble the mixed adding pieces dynamically are unsupported on the GPU side on purpose.
	/// On the GPU side this should only exist as an observer.
	/// </summary>
	template <storage::type STORAGE_TYPE, ends::closeable ENDS>
	struct utils_oop_empty_bases mixed : utils::math::geometry::shape_flag, utils::math::geometry::vertices_as_field<geometry::ends_aware_vertices<STORAGE_TYPE, ENDS.is_closed(), std::dynamic_extent>>
		{
		public:
			inline static constexpr auto storage_type{STORAGE_TYPE};
			inline static constexpr auto ends        {ENDS};

			using self_t                = mixed<storage_type                           , ends>;
			using owner_self_t          = mixed<storage::type::create::owner         (), ends>;
			using observer_self_t       = mixed<storage::type::create::observer      (), ends>;
			using const_observer_self_t = mixed<storage::type::create::const_observer(), ends>;

			//TODO
			//const_observer_self_t create_observer() const noexcept { return {*this}; }
			//      observer_self_t create_observer()       noexcept { return {*this}; }


			using          utils::math::geometry::vertices_as_field<geometry::ends_aware_vertices<storage_type, ends.is_closed(), std::dynamic_extent>>::vertices;
			using typename utils::math::geometry::vertices_as_field<geometry::ends_aware_vertices<storage_type, ends.is_closed(), std::dynamic_extent>>::vertices_t;

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
			constexpr mixed(const shape::point& first_point) noexcept { vertices.storage.emplace_back(first_point); }
			constexpr mixed() noexcept = default;

			//TODO
			//utils_gpu_available constexpr mixed(const concepts::mixed auto& other) 
			//	requires(storage_type.can_construct_from_const()) :
			//	utils::math::geometry::vertices_as_field<geometry::ends_aware_vertices<STORAGE_TYPE, ENDS.is_closed(), std::dynamic_extent>>{other.vertices},
			//	//pieces_metadata{other.pieces_metadata} 
			//	{}
			//
			//utils_gpu_available constexpr mixed(concepts::mixed auto& other) 
			//	requires(storage::constness_matching<self_t, decltype(other)>::compatible_constness) :
			//	utils::math::geometry::vertices_as_field<geometry::ends_aware_vertices<STORAGE_TYPE, ENDS.is_closed(), std::dynamic_extent>>{other.vertices},
			//	pieces_metadata{other.pieces_metadata} 
			//	{}

			auto& clear(const shape::point& first_point) noexcept
				requires(storage_type.is_owner())
				{
				vertices.storage.clear();
				pieces_metadata.storage.clear();

				vertices.storage.emplace_back(first_point);
				return *this;
				}

			auto& add_segment(const shape::point& point) noexcept
				requires(storage_type.is_owner())
				{
				vertices.storage.emplace_back(point);
				add_or_update_metadata(piece_metadata_t::type_t::segment);
				return *this;
				}
			auto& add_segment_cutting_intersection_with_last_element(const shape::point& point) noexcept
				requires(storage_type.is_owner())
				{
				creation_exec_with_last([&](auto shape_a)
					{
					if constexpr (concepts::ab<std::remove_cvref_t<decltype(shape_a)>>)
						{
						//shouldn't happen
						}
					if constexpr (concepts::bezier<std::remove_cvref_t<decltype(shape_a)>>)
						{
						const shape::segment shape_b{vertices[vertices.size() - 1], point};
						const std::pair<float, float> intersections_ts{geometry::interactions(shape_a, shape_b).intersection_ts_approximate_first()};
						if (intersections_ts.first != utils::math::constants::fnan)
							{
							const auto split_a_first_part{shape_a.partition(intersections_ts.first).first};
							for (size_t i{0}; i < shape_a.vertices.size(); i++)
								{
								shape_a.vertices[i] = split_a_first_part.vertices[i];
								}
							}
						}
					});

				return add_segment(point);
				}

			auto& add_segments(const std::initializer_list<shape::point>& points) noexcept
				requires(storage_type.is_owner())
				{
				for (const auto& point : points)
					{
					vertices.storage.emplace_back(point);
					}
				add_or_update_metadata(piece_metadata_t::type_t::segment);
				return *this;
				}

			auto& add_bezier_3pt(const shape::point& b, const shape::point& c) noexcept
				requires(storage_type.is_owner())
				{
				vertices.storage.emplace_back(b);
				vertices.storage.emplace_back(c);
				add_or_update_metadata(piece_metadata_t::type_t::bezier_3pt);
				return *this;
				}

			auto& add_bezier_3pt_cutting_intersection_with_last_element(const shape::point& b, const shape::point& c) noexcept
				requires(storage_type.is_owner())
				{
				creation_exec_with_last([&](auto shape_a)
					{
					const shape::bezier<3> shape_b{vertices[vertices.size() - 1], b, c};
					if constexpr (concepts::ab<std::remove_cvref_t<decltype(shape_a)>>)
						{
						const std::pair<float, float> intersections_ts{geometry::interactions(shape_a, shape_b).intersection_ts_approximate_first()};
						if (intersections_ts.first != utils::math::constants::fnan)
							{
							const auto split_b_second_part{shape_b.partition(intersections_ts.second).second};
							shape_a.b = split_b_second_part.vertices[0];
							return add_bezier_3pt(split_b_second_part.vertices[1], split_b_second_part.vertices[2]);
							}
						}
					if constexpr (concepts::bezier<std::remove_cvref_t<decltype(shape_a)>>)
						{
						const std::pair<float, float> intersections_ts{geometry::interactions(shape_a, shape_b).intersection_ts_approximate_first()};
						if (intersections_ts.first != utils::math::constants::fnan)
							{
							const auto split_a_first_part {shape_b.partition(intersections_ts.first ).first };
							const auto split_b_second_part{shape_b.partition(intersections_ts.second).second};
							shape_a.vertices = split_a_first_part;
							return add_bezier_3pt(split_b_second_part.vertices[1], split_b_second_part.vertices[2]);
							}
						}

					return add_bezier_3pt(b, c);
					});
				return *this;
				}

			auto& add_bezier_3pt(const std::initializer_list<shape::point>& points) noexcept
				requires(storage_type.is_owner())
				{
				const auto qwe{points.size()};
				const auto rty{qwe % 2};
				assert((points.size() % 2) == 0);
				vertices.storage.reserve(vertices.storage.size() + points.size());
				for (const auto& point : points)
					{
					vertices.storage.emplace_back(point);
					}
				add_or_update_metadata(piece_metadata_t::type_t::bezier_3pt);
				return *this;
				}

			auto& add_bezier_4pt(const shape::point& b, const shape::point& c, const shape::point& d) noexcept
				requires(storage_type.is_owner())
				{
				vertices.storage.emplace_back(b);
				vertices.storage.emplace_back(c);
				vertices.storage.emplace_back(d);
				add_or_update_metadata(piece_metadata_t::type_t::bezier_4pt);
				return *this;
				}

			auto& add_bezier_4pt_cutting_intersection_with_last_element(const shape::point& b, const shape::point& c, const shape::point& d) noexcept
				requires(storage_type.is_owner())
				{
				creation_exec_with_last([&](auto shape_a)
					{
					const shape::bezier<4> shape_b{utils::storage::construct_flag_data, vertices[vertices.size() - 1], b, c, d};
					if constexpr (concepts::ab<std::remove_cvref_t<decltype(shape_a)>>)
						{
						const std::pair<float, float> intersections_ts{geometry::interactions(shape_a, shape_b).intersection_ts_approximate_first()};
						if (!std::isnan(intersections_ts.first))
							{
							const auto split_b_second_part{shape_b.partition(intersections_ts.second).second};
							shape_a.b.x() = split_b_second_part.vertices[0].x();
							shape_a.b.y() = split_b_second_part.vertices[0].y();
							return add_bezier_4pt(split_b_second_part.vertices[1], split_b_second_part.vertices[2], split_b_second_part.vertices[3]);
							}
						}
					if constexpr (concepts::bezier<std::remove_cvref_t<decltype(shape_a)>>)
						{
						const std::pair<float, float> intersections_ts{geometry::interactions(shape_a, shape_b).intersection_ts_approximate_first()};
						if (!std::isnan(intersections_ts.first))
							{
							const auto split_a_first_part {shape_a.partition(intersections_ts.first ).first};
							const auto split_b_second_part{shape_b.partition(intersections_ts.second).second};
							for (size_t i{0}; i < shape_a.vertices.size(); i++)
								{
								shape_a.vertices[i] = split_a_first_part.vertices[i];
								}
							return add_bezier_4pt(split_b_second_part.vertices[1], split_b_second_part.vertices[2], split_b_second_part.vertices[3]);
							}
						}
					return add_bezier_4pt(b, c, d);
					});
				return *this;
				}

			auto& add_bezier_4pt(const std::initializer_list<shape::point>& points) noexcept
				requires(storage_type.is_owner())
				{
				assert((points.size() % 3) == 0);
				vertices.storage.reserve(vertices.storage.size() + points.size());
				for (const auto& point : points)
					{
					vertices.storage.emplace_back(point);
					}
				add_or_update_metadata(piece_metadata_t::type_t::bezier_4pt);
				return *this;
				}

			auto& add_bezier(const std::initializer_list<shape::point>& points) noexcept
				requires(storage_type.is_owner())
				{
				if (points.size() == 2) { return add_bezier_3pt(points); }
				if (points.size() == 3) { return add_bezier_4pt(points); }
				assert(points.size() > 3);

				vertices.storage.reserve(vertices.storage.size() + points.size());
				for (const auto& point : points)
					{
					vertices.storage.emplace_back(point);
					}
				pieces_metadata.storage.emplace_back(piece_metadata_t::type_t::bezier_4pt, vertices.size());
				return *this;
				}

			/// <summary> Used to perform operations on the last added element before close()</summary>
			/// <param name="callback">function(observer piece), do not take as reference</param>
			void creation_exec_with_last(auto callback) noexcept
				{
				if (pieces_metadata.size() < 1) { return; }
				const piece_metadata_t& metadata{pieces_metadata[pieces_metadata.size() - 1]};
				const size_t index_end{metadata.end_index};
				switch (metadata.type)
					{
					case piece_metadata_t::type_t::segment   :
						{
						shape::observer::segment piece{vertices.storage[index_end - 2], vertices.storage[index_end - 1]};
						callback(piece);
						break;
						}
					case piece_metadata_t::type_t::bezier_3pt:
						{
						shape::observer::bezier<3> piece{vertices.storage.begin() + (index_end - 3), 3};
						callback(piece);
						break;
						}
					case piece_metadata_t::type_t::bezier_4pt:
						{
						shape::observer::bezier<4> piece{vertices.storage.begin() + (index_end - 4), 4};
						callback(piece);
						break;
						}
					case piece_metadata_t::type_t::bezier:
						{
						assert(false);
						//TODO
						break;
						}
					}
				}

			auto& close() noexcept
				requires(storage_type.is_owner() && ends.is_closed())
				{
				const auto first{vertices[0]};
				const auto last {vertices[vertices.size() - 1]};
				if (utils::math::almost_equal(first.x(), last.x()) && utils::math::almost_equal(first.y(), last.y()))
					{
					vertices.storage.resize(vertices.size() - 1);
					}
				return *this;
				}

			struct pieces_view : ::utils::oop::non_copyable, ::utils::oop::non_movable
				{
				public:
					utils_gpu_available constexpr pieces_view(const self_t& mixed) : mixed_ref{mixed} {}
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
					utils_gpu_available constexpr size_t total_count() const noexcept
						{
						size_t ret{0};
						for_each([&](const auto&) { ret++; });
						return ret;
						}

					utils_gpu_available constexpr void for_each(shape::details::mixed::pieces_callable auto callback) const noexcept
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
							index_vertex = metadata.end_index - 1;
							}
						if (mixed_ref.ends.is_closed() && index_vertex < mixed_ref.vertices.size())
							{
							const shape::segment piece{mixed_ref.vertices[index_vertex], mixed_ref.vertices[0]};
							call(callback, piece, index_vertex, 0);
							}
						}

				private:
					template <shape::details::mixed::pieces_callable callback_t>
					void call(callback_t callback, const auto& piece, size_t first, size_t last) const noexcept
						{
						if constexpr (shape::details::mixed::pieces_callable_with_index<callback_t>)
							{
							callback(piece, first, last);
							}
						else if constexpr (shape::details::mixed::pieces_callable_without_index<callback_t>)
							{
							callback(piece);
							}
						};

					template <typename callback_t>
					utils_gpu_available constexpr void call_segments(size_t index_vertex_begin, size_t index_vertex_end, callback_t callback) const noexcept
						{
						const size_t vertices_count{index_vertex_end - index_vertex_begin};
						const size_t pieces_count{vertices_count - 1};

						for (size_t i{0}; i < pieces_count; i++)
							{
							const size_t index_a{index_vertex_begin + i};
							const size_t index_b{index_a + 1};
							const auto vertex_a{mixed_ref.vertices[index_a]};
							const auto vertex_b{mixed_ref.vertices.ends_aware_access(index_b)};
							const shape::segment piece{vertex_a, vertex_b};
							call(callback, piece, index_a, index_b);
							}
						}
				
					template <typename callback_t>
					utils_gpu_available constexpr void call_bezier_3pts(size_t index_vertex_begin, size_t index_vertex_end, callback_t callback) const noexcept
						{
						const size_t vertices_count{index_vertex_end - index_vertex_begin};
						const size_t pieces_count{(vertices_count - 1) / 2};
						assert((vertices_count - 1) % 2 == 0);

						for (size_t i{index_vertex_begin}; i < index_vertex_end - 1; i += 2)
							{
							const size_t index_a{i};
							const size_t index_b{index_a + 1};
							const size_t index_c{index_b + 1};
							const auto vertex_a{mixed_ref.vertices[index_a]};
							const auto vertex_b{mixed_ref.vertices[index_b]};
							const auto vertex_c{mixed_ref.vertices.ends_aware_access(index_c)};
							const shape::bezier<3> piece{vertex_a, vertex_b, vertex_c};
							call(callback, piece, index_a, index_c);
							}
						}

					template <typename callback_t>
					utils_gpu_available constexpr void call_bezier_4pts(size_t index_vertex_begin, size_t index_vertex_end, callback_t callback) const noexcept
						{
						const size_t vertices_count{index_vertex_end - index_vertex_begin};
						const size_t pieces_count{(vertices_count - 1) / 3};
						//assert((vertices_count - 1) % 3 == 0); //TODO figure out exact math

						for (size_t i{index_vertex_begin}; i < index_vertex_end - 1; i += 3)
							{
							const size_t index_a{i};
							const size_t index_b{index_a + 1};
							const size_t index_c{index_b + 1};
							const size_t index_d{index_c + 1};

							const auto vertex_a{mixed_ref.vertices[index_a]};
							const auto vertex_b{mixed_ref.vertices[index_b]};
							const auto vertex_c{mixed_ref.vertices[index_c]};
							const auto vertex_d{mixed_ref.vertices.ends_aware_access(index_d)};
							const shape::bezier<4> piece{vertex_a, vertex_b, vertex_c, vertex_d};
							call(callback, piece, index_a, index_d);
							}
						}

					template <typename callback_t>
					utils_gpu_available constexpr void call_bezier(size_t index_vertex_begin, size_t index_vertex_last, callback_t callback) const noexcept
						{
						const size_t index_vertex_end{index_vertex_last + 1};
						const size_t vertices_count{index_vertex_end - index_vertex_begin};
					
						//Note: Not "vertices.begin()" because my own iterator can't build a span
						//TODO check why, it's 6 am and i'm too tired to check now.
						shape::const_observer::bezier<std::dynamic_extent> piece{mixed_ref.vertices.storage.begin() + index_vertex_begin, vertices_count};
						call(callback, piece, index_vertex_begin, index_vertex_last);
						}
				};

			utils_gpu_available constexpr auto get_pieces() const noexcept { return pieces_view{*this}; }

			struct sdf_proxy;
			utils_gpu_available constexpr sdf_proxy sdf(const vec<float, 2>& point) const noexcept;
			utils_gpu_available constexpr auto bounding_box() const noexcept;
			utils_gpu_available constexpr auto bounding_circle() const noexcept;
		};
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