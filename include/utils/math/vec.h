#pragma once

#include <array>
#include <cmath>
#include <functional> //reference_wrapper

#include "math.h"
#include "../memory.h"
#include "../details/vec/all.h"
#include "../oop/disable_move_copy.h"
#include "../math/geometry/details/base_types.h"

namespace utils::math
	{
	template <typename T, size_t size>
	struct vec;
	
	//fast typenames
	template <typename T, size_t size> 
	using vecref = vec<T&, size>;
	
	template <size_t size> using vec_i      = vec   <int          , size>;
	template <size_t size> using vec_i8     = vec   <int8_t       , size>;
	template <size_t size> using vec_i16    = vec   <int16_t      , size>;
	template <size_t size> using vec_i32    = vec   <int32_t      , size>;
	template <size_t size> using vec_i64    = vec   <int64_t      , size>;
	template <size_t size> using vec_u      = vec   <unsigned     , size>;
	template <size_t size> using vec_u8     = vec   <uint8_t      , size>;
	template <size_t size> using vec_u16    = vec   <uint16_t     , size>;
	template <size_t size> using vec_u32    = vec   <uint32_t     , size>;
	template <size_t size> using vec_u64    = vec   <uint64_t     , size>;
	template <size_t size> using vec_s      = vec   <size_t       , size>;
	template <size_t size> using vec_f      = vec   <float        , size>;
	template <size_t size> using vec_d      = vec   <double       , size>;
	template <size_t size> using vec_l      = vec   <long         , size>;
	template <size_t size> using vec_ul     = vec   <unsigned long, size>;
	template <size_t size> using vecref_i   = vecref<int          , size>;
	template <size_t size> using vecref_i8  = vecref<int8_t       , size>;
	template <size_t size> using vecref_i16 = vecref<int16_t      , size>;
	template <size_t size> using vecref_i32 = vecref<int32_t      , size>;
	template <size_t size> using vecref_i64 = vecref<int64_t      , size>;
	template <size_t size> using vecref_u   = vecref<unsigned     , size>;
	template <size_t size> using vecref_u8  = vecref<uint8_t      , size>;
	template <size_t size> using vecref_u16 = vecref<uint16_t     , size>;
	template <size_t size> using vecref_u32 = vecref<uint32_t     , size>;
	template <size_t size> using vecref_u64 = vecref<uint64_t     , size>;
	template <size_t size> using vecref_s   = vecref<size_t       , size>;
	template <size_t size> using vecref_f   = vecref<float        , size>;
	template <size_t size> using vecref_d   = vecref<double       , size>;
	template <size_t size> using vecref_l   = vecref<long         , size>;
	template <size_t size> using vecref_ul  = vecref<unsigned long, size>;
	
	namespace concepts
		{
		template <typename T>
		concept vec = std::derived_from<std::remove_cvref_t<T>, utils::math::vec<typename std::remove_cvref_t<T>::template_type, std::remove_cvref_t<T>::extent>>;

		template <typename T, size_t size>
		concept vec_size = vec<T> && std::remove_cvref_t<T>::extent == size;

		template <typename T, typename value_type>
		concept vec_type = vec<T> && std::same_as<typename std::remove_cvref_t<T>::value_type, value_type>;
		template <typename T, typename value_type>
		concept vec_compatible_type = vec<T> && std::convertible_to<typename std::remove_cvref_t<T>::value_type, value_type>;
		}

	namespace details
		{
		inline extern constexpr const char name_vec[]{"vec"};

		template<typename T, size_t size, template <typename, size_t> class unspecialized_derived_T>
		class utils_oop_empty_bases vec_sized_specialization {};
		template<typename T, size_t size, template <typename, size_t> class unspecialized_derived_T>
		class utils_oop_empty_bases vec_typed_specialization {};
		
		//Forward declare the type/size based extensions, so an error appears if the respective headers weren't included
		//Otherwise this is an ODR violation, or IFNDR, I don't remember, since the same specialization risks not being available in all .cpps
		
		template<typename T, template <typename, size_t> class unspecialized_derived_T>
		class utils_oop_empty_bases vec_sized_specialization<T, 2, unspecialized_derived_T>;
		template<size_t size, template <typename, size_t> class unspecialized_derived_T>
		class utils_oop_empty_bases vec_typed_specialization<size_t, size, unspecialized_derived_T>;


		struct pair_sizes_t
			{
			size_t a{0};
			size_t b{0};
			size_t min{0};
			};

		template <typename a_T, typename b_T>
		consteval pair_sizes_t pair_sizes(const a_T& a, const b_T& b) noexcept
			{
			return
				{
				.a{std::remove_cvref_t<a_T>::extent},
				.b{std::remove_cvref_t<b_T>::extent},
				.min{utils::math::min(a_T::extent, b_T::extent)}
				};
			}

		template <typename a_T, typename b_T>
		consteval pair_sizes_t pair_sizes() noexcept
			{
			return
				{
				.a{std::remove_cvref_t<a_T>::extent},
				.b{std::remove_cvref_t<b_T>::extent},
				.min{utils::math::min(a_T::extent, b_T::extent)}
				};
			}
		}

	template<typename T, size_t SIZE>
	struct utils_oop_empty_bases vec : 
		::utils::details::vector::base<T, SIZE, vec, details::name_vec>,
		details::vec_sized_specialization   <T, SIZE, vec>,
		details::vec_typed_specialization   <T, SIZE, vec>
		{
		using base_t = ::utils::details::vector::base<T, SIZE, vec, details::name_vec>;

		using base_t::size;
		using base_t::extent;
		using base_t::storage_type;
		using typename base_t::self_t;
		using typename base_t::value_type;
		using typename base_t::const_aware_value_type;
		using typename base_t::template_type;
		using typename base_t::nonref_self_t;

		using base_t::base;
		using base_t::operator=;
		utils_gpu_available constexpr vec() noexcept requires(storage_type.is_owner()) : base_t{} {}; //for some reason it doesn't use base_t's default constructor with = default

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
		utils_gpu_available constexpr value_type get_length () const noexcept { return std::sqrt(get_length2()); }

		utils_gpu_available constexpr self_t& set_length(value_type value) noexcept requires(!storage_type.is_const()) { *this = normalize() * value; return *this; }

		utils_gpu_available constexpr nonref_self_t normalize() const noexcept 
			{
			const nonref_self_t copy{*this};
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
		utils_gpu_available constexpr self_t& normalize_self() noexcept requires(!storage_type.is_const()) { return *this = normalize(); }
		
		/// <summary> Evaluate distance^2 in the size of this vec. Missing coordinates are considered 0. </summary>
		template <utils::details::vector::concepts::compatible_vector<self_t> a_T, utils::details::vector::concepts::compatible_vector<self_t> b_T>
		utils_gpu_available static constexpr value_type distance2(const a_T& a, const b_T& b) noexcept
			{
			constexpr auto sizes{details::pair_sizes<a_T, b_T>()};

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

		template <utils::details::vector::concepts::compatible_vector<self_t> a_T, utils::details::vector::concepts::compatible_vector<self_t> b_T>
		utils_gpu_available static constexpr value_type distance2_shared(const a_T& a, const b_T& b) noexcept
			{
			constexpr auto sizes{details::pair_sizes<a_T, decltype(b)>()};

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
		template <utils::details::vector::concepts::compatible_vector<self_t> a_T, utils::details::vector::concepts::compatible_vector<self_t> b_T>
		utils_gpu_available static constexpr value_type distance(const a_T& a, const b_T& b) noexcept
			{
			return std::sqrt(distance2(a, b)); 
			}

		/// <summary> Evaluate distance in all the axes of the smaller vec. </summary>
		template <utils::details::vector::concepts::compatible_vector<self_t> a_T, utils::details::vector::concepts::compatible_vector<self_t> b_T>
		utils_gpu_available static constexpr value_type distance_shared(const a_T& a, const b_T& b) noexcept
			{
			return std::sqrt(distance_shared2(a, b)); 
			}

		utils_gpu_available static constexpr nonref_self_t slerp_fast(const self_t& a, const self_t& b, value_type t) noexcept
			{
			return utils::math::lerp(a, b, t).normalize() * (utils::math::lerp(a.get_length(), b.get_length(), t));
			}
		utils_gpu_available static constexpr nonref_self_t tlerp_fast(const self_t& a, const self_t& b, value_type t) noexcept
			{
			return utils::math::lerp(a, b, t).normalize() * std::sqrt(utils::math::lerp(a.get_length2(), b.get_length2(), t));
			}
		utils_gpu_available static constexpr nonref_self_t slerp(const self_t& a, const self_t& b, value_type t) noexcept //TODO test
			{
			value_type dot = utils::math::clamp(self_t::dot(a, b), -1.0f, 1.0f);
			value_type theta = std::acos(dot) * t;
			nonref_self_t relative_vec = (b - a * dot).normalize();
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
			};
				
		template <utils::details::vector::concepts::compatible_vector<self_t> b_t>
		utils_gpu_available static constexpr value_type dot(const self_t& a, const b_t& b) noexcept
			{
			value_type ret{0};
			for (size_t i{0}; i < extent; i++)
				{
				ret += a[i] * b[i];
				} 
			return ret;
			}

		utils_gpu_available constexpr auto& scale_self(const float& scaling) noexcept;
		utils_gpu_available constexpr auto& rotate_self(const angle::concepts::angle auto& rotation) noexcept;
		utils_gpu_available constexpr auto& translate_self(const vec2f& translation) noexcept;
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

#include "vec2.h"
#include "vec3.h"
#include "vec4.h"
#include "vec_s.h"



