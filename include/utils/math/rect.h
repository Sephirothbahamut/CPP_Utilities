#pragma once

#include "vec.h"
#include "transform2.h"
#include "../memory.h"
#include "../storage.h"
#include "../alignment.h"
#include "../compilation/gpu.h"
#include "../math/geometry/details/base_types.h"

namespace utils::math
	{
	template <typename T>
	concept vec_range = std::ranges::range<T> && utils::math::concepts::vec_size<typename T::value_type, 2> && storage::concepts::storage_compatible_with_type<typename T::value_type, float>;

	template <typename T>
	struct rect;

	namespace concepts
		{
		template <typename T>
		concept rect = std::derived_from<T, math::rect<typename T::value_type>>;
		}

	template <typename T = float>
	struct utils_oop_empty_bases rect final : utils::storage::multiple<T, 4, false>, utils::math::geometry::shape_flag
		{
		using storage_t = utils::storage::multiple<T, 4, false>;

		using typename storage_t::value_type;
		using typename storage_t::const_aware_value_type;
		using storage_t::storage_type;
		using vertex_owner    = utils::math::vec2<value_type>;
		using vertex_observer = utils::math::vecref2<const_aware_value_type>;
		
		using self_t                = rect<T>;
		using owner_self_t          = rect<value_type>;
		using observer_self_t       = rect<storage::storage_type_for<value_type, storage::type::create::observer      ()>>;
		using const_observer_self_t = rect<storage::storage_type_for<value_type, storage::type::create::const_observer()>>;
		
		const_observer_self_t create_observer() const noexcept { return {*this}; }
		      observer_self_t create_observer()       noexcept { return {*this}; }

		using utils::storage::multiple<T, 4, false>::multiple;

		struct create : ::utils::oop::non_constructible
			{
			utils_gpu_available inline static constexpr self_t infinite() noexcept
				{
				return {-utils::math::constants::finf, -utils::math::constants::finf, utils::math::constants::finf, utils::math::constants::finf};
				}

			/// <summary> Use as starting point when doing operations such as merging on iteration.</summary>
			utils_gpu_available inline static constexpr self_t inverse_infinite() noexcept
				{
				return {utils::math::constants::finf, utils::math::constants::finf, -utils::math::constants::finf, -utils::math::constants::finf};
				}

			template <std::convertible_to<value_type> T_oth, std::convertible_to<value_type> T2_oth> requires (!utils::concepts::reference<value_type>)
			utils_gpu_available inline static constexpr self_t from_possize(utils::math::vec2<T_oth> position, utils::math::vec2<T2_oth> size) { return {position.x(), position.y(), position.x() + size.x(), position.y() + size.y()}; }
			
			template <std::convertible_to<value_type> T_oth, std::convertible_to<value_type> T2_oth>
			utils_gpu_available inline static constexpr  self_t from_ul_dr  (utils::math::vec2<T_oth> ul, utils::math::vec2<T2_oth> dr) { return {ul.x(), ul.y(), dr.x(), dr.y()}; }

			template <std::convertible_to<value_type> T_oth, std::convertible_to<value_type> T2_oth>
			utils_gpu_available inline static constexpr self_t from_vertices(utils::math::vec2<T_oth>& a, utils::math::vec2<T2_oth>& b)
				requires (storage_type.is_observer())
				{
				return 
					{
					a.x() < b.x() ? a.x() : b.x(),
					a.y() < b.y() ? a.y() : b.y(),
					a.x() > b.x() ? a.x() : b.x(),
					a.y() > b.y() ? a.y() : b.y(),
					};
				}
			template <std::convertible_to<value_type> T_oth>
			utils_gpu_available inline static constexpr self_t from_vertices(const utils::math::vec2<T_oth>& a, const utils::math::vec2<T_oth>& b)
				requires (storage_type.is_owner())
				{
				return 
					{
					a.x() < b.x() ? a.x() : b.x(),
					a.y() < b.y() ? a.y() : b.y(),
					a.x() > b.x() ? a.x() : b.x(),
					a.y() > b.y() ? a.y() : b.y(),
					};
				}

			utils_gpu_available inline static constexpr self_t from_vertices(vec_range auto range)
				requires (storage_type.is_observer())
				{
				const_aware_value_type& first{*range.begin()};
				self_t ret{first.x(), first.y(), first.x(), first.y()};
			
				for (const_aware_value_type& vertex : range)
					{
					if (vertex.x() < ret[0]) { ret.rebind(0, vertex.x()); }
					if (vertex.y() < ret[1]) { ret.rebind(1, vertex.y()); }
					if (vertex.x() > ret[2]) { ret.rebind(2, vertex.x()); }
					if (vertex.y() > ret[3]) { ret.rebind(3, vertex.y()); }
					}
			
				return ret;
				}
			
			utils_gpu_available inline static constexpr self_t from_vertices(vec_range auto range)
				requires (storage_type.is_owner())
				{
				self_t ret{create::inverse_infinite()};
			
				for (const auto& vertex : range)
					{
					if (vertex.x() < ret[0]) { ret[0] = vertex.x(); }
					if (vertex.y() < ret[1]) { ret[1] = vertex.y(); }
					if (vertex.x() > ret[2]) { ret[2] = vertex.x(); }
					if (vertex.y() > ret[3]) { ret[3] = vertex.y(); }
					}
			
				return ret;
				}

			utils_gpu_available inline static constexpr self_t bounding_rect(self_t a, self_t b) noexcept
				requires (storage_type.is_owner())
				{
				return
					{
					utils::math::min(a.ll(), b.ll()),
					utils::math::min(a.up(), b.up()),
					utils::math::max(a.rr(), b.rr()),
					utils::math::max(a.dw(), b.dw())
					};
				}
			utils_gpu_available inline static constexpr self_t bounding_rect(std::initializer_list<self_t> rects) noexcept
				requires (storage_type.is_owner())
				{
				self_t ret{create::infinite()};

				for (const auto& rect : rects)
					{
					ret.ll() = utils::math::min(ret.ll(), rect.ll());
					ret.up() = utils::math::min(ret.up(), rect.up());
					ret.rr() = utils::math::max(ret.rr(), rect.rr());
					ret.dw() = utils::math::max(ret.dw(), rect.dw());
					}

				return ret;
				}
			};

		self_t& merge_self(const self_t& other) noexcept 
			{
			ll() = utils::math::min(ll(), other.ll());
			up() = utils::math::min(up(), other.up());
			rr() = utils::math::max(rr(), other.rr());
			dw() = utils::math::max(dw(), other.dw());
			return *this;
			}
		self_t merge(const self_t& other) const noexcept { self_t tmp{*this}; return tmp.merge_self(other); }

		/// <summary>
		/// Generates a rect of type T with rounded down and up values (top-left rounded down, bottom-right rounded up) that fully encloses this rect.
		/// If T is an unsigned type all negative coordinates will be 0.
		/// </summary>
		template <typename T>
		rect<T> wrapping_round() const noexcept
			{
			const utils::math::rect<T> ret
				{
				utils::math::cast_clamp<T>(utils::math::floor(ll())),
				utils::math::cast_clamp<T>(utils::math::floor(up())),
				utils::math::cast_clamp<T>(utils::math::ceil (rr())),
				utils::math::cast_clamp<T>(utils::math::ceil (dw()))
				};
			return ret;
			}

		utils_gpu_available const const_aware_value_type& ll() const noexcept { return (*this)[0]; }
		utils_gpu_available       const_aware_value_type& ll()       noexcept { return (*this)[0]; }
		utils_gpu_available const const_aware_value_type& up() const noexcept { return (*this)[1]; }
		utils_gpu_available       const_aware_value_type& up()       noexcept { return (*this)[1]; }
		utils_gpu_available const const_aware_value_type& rr() const noexcept { return (*this)[2]; }
		utils_gpu_available       const_aware_value_type& rr()       noexcept { return (*this)[2]; }
		utils_gpu_available const const_aware_value_type& dw() const noexcept { return (*this)[3]; }
		utils_gpu_available       const_aware_value_type& dw()       noexcept { return (*this)[3]; }

#pragma region Proxies
	#pragma region Position
		template <bool is_const>
		class proxy_position_t;
		template <bool is_const>
		class proxy_x_t
			{
			template <typename T2>
			friend struct rect;
			friend class proxy_position_t<is_const>;
			using rect_t = std::conditional_t<is_const, const self_t, self_t>;

			public:
				utils_gpu_available constexpr operator value_type () const noexcept { return r.ll(); }
				utils_gpu_available constexpr operator value_type&() noexcept requires(!is_const) { return utils::remove_reference_v(r.ll()); }

				utils_gpu_available constexpr proxy_x_t& operator=(const value_type& new_value) noexcept
					requires(!is_const)
					{
					value_type previous_width{r.width()};
					r.ll         () = new_value;
					r.proxy_width() = previous_width; 
					return *this; 
					}

				utils_gpu_available constexpr value_type operator+(const value_type& delta ) const noexcept { return static_cast<value_type>(*this) + delta ; }
				utils_gpu_available constexpr value_type operator-(const value_type& delta ) const noexcept { return static_cast<value_type>(*this) - delta ; }
				utils_gpu_available constexpr value_type operator*(const value_type& factor) const noexcept { return static_cast<value_type>(*this) * factor; }
				utils_gpu_available constexpr value_type operator/(const value_type& factor) const noexcept { return static_cast<value_type>(*this) / factor; }

				utils_gpu_available constexpr proxy_x_t& operator+=(const value_type& delta ) noexcept requires(!is_const) { return (*this) = operator+(delta ); }
				utils_gpu_available constexpr proxy_x_t& operator-=(const value_type& delta ) noexcept requires(!is_const) { return (*this) = operator-(delta ); }
				utils_gpu_available constexpr proxy_x_t& operator*=(const value_type& factor) noexcept requires(!is_const) { return (*this) = operator*(factor); }
				utils_gpu_available constexpr proxy_x_t& operator/=(const value_type& factor) noexcept requires(!is_const) { return (*this) = operator/(factor); }

			private:
				utils_gpu_available constexpr proxy_x_t(rect_t& r) : r{r} {}
				rect_t& r;
			};
		template <bool is_const>
		class proxy_y_t
			{
			template <typename T2>
			friend struct rect;
			friend class proxy_position_t<is_const>;
			using rect_t = std::conditional_t<is_const, const self_t, self_t>;

			public:
				utils_gpu_available constexpr operator value_type () const noexcept { return r.up(); }
				utils_gpu_available constexpr operator value_type&() noexcept requires(!is_const) { return utils::remove_reference_v(r.up()); }

				utils_gpu_available constexpr proxy_y_t& operator=(const value_type& new_value) noexcept
					requires(!is_const)
					{
					value_type previous_height{r.height()};
					r.up          () = new_value;
					r.proxy_height() = previous_height;
					return *this;
					}
				
				utils_gpu_available constexpr value_type operator+(const value_type& delta ) const noexcept { return static_cast<value_type>(*this) + delta ; }
				utils_gpu_available constexpr value_type operator-(const value_type& delta ) const noexcept { return static_cast<value_type>(*this) - delta ; }
				utils_gpu_available constexpr value_type operator*(const value_type& factor) const noexcept { return static_cast<value_type>(*this) * factor; }
				utils_gpu_available constexpr value_type operator/(const value_type& factor) const noexcept { return static_cast<value_type>(*this) / factor; }

				utils_gpu_available constexpr proxy_y_t& operator+=(const value_type& delta ) noexcept requires(!is_const) { return (*this) = operator+(delta ); }
				utils_gpu_available constexpr proxy_y_t& operator-=(const value_type& delta ) noexcept requires(!is_const) { return (*this) = operator-(delta ); }
				utils_gpu_available constexpr proxy_y_t& operator*=(const value_type& factor) noexcept requires(!is_const) { return (*this) = operator*(factor); }
				utils_gpu_available constexpr proxy_y_t& operator/=(const value_type& factor) noexcept requires(!is_const) { return (*this) = operator/(factor); }

			private:
				utils_gpu_available constexpr proxy_y_t(rect_t& r) : r{r} {}
				rect_t& r;
			};
		template <bool is_const>
		class proxy_position_t
			{
			template <typename T2>
			friend struct rect;
			using rect_t = std::conditional_t<is_const, const self_t, self_t>;

			public:
				proxy_x_t<is_const> x;
				proxy_y_t<is_const> y;

				utils_gpu_available constexpr operator vertex_owner() const noexcept { return {r.ul()}; }
				utils_gpu_available constexpr operator vertex_observer() noexcept requires(!is_const) { return r.ul(); }

				utils_gpu_available constexpr proxy_position_t& operator=(const vertex_owner& new_value) noexcept
					requires(!is_const)
					{
					x = new_value.x();
					y = new_value.y();
					return *this;
					}
				
				utils_gpu_available constexpr vertex_owner operator+(const vertex_owner& delta ) const noexcept { return static_cast<vertex_owner>(*this) + delta ; }
				utils_gpu_available constexpr vertex_owner operator-(const vertex_owner& delta ) const noexcept { return static_cast<vertex_owner>(*this) - delta ; }
				utils_gpu_available constexpr vertex_owner operator*(const vertex_owner& factor) const noexcept { return static_cast<vertex_owner>(*this) * factor; }
				utils_gpu_available constexpr vertex_owner operator/(const vertex_owner& factor) const noexcept { return static_cast<vertex_owner>(*this) / factor; }

				utils_gpu_available constexpr proxy_position_t& operator+=(const vertex_owner& delta ) noexcept requires(!is_const) { return (*this) = operator+(delta ); }
				utils_gpu_available constexpr proxy_position_t& operator-=(const vertex_owner& delta ) noexcept requires(!is_const) { return (*this) = operator-(delta ); }
				utils_gpu_available constexpr proxy_position_t& operator*=(const vertex_owner& factor) noexcept requires(!is_const) { return (*this) = operator*(factor); }
				utils_gpu_available constexpr proxy_position_t& operator/=(const vertex_owner& factor) noexcept requires(!is_const) { return (*this) = operator/(factor); }

			private:
				utils_gpu_available constexpr proxy_position_t(rect_t& r) : x{r}, y{r}, r{r} {}
				rect_t& r;
			};
	#pragma endregion Position
	#pragma region Size
		template <bool is_const, utils::alignment anchor>
		class proxy_size_t;
		template <bool is_const, utils::alignment::horizontal anchor = utils::alignment::horizontal::left>
		class proxy_width_t
			{
			template <typename T2>
			friend struct rect;
			template <bool is_const2, utils::alignment anchor2>
			friend class proxy_size_t;
			using rect_t = std::conditional_t<is_const, const self_t, self_t>;

			public:
				utils_gpu_available constexpr operator value_type() const noexcept { return r.rr() - r.ll(); }

				utils_gpu_available constexpr proxy_width_t& operator=(const value_type& new_value) noexcept
					requires(!is_const)
					{
					return resize(new_value);
					}

				utils_gpu_available constexpr proxy_width_t& resize(const value_type& new_value) noexcept requires(!is_const) { return resize(anchor, new_value); }
				utils_gpu_available constexpr proxy_width_t& resize(const utils::alignment::horizontal& alternative_anchor, const value_type& new_value) noexcept
					requires(!is_const)
					{
					switch (alternative_anchor)
						{
						case ::utils::alignment::horizontal::left:
							{
							r.rr() = r.ll() + new_value;
							}
							break;
						case ::utils::alignment::horizontal::centre:
							{
							const auto centre{r.centre_x()};
							const auto half_value{new_value / value_type{2}};
							r.ll() = centre - half_value;
							r.rr() = centre + half_value;
							}
							break;
						case ::utils::alignment::horizontal::right:
							{
							r.ll() = r.rr() - new_value;
							}
							break;
						}
					return *this;
					}

				utils_gpu_available constexpr proxy_width_t& scale(const utils::alignment::horizontal& anchor, const value_type& factor) noexcept
					requires(!is_const)
					{
					return resize(anchor, static_cast<value_type>(*this) * factor);
					}

				utils_gpu_available constexpr value_type operator+(const value_type& delta ) const noexcept { return static_cast<value_type>(*this) + delta ; }
				utils_gpu_available constexpr value_type operator-(const value_type& delta ) const noexcept { return static_cast<value_type>(*this) - delta ; }
				utils_gpu_available constexpr value_type operator*(const value_type& factor) const noexcept { return static_cast<value_type>(*this) * factor; }
				utils_gpu_available constexpr value_type operator/(const value_type& factor) const noexcept { return static_cast<value_type>(*this) / factor; }

				utils_gpu_available constexpr proxy_width_t& operator+=(const value_type& delta ) noexcept requires(!is_const) { return (*this) = operator+(delta ); }
				utils_gpu_available constexpr proxy_width_t& operator-=(const value_type& delta ) noexcept requires(!is_const) { return (*this) = operator-(delta ); }
				utils_gpu_available constexpr proxy_width_t& operator*=(const value_type& factor) noexcept requires(!is_const) { return (*this) = operator*(factor); }
				utils_gpu_available constexpr proxy_width_t& operator/=(const value_type& factor) noexcept requires(!is_const) { return (*this) = operator/(factor); }

			private:
				utils_gpu_available constexpr proxy_width_t(rect_t& r) : r{r} {}
				rect_t& r;
			};
		template <bool is_const, utils::alignment::vertical anchor = utils::alignment::vertical::top>
		class proxy_height_t 
			{
			template <typename T2>
			friend struct rect;
			template <bool is_const2, utils::alignment anchor2>
			friend class proxy_size_t;
			using rect_t = std::conditional_t<is_const, const self_t, self_t>;

			public:
				utils_gpu_available constexpr operator value_type() const noexcept { return r.dw() - r.up(); }

				utils_gpu_available constexpr proxy_height_t& operator=(const value_type& new_value) noexcept
					requires(!is_const)
					{
					return resize(new_value);
					}

				utils_gpu_available constexpr proxy_height_t& resize(const value_type& new_value) noexcept requires(!is_const) { return resize(anchor, new_value); }
				utils_gpu_available constexpr proxy_height_t& resize(const utils::alignment::vertical& alternative_anchor, const value_type& new_value) noexcept
					requires(!is_const)
					{
					switch (alternative_anchor)
						{
						case ::utils::alignment::vertical::top:
							{
							r.dw() = r.up() + new_value;
							}
							break;
						case ::utils::alignment::vertical::middle:
							{
							const auto centre{r.centre_y()};
							const auto half_value{new_value / value_type{2}};
							r.up() = centre - half_value;
							r.dw() = centre + half_value;
							}
							break;
						case ::utils::alignment::vertical::bottom:
							{
							r.up() = r.dw() - new_value;
							}
							break;
						}
					return *this;
					}

				utils_gpu_available constexpr proxy_height_t& scale(const utils::alignment::vertical& anchor, const value_type& factor) noexcept
					requires(!is_const)
					{
					return resize(anchor, static_cast<value_type>(*this) * factor);
					}

				utils_gpu_available constexpr value_type operator+(const value_type& delta ) const noexcept { return static_cast<value_type>(*this) + delta ; }
				utils_gpu_available constexpr value_type operator-(const value_type& delta ) const noexcept { return static_cast<value_type>(*this) - delta ; }
				utils_gpu_available constexpr value_type operator*(const value_type& factor) const noexcept { return static_cast<value_type>(*this) * factor; }
				utils_gpu_available constexpr value_type operator/(const value_type& factor) const noexcept { return static_cast<value_type>(*this) / factor; }
				
				utils_gpu_available constexpr proxy_height_t& operator+=(const value_type& delta ) noexcept requires(!is_const) { return (*this) = operator+(delta ); }
				utils_gpu_available constexpr proxy_height_t& operator-=(const value_type& delta ) noexcept requires(!is_const) { return (*this) = operator-(delta ); }
				utils_gpu_available constexpr proxy_height_t& operator*=(const value_type& factor) noexcept requires(!is_const) { return (*this) = operator*(factor); }
				utils_gpu_available constexpr proxy_height_t& operator/=(const value_type& factor) noexcept requires(!is_const) { return (*this) = operator/(factor); }

			private:
				utils_gpu_available constexpr proxy_height_t(rect_t& r) : r{r} {}
				rect_t& r;
			};
		template <bool is_const, utils::alignment anchor = utils::alignment{utils::alignment::horizontal::left, utils::alignment::vertical::top} >
		class proxy_size_t
			{
			template <typename T2>
			friend struct rect;
			using rect_t = std::conditional_t<is_const, const self_t, self_t>;

			public:
				proxy_width_t <is_const, anchor.horizontal_alignment> w;
				proxy_height_t<is_const, anchor.vertical_alignment  > h;

				utils_gpu_available constexpr operator  vertex_owner() const noexcept { return r.dr() - r.ul(); }
				utils_gpu_available constexpr vertex_owner operator*() const noexcept { return r.dr() - r.ul(); }

				utils_gpu_available constexpr proxy_size_t& operator=(const vertex_owner& new_value) noexcept requires(!is_const)
					{
					return resize(new_value);
					}

				utils_gpu_available constexpr proxy_size_t& resize(const vertex_owner& new_value) noexcept requires(!is_const) { return resize(anchor, new_value); }
				utils_gpu_available constexpr proxy_size_t& resize(const utils::alignment& alternative_anchor, const vertex_owner& new_value) noexcept
					requires(!is_const)
					{
					w.resize(alternative_anchor.horizontal_alignment, new_value.x());
					h.resize(alternative_anchor.vertical_alignment  , new_value.y());
					return *this;
					}

				utils_gpu_available constexpr proxy_size_t& scale(const utils::alignment& alternative_anchor, const vertex_owner& factor) noexcept
					requires(!is_const)
					{
					return resize(alternative_anchor, static_cast<vertex_owner>(*this) * factor);
					}

				utils_gpu_available constexpr vertex_owner operator+(const vertex_owner& delta ) const noexcept { return static_cast<vertex_owner>(*this) + delta ; }
				utils_gpu_available constexpr vertex_owner operator-(const vertex_owner& delta ) const noexcept { return static_cast<vertex_owner>(*this) - delta ; }
				utils_gpu_available constexpr vertex_owner operator*(const vertex_owner& factor) const noexcept { return static_cast<vertex_owner>(*this) * factor; }
				utils_gpu_available constexpr vertex_owner operator/(const vertex_owner& factor) const noexcept { return static_cast<vertex_owner>(*this) / factor; }
				utils_gpu_available constexpr vertex_owner operator+(const value_type  & delta ) const noexcept { return static_cast<vertex_owner>(*this) + delta ; }
				utils_gpu_available constexpr vertex_owner operator-(const value_type  & delta ) const noexcept { return static_cast<vertex_owner>(*this) - delta ; }
				utils_gpu_available constexpr vertex_owner operator*(const value_type  & factor) const noexcept { return static_cast<vertex_owner>(*this) * factor; }
				utils_gpu_available constexpr vertex_owner operator/(const value_type  & factor) const noexcept { return static_cast<vertex_owner>(*this) / factor; }
				
				utils_gpu_available constexpr proxy_size_t& operator+=(const vertex_owner& delta ) noexcept requires(!is_const) { return (*this) = operator+(delta ); }
				utils_gpu_available constexpr proxy_size_t& operator-=(const vertex_owner& delta ) noexcept requires(!is_const) { return (*this) = operator-(delta ); }
				utils_gpu_available constexpr proxy_size_t& operator*=(const vertex_owner& factor) noexcept requires(!is_const) { return (*this) = operator*(factor); }
				utils_gpu_available constexpr proxy_size_t& operator/=(const vertex_owner& factor) noexcept requires(!is_const) { return (*this) = operator/(factor); }
				utils_gpu_available constexpr proxy_size_t& operator+=(const value_type  & delta ) noexcept requires(!is_const) { return (*this) = operator+(delta ); }
				utils_gpu_available constexpr proxy_size_t& operator-=(const value_type  & delta ) noexcept requires(!is_const) { return (*this) = operator-(delta ); }
				utils_gpu_available constexpr proxy_size_t& operator*=(const value_type  & factor) noexcept requires(!is_const) { return (*this) = operator*(factor); }
				utils_gpu_available constexpr proxy_size_t& operator/=(const value_type  & factor) noexcept requires(!is_const) { return (*this) = operator/(factor); }

			private:
				proxy_size_t(rect_t& r) : w{r}, h{r}, r{r} {}
				rect_t& r;
			};
	#pragma endregion Size
#pragma endregion Proxies

		utils_gpu_available constexpr bool operator==(const rect& other) const noexcept = default;

#pragma region Accessors
		// Corners
		utils_gpu_available constexpr const vertex_owner ul() const noexcept { return {ll(), up()}; }
		utils_gpu_available constexpr const vertex_owner ur() const noexcept { return {rr(), up()}; }
		utils_gpu_available constexpr const vertex_owner dr() const noexcept { return {rr(), dw()}; }
		utils_gpu_available constexpr const vertex_owner dl() const noexcept { return {ll(), dw()}; }
		utils_gpu_available constexpr vertex_observer    ul()       noexcept { return {ll(), up()}; }
		utils_gpu_available constexpr vertex_observer    ur()       noexcept { return {rr(), up()}; }
		utils_gpu_available constexpr vertex_observer    dr()       noexcept { return {rr(), dw()}; }
		utils_gpu_available constexpr vertex_observer    dl()       noexcept { return {ll(), dw()}; }

		// Pos-size
		utils_gpu_available constexpr const value_type                                           x() const noexcept { return {ll()}; }
		utils_gpu_available constexpr const value_type                                           y() const noexcept { return {up()}; }
		utils_gpu_available constexpr const value_type                                           w() const noexcept { return {rr() - ll()}; }
		utils_gpu_available constexpr const value_type                                           h() const noexcept { return {dw() - up()}; }
		utils_gpu_available constexpr const vertex_owner                                         p() const noexcept { return {ul()}; }
		utils_gpu_available constexpr const vertex_owner                                         s() const noexcept { return {dr() - ul()}; }
		utils_gpu_available constexpr proxy_x_t       <true                              > proxy_x() const noexcept { return {*this}; }
		utils_gpu_available constexpr proxy_y_t       <true                              > proxy_y() const noexcept { return {*this}; }
		utils_gpu_available constexpr proxy_position_t<true                              > proxy_p() const noexcept { return {*this}; }
		utils_gpu_available constexpr proxy_x_t       <storage_t::storage_type.is_const()> proxy_x()       noexcept { return {*this}; }
		utils_gpu_available constexpr proxy_y_t       <storage_t::storage_type.is_const()> proxy_y()       noexcept { return {*this}; }
		utils_gpu_available constexpr proxy_position_t<storage_t::storage_type.is_const()> proxy_p()       noexcept { return {*this}; }

		template<utils::alignment::horizontal alignment = utils::alignment::horizontal::left                                                   > utils_gpu_available constexpr auto proxy_w() const noexcept { return proxy_width_t <true                              , alignment>{*this}; }
		template<utils::alignment::vertical   alignment = utils::alignment::vertical  ::top                                                    > utils_gpu_available constexpr auto proxy_h() const noexcept { return proxy_height_t<true                              , alignment>{*this}; }
		template<utils::alignment             alignment = utils::alignment{utils::alignment::horizontal::left, utils::alignment::vertical::top}> utils_gpu_available constexpr auto proxy_s() const noexcept { return proxy_size_t  <true                              , alignment>{*this}; }
		template<utils::alignment::horizontal alignment = utils::alignment::horizontal::left                                                   > utils_gpu_available constexpr auto proxy_w()       noexcept { return proxy_width_t <storage_t::storage_type.is_const(), alignment>{*this}; }
		template<utils::alignment::vertical   alignment = utils::alignment::vertical  ::top                                                    > utils_gpu_available constexpr auto proxy_h()       noexcept { return proxy_height_t<storage_t::storage_type.is_const(), alignment>{*this}; }
		template<utils::alignment             alignment = utils::alignment{utils::alignment::horizontal::left, utils::alignment::vertical::top}> utils_gpu_available constexpr auto proxy_s()       noexcept { return proxy_size_t  <storage_t::storage_type.is_const(), alignment>{*this}; }

		// Center
		value_type   centre_x()        const noexcept { return ll() + (w() / value_type{2}); }
		value_type   centre_y()        const noexcept { return up() + (h() / value_type{2}); }
		vertex_owner centre  ()        const noexcept { return {centre_x(), centre_y()}; }
		//void set_centre_x(T value) noexcept { auto half_w{get_w() / value_type{2}}; remove_reference_v(ll) = value - half_w; remove_reference_v(rr) = value + half_w; }
		//void set_centre_y(T value){ auto half_h{get_h() / value_type{2}}; remove_reference_v(up) = value - half_h; remove_reference_v(dw()) = value + half_h; }
		//void set_centre(vec2<value_type> value) noexcept { set_centre_x(value.x); set_centre_y(value.y); }
#pragma endregion Accessors

#pragma region Aliases
		// Corners
		utils_gpu_available constexpr auto up_left     () const noexcept { return ul(); }
		utils_gpu_available constexpr auto top_left    () const noexcept { return ul(); }
		utils_gpu_available constexpr auto up_right    () const noexcept { return ur(); }
		utils_gpu_available constexpr auto top_right   () const noexcept { return ur(); }
		utils_gpu_available constexpr auto down_right  () const noexcept { return dr(); }
		utils_gpu_available constexpr auto bottom_right() const noexcept { return dr(); }
		utils_gpu_available constexpr auto down_left   () const noexcept { return dl(); }
		utils_gpu_available constexpr auto bottom_left () const noexcept { return dl(); }
		utils_gpu_available constexpr auto up_left     ()       noexcept { return ul(); }
		utils_gpu_available constexpr auto top_left    ()       noexcept { return ul(); }
		utils_gpu_available constexpr auto up_right    ()       noexcept { return ur(); }
		utils_gpu_available constexpr auto top_right   ()       noexcept { return ur(); }
		utils_gpu_available constexpr auto down_right  ()       noexcept { return dr(); }
		utils_gpu_available constexpr auto bottom_right()       noexcept { return dr(); }
		utils_gpu_available constexpr auto down_left   ()       noexcept { return dl(); }
		utils_gpu_available constexpr auto bottom_left ()       noexcept { return dl(); }

		// Pos-size
		utils_gpu_available constexpr auto       height  () const noexcept { return       h(); }
		utils_gpu_available constexpr auto       width   () const noexcept { return       w(); }
		utils_gpu_available constexpr auto       size    () const noexcept { return       s(); }
		utils_gpu_available constexpr auto       pos     () const noexcept { return       p(); }
		utils_gpu_available constexpr auto       position() const noexcept { return       p(); }
		utils_gpu_available constexpr auto proxy_pos     () const noexcept { return proxy_p(); }
		utils_gpu_available constexpr auto proxy_position() const noexcept { return proxy_p(); }
		utils_gpu_available constexpr auto proxy_pos     ()       noexcept { return proxy_p(); }
		utils_gpu_available constexpr auto proxy_position()       noexcept { return proxy_p(); }
		
		template<utils::alignment::vertical   alignment = utils::alignment::vertical  ::top                                                    > utils_gpu_available constexpr auto proxy_height() const noexcept { return proxy_h<alignment>(); }
		template<utils::alignment::horizontal alignment = utils::alignment::horizontal::left                                                   > utils_gpu_available constexpr auto proxy_width () const noexcept { return proxy_w<alignment>(); }
		template<utils::alignment             alignment = utils::alignment{utils::alignment::horizontal::left, utils::alignment::vertical::top}> utils_gpu_available constexpr auto proxy_size  () const noexcept { return proxy_s<alignment>(); }
		template<utils::alignment::vertical   alignment = utils::alignment::vertical  ::top                                                    > utils_gpu_available constexpr auto proxy_height()       noexcept { return proxy_h<alignment>(); }
		template<utils::alignment::horizontal alignment = utils::alignment::horizontal::left                                                   > utils_gpu_available constexpr auto proxy_width ()       noexcept { return proxy_w<alignment>(); }
		template<utils::alignment             alignment = utils::alignment{utils::alignment::horizontal::left, utils::alignment::vertical::top}> utils_gpu_available constexpr auto proxy_size  ()       noexcept { return proxy_s<alignment>(); }
#pragma endregion Aliases

		utils_gpu_available constexpr bool contains(const concepts::vec_size<2> auto& point) const noexcept { return point.x() >= ll() && point.x() <= rr() && point.y() >= up() && point.y() <= dw(); }
		
		struct sdf_proxy;
		utils_gpu_available constexpr sdf_proxy sdf(const vec2f& point) const noexcept requires(std::same_as<value_type, float>);
		utils_gpu_available constexpr auto bounding_box() const noexcept;
		utils_gpu_available constexpr auto bounding_circle() const noexcept;
		#include "geometry/transform/common_declaration.inline.h"
		};
	}



