#pragma once

#include <ranges>

#include "vec.h"

//https://www.shadertoy.com/view/4t2SzR

namespace utils::math::normal
	{
	namespace blend
		{
		template <std::floating_point T>
		vec3<T> reoriented(vec3<T> a, vec3<T> b)
			{
			inline static constexpr T _1{static_cast<T>(1)};
			inline static constexpr T _2{static_cast<T>(2)};

			// Unpack (see article on why it's not just n*2-1)
			a = a * vec3( _2,  _2,  _2) + vec3(-_1, -_1,  _0);
			b = b * vec3(-_2, -_2,  _2) + vec3( _1,  _1, -_1);

			// Blend
			return a * a.dot(b) / a.z() - b;
			}
		};

	template <std::floating_point T>
	vec3<T> derive_normal_z(vec2<T> pseudonormal)
		{
		inline static constexpr T _1{static_cast<T>(1)};

		return vec3<T>
			{
			pseudonormal.x(),
			pseudonormal.y(),
			std::sqrt(_1 - (pseudonormal.x() * pseudonormal.x()) - (pseudonormal.y() * pseudonormal.y()))
			};
		}
	}