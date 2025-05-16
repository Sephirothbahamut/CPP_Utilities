#pragma once

#include <cassert>

#include "declaration/bezier.h"
#include "ab.h"

// Note: exact distance for 4 control points here: https://github.com/nrtaylor/CubicSplineClosestPoint/tree/master
// TODO: understand math and get exact t parameter instead of just distance so we can get tangent and normal as well
// The current 4 control points solution is approximated and slow.

namespace utils::math::geometry::shape::generic
	{
	template<storage::type STORAGE_TYPE, size_t EXTENT, geometry::ends::optional_ab OPTIONAL_ENDS>
	struct utils_oop_empty_bases bezier : geometry::piece_flag, geometry::shape_flag, utils::math::geometry::vertices_as_field<geometry::vertices<STORAGE_TYPE, EXTENT>>
		{
		inline static constexpr auto storage_type {STORAGE_TYPE};
		inline static constexpr auto extent       {EXTENT};
		inline static constexpr auto optional_ends{OPTIONAL_ENDS};

		using self_t                = bezier<storage_type, extent, optional_ends>;
		using owner_self_t          = bezier<storage::type::create::owner         (), extent, optional_ends>;
		using observer_self_t       = bezier<storage::type::create::observer      (), extent, optional_ends>;
		using const_observer_self_t = bezier<storage::type::create::const_observer(), extent, optional_ends>;

		//TODO
		//const_observer_self_t create_observer() const noexcept { return {*this}; }
		//      observer_self_t create_observer()       noexcept { return {*this}; }

		using          utils::math::geometry::vertices_as_field<geometry::vertices<STORAGE_TYPE, EXTENT>>::vertices;
		using typename utils::math::geometry::vertices_as_field<geometry::vertices<STORAGE_TYPE, EXTENT>>::vertices_t;
		using          utils::math::geometry::vertices_as_field<geometry::vertices<STORAGE_TYPE, EXTENT>>::vertices_as_field;

		struct at_proxy
			{
			friend struct bezier;
			public:
				utils_gpu_available constexpr vec2f point() const noexcept
					{
					const auto& vertices{bezier_curve.vertices};
					if (t == 0.f) { return vertices[0]; }
					if (t == 1.f) { return vertices[vertices.size() - 1]; };

					if (vertices.size() == size_t{3})
						{
						const float inverse_t{1.f - t};
						return vertices[0] * inverse_t * inverse_t + vertices[1] * 2.f * t * inverse_t + vertices[2] * t * t;
						}
					else if (vertices.size() == size_t{4})
						{
						const float inverse_t{1.f - t};
						return vertices[0] * std::pow(inverse_t, 3.f) + vertices[1] * 3.f * t * std::pow(inverse_t, 2.f) + vertices[2] * 3.f * std::pow(t, 2.f) * inverse_t + vertices[3] * std::pow(t, 3.f);
						const auto coefficients{bezier_curve.coefficients()};
						//return (coefficients[0] * t * t * t + coefficients[1] * t * t + coefficients[2] * t + coefficients[3]);
						}
					else
						{
						using tmp_t = std::conditional_t<EXTENT == std::dynamic_extent, std::vector<utils::math::vec2f>, std::array<utils::math::vec2f, EXTENT>>;
						tmp_t tmp;
						if constexpr (EXTENT == std::dynamic_extent)
							{
							tmp.resize(vertices.size());
							}
						for (size_t i{0}; i < vertices.size(); i++)
							{
							tmp[i] = vertices[i];
							}

						size_t i{vertices.size() - 1};
						while (i > 0)
							{
							for (size_t k{0}; k < i; k++)
								{
								tmp[k] = tmp[k] + ((tmp[k + 1] - tmp[k]) * t);
								}
							i--;
							}
						return tmp[0];
						}
					assert(false && "Unsupported amount of control points.");
					}
				utils_gpu_available constexpr vec2f tangent() const noexcept
					{
					const auto& vertices{bezier_curve.vertices};
					if (t == 0.f) { return vertices[1] - vertices[0]; }
					if (t == 1.f) { return vertices[vertices.size() - 1] - vertices[vertices.size() - 2]; };

					if (vertices.size() == size_t{3})
						{
						return (((vertices[0] * (t - 1.f)) + (vertices[1] * (1.f - 2.f * t)) + vertices[2] * t) * 2.f).normalize();
						}
					else if (vertices.size() == size_t{4})
						{
						const auto coefficients{bezier_curve.coefficients()};
						return ((coefficients[0] * 3.0f * t * t) + (coefficients[1] * 2.0f * t) + coefficients[2]);
						}

					assert(false && "Unsupported amount of control points.");
					std::unreachable();
					}
				utils_gpu_available constexpr vec2f normal() const noexcept
					{
					return tangent().perpendicular_left();
					}

				utils_gpu_available constexpr operator vec2f() const noexcept { return point(); }

			private:
				utils_gpu_available constexpr at_proxy(const self_t& bezier_curve, const float t) : bezier_curve{bezier_curve}, t{t} {}
				const self_t& bezier_curve;
				const float t;
			};

		using coefficients_t = utils::storage::multiple<storage::storage_type_for<geometry::shape::point, utils::storage::type::create::owner()>, extent, true>;
		utils_gpu_available constexpr coefficients_t coefficients() const noexcept
			{
			coefficients_t ret{utils::storage::construct_flag_size, vertices.size()};

			if (vertices.size() == size_t{3})
				{
				}
			else if (vertices.size() == size_t{4})
				{
				ret[0] =  vertices[3]         - (vertices[2] * 3.0f) + (vertices[1] * 3.0f) - vertices[0];
				ret[1] = (vertices[2] * 3.0f) - (vertices[1] * 6.0f) + (vertices[0] * 3.0f);
				ret[2] = (vertices[1] * 3.0f) - (vertices[0] * 3.0f);
				ret[3] =  vertices[0];
				}
			else
				{
				assert(false && "Unsupported amount of control points.");
				}

			return ret;
			}

		utils_gpu_available constexpr const at_proxy at(float t) const noexcept
			{
			return {*this, t};
			}
		utils_gpu_available constexpr const at_proxy at_equidistant(float t) const noexcept
			{
			return {*this, t_to_equidistant_t(t)};
			}

		utils_gpu_available constexpr float t_to_equidistant_t(float t) const noexcept
			{
			//TODO
			assert(false);
			return t;
			}

		utils_gpu_available constexpr float length(float t_min = 0.f, float t_max = 1.f) const noexcept
			{
			if (vertices.size() == 3)
				{
				const utils::math::vec2f b_to_a{vertices[1] - vertices[0]};
				const utils::math::vec2f c_to_b{vertices[2] - vertices[1]};
				const utils::math::vec2f cb_to_ba = c_to_b - b_to_a;

				const float a{utils::math::vec2f::dot(cb_to_ba, cb_to_ba)};
				const float b{utils::math::vec2f::dot(b_to_a  , cb_to_ba)};
				const float c{utils::math::vec2f::dot(b_to_a  , b_to_a  )};

				utils::math::vec_f<4> pre{a * c - b * b, b / a, 2.f * b, 0.f};
				pre.w() = std::sqrt(pre.x());

				const float value_at_min{a * t_min};
				const float value_at_max{a * t_max};

				const float q1{(t_max + pre.y()) * std::sqrt(c + pre.z() * t_max + value_at_max * t_max) - (t_min + pre.y()) * std::sqrt(c + pre.z() * t_min + value_at_min * t_min)};
				const float q2{pre.x() / std::pow(a, 1.5f) * (std::asinh((value_at_max + b) / pre.w()) - std::asinh((value_at_min + b) / pre.w()))};

				return q1 + q2;
				}
			}

		template <bool equidistant>
		struct edges_view : std::ranges::view_interface<edges_view<equidistant>>
			{
			using edge = utils::math::geometry::shape::segment;

			struct iterator : std::contiguous_iterator_tag
				{
				//TODO random access iterator implementation https://en.cppreference.com/w/cpp/iterator/random_access_iterator
				using difference_type   = std::ptrdiff_t;
				using value_type        = edge;
				using pointer           = value_type*;
				using reference         = value_type&;

				utils_gpu_available constexpr iterator() noexcept = default;
				utils_gpu_available constexpr iterator(const edges_view* edges_view_ptr, size_t index = 0) noexcept : edges_view_ptr{edges_view_ptr}, index{index} {}

				utils_gpu_available constexpr edge operator*() const noexcept { return edges_view_ptr->operator[](index); }
				utils_gpu_available constexpr edge operator*()       noexcept { return edges_view_ptr->operator[](index); }
				
				utils_gpu_available constexpr iterator& operator++(   ) noexcept { index++; return *this; }
				utils_gpu_available constexpr iterator& operator--(   ) noexcept { index--; return *this; }
				utils_gpu_available constexpr iterator  operator++(int) noexcept { iterator ret{*this}; ++(*this); return ret; }
				utils_gpu_available constexpr iterator  operator--(int) noexcept { iterator ret{*this}; --(*this); return ret; }
				
				utils_gpu_available friend constexpr bool operator== (const iterator& a, const iterator& b) noexcept { return a.index == b.index && a.edges_view_ptr == b.edges_view_ptr; };
				utils_gpu_available friend constexpr auto operator<=>(const iterator& a, const iterator& b) noexcept { return a.index <=> b.index; }

				const edges_view* edges_view_ptr;
				size_t index{0};
				};
				
			//static_assert(std::bidirectional_iterator<iterator<true>>); //TODO check why non copy constructible if storage inner container is span?
			//static_assert(std::random_access_iterator<iterator>);
			//static_assert(std::condiguous_iterator   <iterator>);
			
			const self_t& bezier_curve_ref;
			size_t subdivisions{1};
				
			utils_gpu_available const float index_to_t(const size_t& index) const noexcept
				{
				const float t{static_cast<float>(index) / static_cast<float>(subdivisions)};
				if constexpr (equidistant) 
					{
					return bezier_curve_ref.t_to_equidistant_t(t);
					}
				return t;
				}
					
			utils_gpu_available edge operator[](const size_t& index) const noexcept                          
				{
				const float t_a{index_to_t(index)};
				const float t_b{index_to_t(index + 1)};
				return edge{bezier_curve_ref.at(t_a).point(), bezier_curve_ref.at(t_b).point()};
				}

			utils_gpu_available constexpr auto begin() const noexcept { return iterator{this, 0     }; }
			utils_gpu_available constexpr auto begin()       noexcept { return iterator{this, 0     }; }
			utils_gpu_available constexpr auto end  () const noexcept { return iterator{this, size()}; }
			utils_gpu_available constexpr auto end  ()       noexcept { return iterator{this, size()}; }
			
			utils_gpu_available constexpr bool   empty() const noexcept { return bezier_curve_ref.size() <= 1; }
			utils_gpu_available constexpr size_t size () const noexcept
				{
				return subdivisions;
				}

			utils_gpu_available constexpr edges_view(const self_t& bezier_curve, size_t subdivisions = 1) : bezier_curve_ref{bezier_curve}, subdivisions{subdivisions} {}
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
		utils_gpu_available constexpr auto get_edges            (size_t divisions) const noexcept { return edges_view<false>{*this, divisions}; }
		utils_gpu_available constexpr auto get_edges_equidistant(size_t divisions) const noexcept { return edges_view<true >{*this, divisions}; }

		inline static constexpr auto partition_ends_first {optional_ends.has_value() ? utils::math::geometry::ends::optional_ab::create::value(utils::math::geometry::ends::ab{optional_ends.value().finite_a, true}) : optional_ends};
		inline static constexpr auto partition_ends_second{optional_ends.has_value() ? utils::math::geometry::ends::optional_ab::create::value(utils::math::geometry::ends::ab{true, optional_ends.value().finite_b}) : optional_ends};

		using partition_first_t  = bezier<utils::storage::type::create::owner(), extent, partition_ends_first >;
		using partition_second_t = bezier<utils::storage::type::create::owner(), extent, partition_ends_second>;
		using partitions_t = std::pair<partition_first_t, partition_second_t>;
		utils_gpu_available constexpr partitions_t partition(const float& t) const noexcept
			{//https://github.com/oysteinmyrmo/bezier
			utils::storage::multiple<utils::math::vec2f, extent, true> l(vertices.size());
			utils::storage::multiple<utils::math::vec2f, extent, true> r(vertices.size());
			l[0] = vertices[0];
			r[0] = vertices[vertices.size() - 1];
			
			utils::storage::multiple<utils::math::vec2f, extent, true> prev{vertices};
			utils::storage::multiple<utils::math::vec2f, extent, true> curr{utils::storage::construct_flag_size_t{}, vertices.size()};
			
			// de Casteljau: https://pomax.github.io/bezierinfo/#splitting
			const size_t N{vertices.size() - 1};
			size_t subs{0};
			while (subs < N)
				{
				for (size_t i{0}; i < N - subs; i++)
					{
					curr[i].x() = (1.f - t) * prev[i].x() + t * prev[i + 1].x();
					curr[i].y() = (1.f - t) * prev[i].y() + t * prev[i + 1].y();
					if (i == 0)
						{
						l[subs + 1] = curr[i];
						}
					if (i == (N - subs - 1))
						{
						r[subs + 1] = curr[i];
						}
					}
				std::swap(prev.storage, curr.storage);//TODO check if .storage is necessary
				subs++;
				}

			partition_first_t  first (vertices.size());
			partition_second_t second(vertices.size());
			for (size_t i{0}; i < vertices.size(); i++)
				{
				first .vertices[i] = l[i];
				second.vertices[i] = r[vertices.size() - 1 - i];
				}
			return {first, second};
			}



		utils_gpu_available constexpr bool is_quadratic_elevated_to_cubic() const noexcept
			requires(extent == 4)
			{
			//If this is 0, this is a quadratic curve represented with cubic control points.
			//If so, store it as quadratic for better algorithms performance.
			const auto quadratic_as_cubic_test{-vertices[0] + vertices[1] * 3.f - vertices[2] * 3.f + vertices[3]};
			return 
				(
				utils::math::almost_equal(quadratic_as_cubic_test.x(), 0.f, 0.000001f * std::max({std::abs(vertices[0].x()), std::abs(vertices[1].x()), std::abs(vertices[2].x()), std::abs(vertices[3].x())})) &&
				utils::math::almost_equal(quadratic_as_cubic_test.y(), 0.f, 0.000001f * std::max({std::abs(vertices[0].y()), std::abs(vertices[1].y()), std::abs(vertices[2].y()), std::abs(vertices[3].y())}))
				);
			}

		utils_gpu_available constexpr bool is_quadratic_elevated_to_cubic() const noexcept
			requires(extent == std::dynamic_extent)
			{
			if (vertices.size() == 4)
				{
				//If this is 0, this is a quadratic curve represented with cubic control points.
				//If so, store it as quadratic for better algorithms performance.
				const auto quadratic_as_cubic_test{-vertices[0] + vertices[1] * 3.f - vertices[2] * 3.f + vertices[3]};
				return 
					(
					utils::math::almost_equal(quadratic_as_cubic_test.x(), 0.f, 0.000001f * std::max({std::abs(vertices[0].x()), std::abs(vertices[1].x()), std::abs(vertices[2].x()), std::abs(vertices[3].x())})) &&
					utils::math::almost_equal(quadratic_as_cubic_test.y(), 0.f, 0.000001f * std::max({std::abs(vertices[0].y()), std::abs(vertices[1].y()), std::abs(vertices[2].y()), std::abs(vertices[3].y())}))
					);
				}
			return false;
			}
		
		utils_gpu_available constexpr bezier<storage::type::create::owner(), 3, optional_ends> revert_quadratic_elevated_to_cubic() const noexcept
			{
			static_assert(extent == 4 || extent == std::dynamic_extent);
			assert(vertices.size() == 4);
		
			// https://stackoverflow.com/questions/3162645/convert-a-quadratic-bezier-to-a-cubic-one
			// CP1 = QP0 + 2 / 3 * (QP1 - QP0)
			// CP2 = QP2 + 2 / 3 * (QP1 - QP2)
		
			// Bringing back my math memories I extracted this inverting the previous formulas
			// QP1 = (CP1 - QP0)3/2 + QP0
			// QP1 = (CP2 - QP2)3/2 + QP2
		
			const auto quadratic_p1_from_cubic_p0{(vertices[1] - vertices[0]) * 3.f / 2.f + vertices[0]};
			const auto quadratic_p1_from_cubic_p3{(vertices[2] - vertices[3]) * 3.f / 2.f + vertices[3]};
			//Since they're not always equal or almost_equal (with math::constants:epsilon) due to
			// sweet floating point error accumulation, I'm averaging them.
			const auto quadratic_p1{(quadratic_p1_from_cubic_p0 + quadratic_p1_from_cubic_p3) / 2.f};
		
			const bezier<storage::type::create::owner(), 3, optional_ends> ret
				{
				vertices[0], quadratic_p1, vertices[3]
				};
			return ret;
			}

		struct sdf_proxy;
		utils_gpu_available constexpr sdf_proxy sdf(const vec<float, 2>& point) const noexcept;
		utils_gpu_available constexpr auto bounding_box() const noexcept;
		utils_gpu_available constexpr auto bounding_circle() const noexcept;
		};
	}

static_assert(utils::math::geometry::shape::concepts::bezier
	<
	utils::math::geometry::shape::bezier<std::dynamic_extent>
	>);
static_assert(utils::math::geometry::shape::concepts::has_vertices
	<
	utils::math::geometry::shape::bezier<std::dynamic_extent>
	>);