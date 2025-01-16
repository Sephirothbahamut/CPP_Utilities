#pragma once

#include <array>
#include <cmath>
#include <functional> //reference_wrapper

#include "../../math.h"
#include "../../../memory.h"
#include "../../../storage.h"
#include "../../../compilation/compiler.h"
#include "../../../oop/disable_move_copy.h"
#include "../../../math/geometry/details/base_types.h"

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
		}

	namespace details
		{
		inline extern constexpr const char name_vec[]{"vec"};

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
	}