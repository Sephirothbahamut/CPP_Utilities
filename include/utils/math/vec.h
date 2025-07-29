#pragma once

#include <ranges>

#include "details/vec/all.h"
#include "math.h"

#include "../oop/conditional_inheritance.h"

namespace utils::math
	{
	template<typename T, size_t SIZE>
	struct utils_oop_empty_bases vec final : utils::storage::multiple<T, SIZE, false>, utils::oop::type_or_nothing<SIZE == 2 && std::convertible_to<T, float>, utils::math::geometry::shape_flag>
		{
		using base_t = utils::storage::multiple<T, SIZE, false>;

		using base_t::size;
		using base_t::extent;
		using base_t::storage_type;
		using typename base_t::value_type;
		using typename base_t::const_aware_value_type;
		using typename base_t::template_type;

		using self_t                = vec<T                                                                             , extent>;
		using owner_self_t          = vec<value_type                                                                    , extent>;
		using observer_self_t       = vec<storage::storage_type_for<value_type, storage::type::create::observer      ()>, extent>;
		using const_observer_self_t = vec<storage::storage_type_for<value_type, storage::type::create::const_observer()>, extent>;
		
		const_observer_self_t create_observer() const noexcept { return {*this}; }
		      observer_self_t create_observer()       noexcept { return {*this}; }

		using utils::storage::multiple<T, SIZE, false>::multiple;

		utils_gpu_available constexpr vec() noexcept requires(storage_type.is_owner()) : base_t{} {}; //for some reason it doesn't use base_t's default constructor with = default

		template <typename T2, size_t SIZE2>
		utils_gpu_available explicit constexpr operator vec<T2, SIZE2>() const noexcept
			{
			vec<T2, SIZE2> ret;
			const size_t indices{utils::math::min(base_t::size(), ret.base_t::size())};
			for (size_t i{0}; i < indices; i++)
				{
				ret[i] = static_cast<T2>((*this).operator[](i));
				}
			return ret;
			}

		#pragma region fields
		utils_gpu_available constexpr const const_aware_value_type& x() const noexcept requires(extent >= 1) { return (*this)[0]; }
		utils_gpu_available constexpr       const_aware_value_type& x()       noexcept requires(extent >= 1) { return (*this)[0]; }
		utils_gpu_available constexpr const const_aware_value_type& y() const noexcept requires(extent >= 2) { return (*this)[1]; }
		utils_gpu_available constexpr       const_aware_value_type& y()       noexcept requires(extent >= 2) { return (*this)[1]; }
		utils_gpu_available constexpr const const_aware_value_type& z() const noexcept requires(extent >= 3) { return (*this)[2]; }
		utils_gpu_available constexpr       const_aware_value_type& z()       noexcept requires(extent >= 3) { return (*this)[2]; }
		utils_gpu_available constexpr const const_aware_value_type& w() const noexcept requires(extent >= 4) { return (*this)[3]; }
		utils_gpu_available constexpr       const_aware_value_type& w()       noexcept requires(extent >= 4) { return (*this)[3]; }
		#pragma endregion fields

		#pragma region swizzle
		utils_gpu_available constexpr const vec<const       value_type&, 2> xy () const noexcept requires(extent >= 2) { return {(*this)[0], (*this)[1]}; }
		utils_gpu_available constexpr       vec<const_aware_value_type&, 2> xy ()       noexcept requires(extent >= 2) { return {(*this)[0], (*this)[1]}; }
		utils_gpu_available constexpr const vec<const       value_type&, 2> xz () const noexcept requires(extent >= 3) { return {(*this)[0], (*this)[2]}; }
		utils_gpu_available constexpr       vec<const_aware_value_type&, 2> xz ()       noexcept requires(extent >= 3) { return {(*this)[0], (*this)[2]}; }
		utils_gpu_available constexpr const vec<const       value_type&, 2> xw () const noexcept requires(extent >= 4) { return {(*this)[0], (*this)[3]}; }
		utils_gpu_available constexpr       vec<const_aware_value_type&, 2> xw ()       noexcept requires(extent >= 4) { return {(*this)[0], (*this)[3]}; }

		utils_gpu_available constexpr const vec<const       value_type&, 2> yx () const noexcept requires(extent >= 3) { return {(*this)[1], (*this)[0]}; }
		utils_gpu_available constexpr       vec<const_aware_value_type&, 2> yx ()       noexcept requires(extent >= 3) { return {(*this)[1], (*this)[0]}; }
		utils_gpu_available constexpr const vec<const       value_type&, 2> yz () const noexcept requires(extent >= 3) { return {(*this)[1], (*this)[2]}; }
		utils_gpu_available constexpr       vec<const_aware_value_type&, 2> yz ()       noexcept requires(extent >= 3) { return {(*this)[1], (*this)[2]}; }
		utils_gpu_available constexpr const vec<const       value_type&, 2> yw () const noexcept requires(extent >= 4) { return {(*this)[1], (*this)[3]}; }
		utils_gpu_available constexpr       vec<const_aware_value_type&, 2> yw ()       noexcept requires(extent >= 4) { return {(*this)[1], (*this)[3]}; }

		utils_gpu_available constexpr const vec<const       value_type&, 2> zx () const noexcept requires(extent >= 3) { return {(*this)[2], (*this)[0]}; }
		utils_gpu_available constexpr       vec<const_aware_value_type&, 2> zx ()       noexcept requires(extent >= 3) { return {(*this)[2], (*this)[0]}; }
		utils_gpu_available constexpr const vec<const       value_type&, 2> zy () const noexcept requires(extent >= 2) { return {(*this)[2], (*this)[1]}; }
		utils_gpu_available constexpr       vec<const_aware_value_type&, 2> zy ()       noexcept requires(extent >= 2) { return {(*this)[2], (*this)[1]}; }
		utils_gpu_available constexpr const vec<const       value_type&, 2> zw () const noexcept requires(extent >= 4) { return {(*this)[2], (*this)[3]}; }
		utils_gpu_available constexpr       vec<const_aware_value_type&, 2> zw ()       noexcept requires(extent >= 4) { return {(*this)[2], (*this)[3]}; }

		utils_gpu_available constexpr const vec<const       value_type&, 2> wx () const noexcept requires(extent >= 3) { return {(*this)[3], (*this)[0]}; }
		utils_gpu_available constexpr       vec<const_aware_value_type&, 2> wx ()       noexcept requires(extent >= 3) { return {(*this)[3], (*this)[0]}; }
		utils_gpu_available constexpr const vec<const       value_type&, 2> wy () const noexcept requires(extent >= 2) { return {(*this)[3], (*this)[1]}; }
		utils_gpu_available constexpr       vec<const_aware_value_type&, 2> wy ()       noexcept requires(extent >= 2) { return {(*this)[3], (*this)[1]}; }
		utils_gpu_available constexpr const vec<const       value_type&, 2> wz () const noexcept requires(extent >= 4) { return {(*this)[3], (*this)[2]}; }
		utils_gpu_available constexpr       vec<const_aware_value_type&, 2> wz ()       noexcept requires(extent >= 4) { return {(*this)[3], (*this)[2]}; }
		
		utils_gpu_available constexpr       vec<const_aware_value_type&, 3> xyz()       noexcept requires(extent >= 3) { return {(*this)[0], (*this)[1], (*this)[2]}; }
		utils_gpu_available constexpr const vec<const       value_type&, 3> xyz() const noexcept requires(extent >= 3) { return {(*this)[0], (*this)[1], (*this)[2]}; }
		utils_gpu_available constexpr       vec<const_aware_value_type&, 3> xzy()       noexcept requires(extent >= 3) { return {(*this)[0], (*this)[2], (*this)[1]}; }
		utils_gpu_available constexpr const vec<const       value_type&, 3> xzy() const noexcept requires(extent >= 3) { return {(*this)[0], (*this)[2], (*this)[1]}; }
		utils_gpu_available constexpr       vec<const_aware_value_type&, 3> yxz()       noexcept requires(extent >= 3) { return {(*this)[1], (*this)[0], (*this)[2]}; }
		utils_gpu_available constexpr const vec<const       value_type&, 3> yxz() const noexcept requires(extent >= 3) { return {(*this)[1], (*this)[0], (*this)[2]}; }
		utils_gpu_available constexpr       vec<const_aware_value_type&, 3> yzx()       noexcept requires(extent >= 3) { return {(*this)[1], (*this)[2], (*this)[1]}; }
		utils_gpu_available constexpr const vec<const       value_type&, 3> yzx() const noexcept requires(extent >= 3) { return {(*this)[1], (*this)[2], (*this)[1]}; }
		utils_gpu_available constexpr       vec<const_aware_value_type&, 3> zxy()       noexcept requires(extent >= 3) { return {(*this)[2], (*this)[0], (*this)[1]}; }
		utils_gpu_available constexpr const vec<const       value_type&, 3> zxy() const noexcept requires(extent >= 3) { return {(*this)[2], (*this)[0], (*this)[1]}; }
		utils_gpu_available constexpr       vec<const_aware_value_type&, 3> zyx()       noexcept requires(extent >= 3) { return {(*this)[2], (*this)[1], (*this)[0]}; }
		utils_gpu_available constexpr const vec<const       value_type&, 3> zyx() const noexcept requires(extent >= 3) { return {(*this)[2], (*this)[1], (*this)[0]}; }
		#pragma endregion swizzle

		#pragma region distances
		utils_gpu_available constexpr value_type get_length2() const noexcept 
			{
			value_type ret{0}; 
			base_t::for_each([&ret](const auto& value) { ret += value * value; });
			return ret; 
			}
		utils_gpu_available constexpr value_type get_length () const noexcept
			requires (std::floating_point<value_type>)
			{
			return std::sqrt(get_length2()); 
			}

		utils_gpu_available constexpr self_t& set_length(value_type value) noexcept
			requires(!storage_type.is_const() && std::floating_point<value_type>)
			{
			*this = normalize() * value;
			return *this; 
			}

		utils_gpu_available constexpr owner_self_t normalize() const noexcept 
			requires (std::floating_point<value_type>)
			{
			const owner_self_t copy{*this};
			const auto length2{get_length2()};
			if (length2 != value_type{0} && length2 != value_type{1})
				{
				const auto length{std::sqrt(length2)};
				const auto length_inverse{value_type{1} / length};
				const auto ret{copy * length_inverse};
				return ret;
				}
			return copy;
			}
		utils_gpu_available constexpr self_t& normalize_self() noexcept 
			requires(!storage_type.is_const() && std::floating_point<value_type>)
			{ 
			return *this = normalize(); 
			}
		
		/// <summary> Evaluate distance^2 in the size of this vec. Missing coordinates are considered 0. </summary>
		utils_gpu_available static constexpr value_type distance2
			(const utils::storage::concepts::compatible_multiple<self_t> auto& a, const utils::storage::concepts::compatible_multiple<self_t> auto& b) noexcept
			requires (std::floating_point<value_type>)
			{
			constexpr auto sizes{details::pair_sizes<std::remove_cvref_t<decltype(a)>, std::remove_cvref_t<decltype(b)>>()};

			value_type ret{0};
			size_t i{0};
			for (; i < sizes.min; i++)
				{
				value_type tmp{a[i] - b[i]};
				ret += tmp * tmp;
				}
					
			     if constexpr (sizes.a > sizes.b) { for (; i < sizes.a; i++) { ret += a[i] * a[i]; } }
			else if constexpr (sizes.a < sizes.b) { for (; i < sizes.b; i++) { ret += b[i] * b[i]; } }

			return ret;
			}

		utils_gpu_available static constexpr value_type distance2_shared
			(const utils::storage::concepts::compatible_multiple<self_t> auto& a, const utils::storage::concepts::compatible_multiple<self_t> auto& b) noexcept
			requires (std::floating_point<value_type>)
			{
			constexpr auto sizes{details::pair_sizes<std::remove_cvref_t<decltype(a)>, std::remove_cvref_t<decltype(b)>>()};

			value_type ret{0};
			size_t i{0};
			for (; i < sizes.min; i++)
				{
				value_type tmp{a[i] - b[i]};
				ret += tmp * tmp;
				}

			return ret;
			}

		/// <summary> Evaluate distance in the size of this vec. Missing coordinates are considered 0. </summary>
		utils_gpu_available static constexpr value_type distance
			(const utils::storage::concepts::compatible_multiple<self_t> auto& a, const utils::storage::concepts::compatible_multiple<self_t> auto& b) noexcept
			requires (std::floating_point<value_type>)
			{
			return std::sqrt(distance2(a, b)); 
			}

		/// <summary> Evaluate distance in all the axes of the smaller vec. </summary>
		utils_gpu_available static constexpr value_type distance_shared
			(const utils::storage::concepts::compatible_multiple<self_t> auto& a, const utils::storage::concepts::compatible_multiple<self_t> auto& b) noexcept
			requires (std::floating_point<value_type>)
			{
			return std::sqrt(distance_shared2(a, b)); 
			}
		#pragma endregion distances

		utils_gpu_available static constexpr owner_self_t slerp_fast(const self_t& a, const self_t& b, value_type t) noexcept
			requires (std::floating_point<value_type>)
			{
			return utils::math::lerp(a, b, t).normalize() * (utils::math::lerp(a.get_length(), b.get_length(), t));
			}
		utils_gpu_available static constexpr owner_self_t tlerp_fast(const self_t& a, const self_t& b, value_type t) noexcept
			requires (std::floating_point<value_type>)
			{
			return utils::math::lerp(a, b, t).normalize() * std::sqrt(utils::math::lerp(a.get_length2(), b.get_length2(), t));
			}
		utils_gpu_available static constexpr owner_self_t slerp(const self_t& a, const self_t& b, value_type t) noexcept //TODO test
			requires (std::floating_point<value_type>)
			{
			value_type dot   = utils::math::clamp(self_t::dot(a, b), -1.0f, 1.0f);
			value_type theta = std::acos(dot) * t;
			owner_self_t relative_vec = (b - a * dot).normalize();
			return ((a * std::cos(theta)) + (relative_vec * std::sin(theta)));
			}

		struct create : ::utils::oop::non_constructible
			{
			utils_gpu_available static constexpr self_t zero    () noexcept requires(storage_type.is_owner() && extent >= 1) { return {value_type{ 0}}; }

			utils_gpu_available static constexpr self_t rr      () noexcept requires(storage_type.is_owner() && extent == 1) { return {value_type{ 1}}; }
			utils_gpu_available static constexpr self_t ll      () noexcept requires(storage_type.is_owner() && extent == 1) { return {value_type{-1}}; }
			utils_gpu_available static constexpr self_t rr      () noexcept requires(storage_type.is_owner() && extent >  1) { return {value_type{ 1}, value_type{ 0}}; }
			utils_gpu_available static constexpr self_t ll      () noexcept requires(storage_type.is_owner() && extent >  1) { return {value_type{-1}, value_type{ 0}}; }
			utils_gpu_available static constexpr self_t right   () noexcept requires(storage_type.is_owner() && extent >= 1) { return rr(); }
			utils_gpu_available static constexpr self_t left    () noexcept requires(storage_type.is_owner() && extent >= 1) { return ll(); }

			utils_gpu_available static constexpr self_t up      () noexcept requires(storage_type.is_owner() && extent == 2) { return {value_type{ 0}, value_type{-1}}; }
			utils_gpu_available static constexpr self_t dw      () noexcept requires(storage_type.is_owner() && extent == 2) { return {value_type{ 0}, value_type{ 1}}; }
			utils_gpu_available static constexpr self_t up      () noexcept requires(storage_type.is_owner() && extent >  2) { return {value_type{ 0}, value_type{-1}, value_type{ 0}}; }
			utils_gpu_available static constexpr self_t dw      () noexcept requires(storage_type.is_owner() && extent >  2) { return {value_type{ 0}, value_type{ 1}, value_type{ 0}}; }
			utils_gpu_available static constexpr self_t down    () noexcept requires(storage_type.is_owner() && extent >= 2) { return dw(); }

			utils_gpu_available static constexpr self_t fw      () noexcept requires(storage_type.is_owner() && extent == 3) { return {value_type{ 0}, value_type{ 0}, value_type{ 1}}; }
			utils_gpu_available static constexpr self_t bw      () noexcept requires(storage_type.is_owner() && extent == 3) { return {value_type{ 0}, value_type{ 0}, value_type{-1}}; }
			utils_gpu_available static constexpr self_t fw      () noexcept requires(storage_type.is_owner() && extent >  3) { return {value_type{ 0}, value_type{ 0}, value_type{ 1}, value_type{ 0}}; }
			utils_gpu_available static constexpr self_t bw      () noexcept requires(storage_type.is_owner() && extent >  3) { return {value_type{ 0}, value_type{ 0}, value_type{-1}, value_type{ 0}}; }
			utils_gpu_available static constexpr self_t forward () noexcept requires(storage_type.is_owner() && extent >= 3) { return fw(); }
			utils_gpu_available static constexpr self_t backward() noexcept requires(storage_type.is_owner() && extent >= 3) { return bw(); }
			
			utils_gpu_available static constexpr self_t from_angle(const math::angle::concepts::angle auto& angle, const value_type& length = 1) noexcept
				requires(storage_type.is_owner() && extent == 2 && std::floating_point<value_type>)
				{
				auto x{angle.cos() * length};
				auto y{angle.sin() * length};
				return self_t{x, y};
				}
			};
				
		utils_gpu_available static constexpr value_type dot(const self_t& a, const self_t& b) noexcept
			{
			value_type ret{0};
			for (size_t i{0}; i < extent; i++)
				{
				ret += a[i] * b[i];
				} 
			return ret;
			}

		#pragma region angle
			//These operators ghost the parent, so we must re-expose those
			using base_t::operator+;
			using base_t::operator+=;
			using base_t::operator-;
			using base_t::operator-=;
			
			template <typename T2 = float, T2 f_a_v = 360.f>
			utils_gpu_available constexpr math::angle::base<T2, f_a_v> angle(this const auto& self) noexcept { return math::angle::base<T2, f_a_v>::atan2(self.y(), self.x()); }
				

			utils_gpu_available constexpr auto  operator+ (this const auto& self, const math::angle::concepts::angle_operand<decltype(self)> auto& angle) noexcept
				{
				//const auto a1{self.x()};
				//const auto a2{angle.cos()};
				//const auto a{a1 * a2};
				//const auto b{self().y() * angle.sin()};
				//const auto c{a - b};
				//const auto d{self().x() * angle.sin()};
				//const auto e{self().y() * angle.cos()};
				//const auto f{d + e};
				//const owner_self_t ret{c, f};

				const owner_self_t ret
					{
					self.x() * angle.cos() - self.y() * angle.sin(),
					self.x() * angle.sin() + self.y() * angle.cos()
					};
				return ret;
				}
			utils_gpu_available constexpr auto  operator- (this const auto& self, const math::angle::concepts::angle_operand<decltype(self)> auto& angle) noexcept
				{
				const auto nangle{-angle};
				return self.operator+(nangle);
				}
			utils_gpu_available constexpr auto& operator+=(this storage::concepts::non_const auto& self, const math::angle::concepts::angle_operand<decltype(self)> auto& angle) noexcept { return self = self - angle; }
			utils_gpu_available constexpr auto& operator-=(this storage::concepts::non_const auto& self, const math::angle::concepts::angle_operand<decltype(self)> auto& angle) noexcept { return self = self - angle; }



			
			utils_gpu_available constexpr self_t& operator= (const  math::angle::concepts::angle_operand<self_t> auto& angle) noexcept
				{
				const auto previous_length{get_length()};
				const auto new_self{create::from_angle(angle, previous_length)};
				return *this = new_self;
				}
		#pragma endregion angle
		#pragma region 2d
			utils_gpu_available constexpr owner_self_t perpendicular_right           () const noexcept requires(storage_type.is_owner() && extent == 2) { return { y(), -x()}; }
			utils_gpu_available constexpr owner_self_t perpendicular_left            () const noexcept requires(storage_type.is_owner() && extent == 2) { return {-y(),  x()}; }
			utils_gpu_available constexpr owner_self_t perpendicular_clockwise       () const noexcept requires(storage_type.is_owner() && extent == 2) { return perpendicular_right(); }
			utils_gpu_available constexpr owner_self_t perpendicular_counterclockwise() const noexcept requires(storage_type.is_owner() && extent == 2) { return perpendicular_left (); }
		#pragma endregion 2d
		#pragma region geometry
			struct sdf_proxy;
			utils_gpu_available constexpr sdf_proxy sdf(const vec<float, 2>& point) const noexcept requires(std::convertible_to<value_type, float> && extent == 2);
			utils_gpu_available constexpr auto bounding_box   ()          const noexcept requires(std::convertible_to<value_type, float> && extent == 2);
			utils_gpu_available constexpr auto bounding_circle()          const noexcept requires(std::convertible_to<value_type, float> && extent == 2);

			utils_gpu_available constexpr auto& scale_self    (this utils::concepts::non_const auto& self, const float             & scaling    ) noexcept requires(std::convertible_to<value_type, float> && extent == 2 && !std::remove_cvref_t<decltype(self)>::storage_type.is_const());

			utils_gpu_available constexpr auto& scale_self    (this utils::concepts::non_const auto& self, const utils::math::vec2f& scaling    ) noexcept requires(std::convertible_to<value_type, float> && extent == 2 && !std::remove_cvref_t<decltype(self)>::storage_type.is_const());

			//Angle should be angle::concepts::angle, which is correct for clang, but causes an obscure error I couldn't find anything about on MSVC:
			//L_GRAMMAR_abbreviated_function_template_body
			//Replacing it with degf lets MSVC compile
			utils_gpu_available constexpr auto& rotate_self   (this utils::concepts::non_const auto& self, const angle::degf       & rotation   ) noexcept requires(std::convertible_to<value_type, float> && extent == 2 && !std::remove_cvref_t<decltype(self)>::storage_type.is_const());
			utils_gpu_available constexpr auto& translate_self(this utils::concepts::non_const auto& self, const vec2f             & translation) noexcept requires(std::convertible_to<value_type, float> && extent == 2 && !std::remove_cvref_t<decltype(self)>::storage_type.is_const());
		#pragma endregion geometry



		#pragma region size_t
			utils_gpu_available constexpr std::ranges::iota_view<size_t, size_t> indices_range() const noexcept
				requires(std::convertible_to<value_type, size_t>)
				{
				return std::ranges::iota_view<size_t, size_t>(size_t{0}, sizes_to_size());
				}

			utils_gpu_available constexpr size_t sizes_to_size() const noexcept
				requires(std::convertible_to<value_type, size_t>)
				{
				size_t ret{1};
				for (const auto& value : (*this))
					{
					ret *= value;
					}
				return ret;
				}

			utils_gpu_available constexpr size_t coords_to_index(math::vec_s<extent> coords) const noexcept
				requires(std::convertible_to<value_type, size_t>)
				{
				size_t ret{0};
				size_t multiplier{1};

				for (size_t i{0}; i < size(); i++)
					{
					const auto coord{coords [i]};
					const auto size_{(*this)[i]};
					assert(coord < size_);
					ret += coord * multiplier;
					multiplier *= size_;
					}

				return ret;
				}

			//TODO N-dimensional, for now I'm tired and 2d is enough
			utils_gpu_available constexpr vec_s<extent> index_to_coords(size_t index) const noexcept
				requires(std::convertible_to<value_type, size_t> && extent == 2)
				{
				//math::vec_s<size> ret;
				//size_t multiplier{size};
				//
				//for (size_t i{size}; i != 0; i--)
				//	{
				//	multiplier /= self()[i - 1];
				//	ret[i - 1] = index / multiplier;
				//	index -= ret[i - 1] * multiplier;
				//	}
				//
				//return ret;

				return
					{
					index % x(),
					index / x()
					};
				}

			utils_gpu_available constexpr bool validate_coords_as_size(math::vec_s<extent> coords) const noexcept
				requires(std::convertible_to<value_type, size_t>)
				{
				if constexpr (coords.size() > size()) { return false; }

				for (size_t i{0}; i < size(); i++)
					{
					const auto coord{coords [i]};
					const auto size_{(*this)[i]};
					if (coord >= size_) { return false; }
					}

				return true;
				}
		#pragma endregion size_t



		};

	namespace operators
		{
		inline constexpr struct _dot
			{
			template <concepts::vec a_t>
			class _inner;

			template <concepts::vec a_t>
			utils_gpu_available inline friend _inner<a_t> operator<(const a_t& lhs, _dot proxy) noexcept { return {lhs}; }

			template <concepts::vec a_t>
			class _inner
				{
				public:
					template <concepts::vec b_t>
					typename a_t::value_type operator>(const b_t& rhs) const noexcept  { return a_t::dot(lhs, rhs); }
					utils_gpu_available _inner(const a_t& lhs) noexcept : lhs{lhs} {}
				private:
					const a_t& lhs;
				};

			} dot;

		inline constexpr struct _cross
			{
			template <concepts::vec a_t>
			class _inner;

			template <concepts::vec a_t>
			utils_gpu_available inline friend _inner<a_t> operator<(const a_t& lhs, _cross proxy) noexcept { return {lhs}; }

			template <concepts::vec a_t>
			class _inner
				{
				public:
					template <concepts::vec b_t>
					a_t::nonref_derived_t operator>(const b_t& rhs) const noexcept { return lhs * rhs; }
					utils_gpu_available _inner(const a_t& lhs) noexcept : lhs{lhs} {}
				private:
					const a_t& lhs;
				};

			} cross;
		}
	}