//namespace utils::output
//	{
//	namespace typeless
//		{
//		template <typename T>
//		inline ::std::ostream& operator<<(::std::ostream& os, const utils::math::rect<T>& aabb)
//			{
//			namespace ucc = utils::console::colour;
//
//			os  << ucc::brace << "(" 
//				<< ucc::type << "left"  << ucc::separ << ": " << ucc::value << aabb.ll() << ucc::separ << ", "
//				<< ucc::type << "up"    << ucc::separ << ": " << ucc::value << aabb.up() << ucc::separ << ", "
//				<< ucc::type << "right" << ucc::separ << ": " << ucc::value << aabb.rr() << ucc::separ << ", "
//				<< ucc::type << "down"  << ucc::separ << ": " << ucc::value << aabb.dw() << ucc::separ
//				<< ucc::brace << ")";
//			return os;
//			}
//		}
//
//	template <typename T>
//	inline ::std::ostream& operator<<(::std::ostream& os, const utils::math::rect<T>& aabb)
//		{
//		namespace ucc = utils::console::colour;
//		os << ucc::type << "rect" << ucc::brace << "<" << ucc::type << typeid(T).name() << ucc::brace << ">";
//		return utils::output::typeless::operator<<(os, aabb);
//		}
//
//	//TODO understand why not working
//	template <typename T> 
//	inline ::std::ostream& operator<<(::std::ostream& os, const typename utils::math::rect<T>::x_proxy& proxy) { return os << static_cast<T>(proxy); }
//	template <typename T> 
//	inline ::std::ostream& operator<<(::std::ostream& os, const typename utils::math::rect<T>::y_proxy& proxy) { return os << static_cast<T>(proxy); }
//	template <typename T> 
//	inline ::std::ostream& operator<<(::std::ostream& os, const typename utils::math::rect<T>::p_proxy& proxy) { return os << static_cast<utils::math::vec2<T>>(proxy); }
//	template <typename T> 
//	inline ::std::ostream& operator<<(::std::ostream& os, const typename utils::math::rect<T>::w_proxy& proxy) { return os << static_cast<T>(proxy); }
//	template <typename T> 
//	inline ::std::ostream& operator<<(::std::ostream& os, const typename utils::math::rect<T>::h_proxy& proxy) { return os << static_cast<T>(proxy); }
//	template <typename T> 
//	inline ::std::ostream& operator<<(::std::ostream& os, const typename utils::math::rect<T>::s_proxy& proxy) { return os << static_cast<utils::math::vec2<T>>(proxy); }
//	}