#pragma once

#include <span>
#include <array>
#include <vector>
#include <optional>

#include "declaration/polycurve.h"
#include "vertices.h"

namespace utils::math::geometry::shape::generic
	{
	namespace details
		{
		template <typename T>
		concept edges_callable_without_index = requires(T t, shape::segment edge, size_t index) { t(edge); };
		template <typename T>
		concept edges_callable_with_index = requires(T t, shape::segment edge, size_t index) { t(edge, index); };
		template <typename T>
		concept edges_callable = edges_callable_without_index<T> || edges_callable_with_index<T>;
		}

	template <size_t PIECE_VERTICES_COUNT = 4, storage::type STORAGE_TYPE, geometry::ends::closeable ENDS, size_t EXTENT>
		requires(PIECE_VERTICES_COUNT >= 2)
	struct utils_oop_empty_bases polycurve : utils::math::geometry::shape_flag, utils::math::geometry::vertices_as_field<geometry::ends_aware_vertices<STORAGE_TYPE, ENDS.is_closed(), EXTENT>>
		{
		inline static constexpr auto storage_type        {STORAGE_TYPE};
		inline static constexpr auto ends                {ENDS};
		inline static constexpr auto extent              {EXTENT};
		inline static constexpr auto piece_vertices_count{PIECE_VERTICES_COUNT};

		using self_t        = polycurve<piece_t, storage_type, ends, extent>;
		using nonref_self_t = polycurve<piece_t, storage::type::create::owner(), ends, extent>;

		using          utils::math::geometry::vertices_as_field<geometry::ends_aware_vertices<storage_type, ends.is_closed(), extent>>::vertices;
		using typename utils::math::geometry::vertices_as_field<geometry::ends_aware_vertices<storage_type, ends.is_closed(), extent>>::vertices_t;
		using          utils::math::geometry::vertices_as_field<geometry::ends_aware_vertices<storage_type, ends.is_closed(), extent>>::vertices_as_field;


		template <bool is_function_const>
		inline static consteval auto piece_storage_type() { return (is_function_const || storage_type.is_const()) ? storage::type::create::const_observer() : storage::type::create::observer(); }

		template <bool is_function_const, utils::math::geometry::ends::optional_ab optional_ends>
		using piece_t = std::conditional_t
			<
			piece_vertices_count == 2,
			utils::math::geometry::shape::generic::ab<piece_storage_type(is_function_const), optional_ends>,
			utils::math::geometry::shape::generic::bezier<piece_storage_type(is_function_const), piece_vertices_count, optional_ends>
			>;

		template <bool is_view_const>
		struct pieces_view : std::ranges::view_interface<pieces_view<is_view_const>>, utils::oop::non_copyable, utils::oop::non_movable
			{
			using polyline_t = std::conditional_t<is_view_const, const self_t, self_t>;
			polyline_t& polyline_ref{nullptr};

			utils_gpu_available constexpr pieces_view(polyline_t& polycurve) : polyline_ref{polycurve} {}

			template <bool is_iterator_const>
			struct iterator : std::conditional_t<ENDS.open, std::contiguous_iterator_tag, std::random_access_iterator_tag>
				{
				//TODO random access iterator implementation https://en.cppreference.com/w/cpp/iterator/random_access_iterator
					
				using polyline_t = std::conditional_t<is_view_const, const self_t, self_t>;
				polyline_t* polyline_ptr{nullptr};
					
				//using iterator_category = std::conditional_t<ENDS.open, std::contiguous_iterator_tag, std::random_access_iterator_tag>;
				using difference_type   = std::ptrdiff_t;
				using value_type        = piece_t<is_iterator_const, utils::math::geometry::ends::optional_ab::create::empty()>;
				using pointer           = value_type*;
				using reference         = value_type&;

				utils_gpu_available constexpr iterator() noexcept = default;
				utils_gpu_available constexpr iterator(polyline_t& polycurve, size_t index = 0) noexcept : polyline_ptr{&polycurve}, index{index} {}

				utils_gpu_available constexpr auto operator*() const noexcept { return piece_t<true             >{polyline_ptr->ends_aware_access(index), polyline_ptr->ends_aware_access(index + 1)}; }
				utils_gpu_available constexpr auto operator*()       noexcept { return piece_t<is_iterator_const>{polyline_ptr->ends_aware_access(index), polyline_ptr->ends_aware_access(index + 1)}; }
				
				utils_gpu_available constexpr iterator& operator++(   ) noexcept { index++; return *this; }
				utils_gpu_available constexpr iterator& operator--(   ) noexcept { index--; return *this; }
				utils_gpu_available constexpr iterator  operator++(int) noexcept { iterator ret{*this}; ++(*this); return ret; }
				utils_gpu_available constexpr iterator  operator--(int) noexcept { iterator ret{*this}; --(*this); return ret; }
				
				utils_gpu_available friend constexpr bool operator== (const iterator& a, const iterator& b) noexcept { return a.index == b.index && a.polyline_ptr == b.polyline_ptr; };
				utils_gpu_available friend constexpr auto operator<=>(const iterator& a, const iterator& b) noexcept { return a.index <=> b.index; }
					
				size_t index{0};
				};
				
			//static_assert(std::bidirectional_iterator<iterator<true>>); //TODO check why non copy constructible if storage inner container is span?
			//static_assert(std::random_access_iterator<iterator>);
			//static_assert(std::condiguous_iterator   <iterator>);

			template <bool closed> utils_gpu_available constexpr auto ends_aware_access(size_t index) const noexcept                          { return piece_t<true         >{polyline_ref.vertices.ends_aware_access<closed>(index), polyline_ref.vertices.ends_aware_access<closed>(index + 1)}; }
			template <bool closed> utils_gpu_available constexpr auto ends_aware_access(size_t index)       noexcept requires(!is_view_const) { return piece_t<is_view_const>{polyline_ref.vertices.ends_aware_access<closed>(index), polyline_ref.vertices.ends_aware_access<closed>(index + 1)}; }
			utils_gpu_available constexpr auto ends_aware_access(size_t index) const noexcept                          { return ends_aware_access<polyline_ref.vertices.closed>(index); }
			utils_gpu_available constexpr auto ends_aware_access(size_t index)       noexcept requires(!is_view_const) { return ends_aware_access<polyline_ref.vertices.closed>(index); }


			utils_gpu_available constexpr auto operator[](size_t index) const noexcept                          { return ends_aware_access<true>(index); }
			utils_gpu_available constexpr auto operator[](size_t index)       noexcept requires(!is_view_const) { return ends_aware_access<true>(index); }

			utils_gpu_available constexpr auto begin() const noexcept { return iterator<true         >{&polyline_ref, 0     }; }
			utils_gpu_available constexpr auto begin()       noexcept { return iterator<is_view_const>{&polyline_ref, 0     }; }
			utils_gpu_available constexpr auto end  () const noexcept { return iterator<true         >{&polyline_ref, size()}; }
			utils_gpu_available constexpr auto end  ()       noexcept { return iterator<is_view_const>{&polyline_ref, size()}; }
			
			utils_gpu_available constexpr bool   empty() const noexcept { return polyline_ref.empty() || polyline_ref.size() == 1; }

			template <bool closed> 
			utils_gpu_available constexpr size_t size() const noexcept
				{
				const auto eas{polyline_ref.vertices.size()};
				if constexpr (closed) { return eas; }
				return eas - 1; 
				}
			utils_gpu_available constexpr size_t size() const noexcept { return size<polyline_ref.vertices.closed>(); }
				
			template <bool include_last_if_closed = true>
			utils_gpu_available constexpr void for_each(details::edges_callable auto callback) const noexcept
				{
				if (size() == 0) { return; }

				using callback_t = decltype(callback);
				const auto call{[]<details::edges_callable callback_t, typename edge_t>(callback_t callback, const edge_t & edge, size_t index)
					{
					if constexpr (details::edges_callable_with_index<callback_t>)
						{
						callback(edge, index);
						}
					else if constexpr (details::edges_callable_without_index<callback_t>) 
						{
						callback(edge); 
						}
					}};

				if constexpr (polyline_t::ends.is_a_infinite())
					{
					if constexpr (polycurve::ends.is_b_infinite())
						{
						if (size() == 1)
							{
							const shape::line line{polyline_ref.vertices[0], polyline_ref.vertices[1]};
							call(callback, line, size_t{0});
							return;
							}
						}
						
					const shape::reverse_ray ray{polyline_ref.vertices[0], polyline_ref.vertices[1]};
					call(callback, ray, size_t{0});
					}

				const size_t index_begin{polyline_t::ends.is_a_infinite() ? 1 : 0};
				const size_t index_end  {polyline_ref.vertices.size() - 1 - (polyline_t::ends.is_b_infinite() ? 1 : 0)};

				for (size_t i{index_begin}; i < index_end; i++)
					{
					const shape::segment segment{polyline_ref.vertices[i], polyline_ref.vertices[i + 1]};
					call(callback, segment, i);
					}

				if constexpr (polyline_t::ends.is_b_infinite())
					{
					const shape::ray ray{polyline_ref.vertices[index_end], polyline_ref.vertices[index_end + 1]};
					call(callback, ray, index_end);
					}
				else if constexpr (polyline_t::ends.is_closed() && include_last_if_closed)
					{
					const shape::segment segment{polyline_ref.vertices[index_end], polyline_ref.vertices[0]};
					call(callback, segment, index_end);
					}
				}

			template <bool ends_aware = true>
			utils_gpu_available constexpr shape::point first_point_at(size_t index) const noexcept
				{
				return polyline_ref.vertices.ends_aware_access<ends_aware>(index);
				}
			template <bool ends_aware = true>
			utils_gpu_available constexpr shape::point second_point_at(size_t index) const noexcept
				{
				return polyline_ref.vertices.ends_aware_access<ends_aware>(index + 1);
				}
			template <bool ends_aware = true>
			utils_gpu_available constexpr shape::point last_point_at(size_t index) const noexcept
				{
				return polyline_ref.vertices.ends_aware_access<ends_aware>(index + 1);
				}
			template <bool ends_aware = true>
			utils_gpu_available constexpr shape::point second_last_point_at(size_t index) const noexcept
				{
				return polyline_ref.vertices.ends_aware_access<ends_aware>(index);
				}
			};
			
		/// <summary> 
		/// Usage note: This shape contains vertices, not edges. 
		/// The operator[] and dereferencing an iterator in the edges view will not return a reference to an edge, it will return an actual edge object which itself contains references to the vertices.
		/// So don't write `for(auto& edge : x.get_edges())`, write `for(auto edge : x.get_edges())` instead.
		/// If this shape is const or the edges view is stored in a const variable, edges will be const observers.
		/// Otherwise they will be simple observers, and changing them will affect the vertices of this shape.
		/// A lot of tears and blood were poured into making this seemingly seamless, it's part of the reason I restarted the geometry portion of this library from scratch at least 3 times, 
		/// please appreciate my efforts for such an useless feature nobody will ever need :)
		/// </summary>
		utils_gpu_available constexpr auto get_edges() const noexcept { return pieces_view<true>{*this}; }

		/// <summary> 
		/// Usage note: This shape contains vertices, not edges. 
		/// The operator[] and dereferencing an iterator in the edges view will not return a reference to an edge, it will return an actual edge object which itself contains references to the vertices.
		/// So don't write `for(auto& edge : x.get_edges())`, write `for(auto edge : x.get_edges())` instead.
		/// If this shape is const or the edges view is stored in a const variable, edges will be const observers.
		/// Otherwise they will be simple observers, and changing them will affect the vertices of this shape.
		/// A lot of tears and blood were poured into making this seemingly seamless, it's part of the reason I restarted the geometry portion of this library from scratch at least 3 times, 
		/// please appreciate my efforts for such an useless feature nobody will ever need :)
		/// </summary>
		utils_gpu_available constexpr auto get_edges() noexcept { return pieces_view<storage_type.is_const()>{*this}; }

		#include "../sdf/common_declaration.inline.h"
		#include "../bounds/common_declaration.inline.h"
		};
	}

static_assert(utils::math::geometry::shape::concepts::shape
	<
	utils::math::geometry::shape::polycurve<>
	>);
static_assert(utils::math::geometry::shape::concepts::polycurve
	<
	utils::math::geometry::shape::polycurve<>
	>);
static_assert(utils::math::geometry::shape::concepts::polycurve
	<
	utils::math::geometry::shape::polygon<>
	>);
static_assert(utils::math::geometry::shape::concepts::polygon
	<
	utils::math::geometry::shape::polygon<>
	>);
static_assert(utils::math::geometry::shape::concepts::has_vertices
	<
	utils::math::geometry::shape::polycurve<>
	>);
static_assert(utils::math::geometry::shape::concepts::has_vertices
	<
	utils::math::geometry::shape::polygon<>
	>);