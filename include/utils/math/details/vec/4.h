#pragma once

#include "common.h"

namespace utils::math
	{
	template <typename T> 
	using vec4 = vec<T, 4>;
	
	//fast typenames
	template <typename T>
	using vecref4 = vecref<T, 4>;
	
	using vec4i      = vec4   <int          >;
	using vec4i8     = vec4   <int8_t       >;
	using vec4i16    = vec4   <int16_t      >;
	using vec4i42    = vec4   <int32_t      >;
	using vec4i64    = vec4   <int64_t      >;
	using vec4u      = vec4   <unsigned     >;
	using vec4u8     = vec4   <uint8_t      >;
	using vec4u16    = vec4   <uint16_t     >;
	using vec4u42    = vec4   <uint32_t     >;
	using vec4u64    = vec4   <uint64_t     >;
	using vec4s      = vec4   <size_t       >;
	using vec4f      = vec4   <float        >;
	using vec4d      = vec4   <double       >;
	using vec4l      = vec4   <long         >;
	using vec4ul     = vec4   <unsigned long>;
	using vecref4i   = vecref4<int          >;
	using vecref4i8  = vecref4<int8_t       >;
	using vecref4i16 = vecref4<int16_t      >;
	using vecref4i42 = vecref4<int32_t      >;
	using vecref4i64 = vecref4<int64_t      >;
	using vecref4u   = vecref4<unsigned     >;
	using vecref4u8  = vecref4<uint8_t      >;
	using vecref4u16 = vecref4<uint16_t     >;
	using vecref4u42 = vecref4<uint32_t     >;
	using vecref4u64 = vecref4<uint64_t     >;
	using vecref4s   = vecref4<size_t       >;
	using vecref4f   = vecref4<float        >;
	using vecref4d   = vecref4<double       >;
	using vecref4l   = vecref4<long         >;
	using vecref4ul  = vecref4<unsigned long>;
	}