namespace utils::math
	{
	template <concepts::vec T>
	utils_gpu_available constexpr auto abs(const T& v) noexcept { return v.for_each_to_new([](const auto& value) { return utils::math::abs(value); }); }

	template <concepts::vec T>
	utils_gpu_available constexpr auto floor(const T& v) noexcept { return v.for_each_to_new([](const auto& value) { return utils::math::floor(value); }); }

	template <concepts::vec T>
	utils_gpu_available constexpr auto ceil(const T& v) noexcept { return v.for_each_to_new([](const auto& value) { return utils::math::ceil(value); }); }

	template <concepts::vec T>
	utils_gpu_available constexpr auto pow(const T& a, const storage::concepts::type_compatible_with_storage<T> auto& b)
		{
		return a.operator_to_new(b, [](const auto& a, const auto& b) { return std::pow(a, b); });
		}

	template <concepts::vec T>
	utils_gpu_available constexpr typename T::owner_self_t pow(const T& a, const storage::concepts::type_compatible_with_storage<T> auto& b)
		{
		return a.operator_to_new(b, [](const auto& a, const auto& b) { return std::pow(a, b); });
		}

	template <concepts::vec T>
	utils_gpu_available constexpr typename T::owner_self_t lerp(const T& a, const T& b, float t)
		{
		typename T::owner_self_t ret;
		for (size_t i = 0; i < T::extent; i++)
			{
			ret[i] = utils::math::lerp(a[i], b[i], t);
			}
		return ret;
		}

	template <concepts::vec T>
	utils_gpu_available constexpr typename T::owner_self_t clamp(const T& in, const T& min, const T& max)
		{
		typename T::owner_self_t ret;
		for (size_t i = 0; i < T::extent; i++)
			{
			ret[i] = utils::math::clamp(in[i], min[i], max[i]);
			}
		return ret;
		}

	template <concepts::vec T>
	utils_gpu_available constexpr typename T::owner_self_t min(const T& a, const T& b)
		{
		return a.operator_to_new(b, [](const auto& a, const auto& b) { return utils::math::min(a, b); });
		}

	template <concepts::vec T>
	utils_gpu_available constexpr typename T::owner_self_t max(const T& a, const T& b)
		{
		return a.operator_to_new(b, [](const auto& a, const auto& b) { return utils::math::max(a, b); });
		}
	
	template <concepts::vec T>
	utils_gpu_available constexpr typename T::owner_self_t clamp(const T& in, const storage::concepts::type_compatible_with_storage<T> auto& min, const storage::concepts::type_compatible_with_storage<T> auto& max)
		{
		typename T::owner_self_t ret;
		for (size_t i = 0; i < T::extent; i++)
			{
			ret[i] = utils::math::clamp(in[i], min, max);
			}
		return ret;
		}

	template <concepts::vec T>
	utils_gpu_available constexpr typename T::owner_self_t min(const T& a, const storage::concepts::type_compatible_with_storage<T> auto& b)
		{
		return a.operator_to_new(b, [](const auto& a, const auto& b) { return utils::math::min(a, b); });
		}

	template <concepts::vec T>
	utils_gpu_available constexpr typename T::owner_self_t max(const T& a, const storage::concepts::type_compatible_with_storage<T> auto& b)
		{
		return a.operator_to_new(b, [](const auto& a, const auto& b) { return utils::math::max(a, b); });
		}
	}