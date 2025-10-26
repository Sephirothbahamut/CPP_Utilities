#pragma once

#include <numeric>
#include <numbers>
#include <concepts>
// Reference https://easings.net/

//TODO write test cases


namespace utils::math::easing
	{
	namespace _
		{
		template <std::floating_point T>
		T ease_in_sine(T t)
			{
			return std::sin(static_cast<T>(1.5707963) * t);
			}

		template <std::floating_point T>
		T ease_out_sine(T t)
			{
			return static_cast<T>(1) + std::sin(static_cast<T>(1.5707963) * (--t));
			}

		template <std::floating_point T>
		T ease_in_out_sine(T t)
			{
			return static_cast<T>(0.5) * (static_cast<T>(1) + std::sin(static_cast<T>(3.1415926) * (t - static_cast<T>(0.5))));
			}

		template <std::floating_point T>
		T ease_in_quad(T t)
			{
			return t * t;
			}

		template <std::floating_point T>
		T ease_out_quad(T t)
			{
			return t * (static_cast<T>(2) - t);
			}

		template <std::floating_point T>
		T ease_in_out_quad(T t)
			{
			return t < static_cast<T>(0.5) ? static_cast<T>(2) * t * t : t * (static_cast<T>(4) - static_cast<T>(2) * t) - static_cast<T>(1);
			}

		template <std::floating_point T>
		T ease_in_cubic(T t)
			{
			return t * t * t;
			}

		template <std::floating_point T>
		T ease_out_cubic(T t)
			{
			return static_cast<T>(1) + (--t) * t * t;
			}

		template <std::floating_point T>
		T ease_in_out_cubic(T t)
			{
			return t < static_cast<T>(0.5) ? static_cast<T>(4) * t * t * t : static_cast<T>(1) + (--t) * (static_cast<T>(2) * (--t)) * (static_cast<T>(2) * t);
			}

		template <std::floating_point T>
		T ease_in_quart(T t)
			{
			t *= t;
			return t * t;
			}

		template <std::floating_point T>
		T ease_out_quart(T t)
			{
			t = (--t) * t;
			return static_cast<T>(1) - t * t;
			}

		template <std::floating_point T>
		T ease_in_out_quart(T t)
			{
			if (t < static_cast<T>(0.5))
				{
				t *= t;
				return static_cast<T>(8) * t * t;
				}
			else
				{
				t = (--t) * t;
				return static_cast<T>(1) - static_cast<T>(8) * t * t;
				}
			}

		template <std::floating_point T>
		T ease_in_quint(T t)
			{
			T t2 = t * t;
			return t * t2 * t2;
			}

		template <std::floating_point T>
		T ease_out_quint(T t)
			{
			T t2 = (--t) * t;
			return 1 + t * t2 * t2;
			}

		template <std::floating_point T>
		T ease_in_out_quint(T t)
			{
			T t2;
			if (t < static_cast<T>(0.5))
				{
				t2 = t * t;
				return static_cast<T>(16) * t * t2 * t2;
				}
			else
				{
				t2 = (--t) * t;
				return static_cast<T>(1) + static_cast<T>(16) * t * t2 * t2;
				}
			}

		template <std::floating_point T>
		T ease_in_expo(T t)
			{
			return (std::pow(static_cast<T>(2), static_cast<T>(8) * t) - static_cast<T>(1)) / static_cast<T>(255);
			}

		template <std::floating_point T>
		T ease_out_expo(T t)
			{
			return static_cast<T>(1) - std::pow(static_cast<T>(2), -static_cast<T>(8) * t);
			}

		template <std::floating_point T>
		T ease_in_out_expo(T t)
			{
			if (t < static_cast<T>(0.5))
				{
				return (std::pow(static_cast<T>(2), static_cast<T>(16) * t) - static_cast<T>(1)) / static_cast<T>(510);
				}
			else
				{
				return static_cast<T>(1) - static_cast<T>(0.5) * std::pow(static_cast<T>(2), -static_cast<T>(16) * (t - static_cast<T>(0.5)));
				}
			}

		template <std::floating_point T>
		T ease_in_circ(T t)
			{
			return static_cast<T>(1) - std::sqrt(static_cast<T>(1) - t);
			}

		template <std::floating_point T>
		T ease_out_circ(T t)
			{
			return std::sqrt(t);
			}

		template <std::floating_point T>
		T ease_in_out_circ(T t)
			{
			if (t < static_cast<T>(0.5))
				{
				return (static_cast<T>(1) - std::sqrt(static_cast<T>(1) - static_cast<T>(2) * t)) * static_cast<T>(0.5);
				}
			else
				{
				return (static_cast<T>(1) + std::sqrt(static_cast<T>(2) * t - static_cast<T>(1))) * static_cast<T>(0.5);
				}
			}

		template <std::floating_point T>
		T ease_in_back(T t)
			{
			return t * t * (static_cast<T>(2.70158) * t - static_cast<T>(1.70158));
			}

		template <std::floating_point T>
		T ease_out_back(T t)
			{
			return static_cast<T>(1) + (--t) * t * (static_cast<T>(2.70158) * t + static_cast<T>(1.70158));
			}

		template <std::floating_point T>
		T ease_in_out_back(T t)
			{
			if (t < static_cast<T>(0.5))
				{
				return t * t * (static_cast<T>(7) * t - static_cast<T>(2.5)) * static_cast<T>(2);
				}
			else
				{
				return static_cast<T>(1) + (--t) * t * static_cast<T>(2) * (static_cast<T>(7) * t + static_cast<T>(2.5));
				}
			}

		template <std::floating_point T>
		T ease_in_elastic(T t)
			{
			T t2 = t * t;
			return t2 * t2 * std::sin(t * static_cast<T>(std::numbers::pi) * static_cast<T>(4.5));
			}

		template <std::floating_point T>
		T ease_out_elastic(T t)
			{
			T t2 = (t - static_cast<T>(1)) * (t - static_cast<T>(1));
			return static_cast<T>(1) - t2 * t2 * std::cos(t * static_cast<T>(std::numbers::pi) * static_cast<T>(4.5));
			}

		template <std::floating_point T>
		T ease_in_out_elastic(T t)
			{
			T t2;
			if (t < static_cast<T>(0.45))
				{
				t2 = t * t;
				return static_cast<T>(8) * t2 * t2 * std::sin(t * static_cast<T>(std::numbers::pi) * static_cast<T>(9));
				}
			else if (t < static_cast<T>(0.55))
				{
				return static_cast<T>(0.5) + static_cast<T>(0.75) * std::sin(t * static_cast<T>(std::numbers::pi) * static_cast<T>(4));
				}
			else
				{
				t2 = (t - static_cast<T>(1)) * (t - static_cast<T>(1));
				return static_cast<T>(1) - static_cast<T>(8) * t2 * t2 * std::sin(t * static_cast<T>(std::numbers::pi) * static_cast<T>(9));
				}
			}

		template <std::floating_point T>
		T ease_in_bounce(T t)
			{
			return std::pow(static_cast<T>(2), static_cast<T>(6) * (t - static_cast<T>(1))) * std::abs(std::sin(t * static_cast<T>(std::numbers::pi) * static_cast<T>(3.5)));
			}

		template <std::floating_point T>
		T ease_out_bounce(T t)
			{
			return static_cast<T>(1) - std::pow(static_cast<T>(2), -static_cast<T>(6) * t) * std::abs(std::cos(t * static_cast<T>(std::numbers::pi) * static_cast<T>(3.5)));
			}

		template <std::floating_point T>
		T ease_in_out_bounce(T t)
			{
			if (t < static_cast<T>(0.5))
				{
				return static_cast<T>(8) * std::pow(static_cast<T>(2), static_cast<T>(8) * (t - static_cast<T>(1))) * std::abs(std::sin(t * static_cast<T>(std::numbers::pi) * static_cast<T>(7)));
				}
			else
				{
				return static_cast<T>(1) - static_cast<T>(8) * std::pow(static_cast<T>(2), -static_cast<T>(8) * t) * std::abs(std::sin(t * static_cast<T>(std::numbers::pi) * static_cast<T>(7)));
				}
			}
		}
	enum type { linear, sine, quad, cubic, quart, quint, expo, circ, back, elastic, bounce };
	enum dir { in, out, in_out };

	template <type type, dir dir = in_out, std::floating_point T>
	T ease(T t)
		{
		if constexpr (type == linear) { return t; }
		switch (type)
			{
			case utils::math::easing::linear: return t;
			case utils::math::easing::sine:    if constexpr (dir == dir::in)     { return _::ease_in_sine       (t); }
			                              else if constexpr (dir == dir::out)    { return _::ease_out_sine      (t); }
			                              else if constexpr (dir == dir::in_out) { return _::ease_in_out_sine   (t); }
			case utils::math::easing::quad:    if constexpr (dir == dir::in)     { return _::ease_in_quad       (t); }
			                              else if constexpr (dir == dir::out)    { return _::ease_out_quad      (t); }
			                              else if constexpr (dir == dir::in_out) { return _::ease_in_out_quad   (t); }
			case utils::math::easing::cubic:   if constexpr (dir == dir::in)     { return _::ease_in_cubic      (t); }
			                              else if constexpr (dir == dir::out)    { return _::ease_out_cubic     (t); }
			                              else if constexpr (dir == dir::in_out) { return _::ease_in_out_cubic  (t); }
			case utils::math::easing::quart:   if constexpr (dir == dir::in)     { return _::ease_in_quart      (t); }
			                              else if constexpr (dir == dir::out)    { return _::ease_out_quart     (t); }
			                              else if constexpr (dir == dir::in_out) { return _::ease_in_out_quart  (t); }
			case utils::math::easing::quint:   if constexpr (dir == dir::in)     { return _::ease_in_quint      (t); }
			                              else if constexpr (dir == dir::out)    { return _::ease_out_quint     (t); }
			                              else if constexpr (dir == dir::in_out) { return _::ease_in_out_quint  (t); }
			case utils::math::easing::expo:    if constexpr (dir == dir::in)     { return _::ease_in_expo       (t); }
			                              else if constexpr (dir == dir::out)    { return _::ease_out_expo      (t); }
			                              else if constexpr (dir == dir::in_out) { return _::ease_in_out_expo   (t); }
			case utils::math::easing::circ:    if constexpr (dir == dir::in)     { return _::ease_in_circ       (t); }
			                              else if constexpr (dir == dir::out)    { return _::ease_out_circ      (t); }
			                              else if constexpr (dir == dir::in_out) { return _::ease_in_out_circ   (t); }
			case utils::math::easing::back:    if constexpr (dir == dir::in)     { return _::ease_in_back       (t); }
			                              else if constexpr (dir == dir::out)    { return _::ease_out_back      (t); }
			                              else if constexpr (dir == dir::in_out) { return _::ease_in_out_back   (t); }
			case utils::math::easing::elastic: if constexpr (dir == dir::in)     { return _::ease_in_elastic    (t); }
			                              else if constexpr (dir == dir::out)    { return _::ease_out_elastic   (t); }
			                              else if constexpr (dir == dir::in_out) { return _::ease_in_out_elastic(t); }
			case utils::math::easing::bounce:  if constexpr (dir == dir::in)     { return _::ease_in_bounce     (t); }
			                              else if constexpr (dir == dir::out)    { return _::ease_out_bounce    (t); }
			                              else if constexpr (dir == dir::in_out) { return _::ease_in_out_bounce (t); }
			default: std::unreachable();
			}
		}

	template <type in, type out, std::floating_point T>
	T ease(T t)
		{
		if constexpr (in == out) { return ease<in, T>(t); }

		if (t < 0.5) { return ease<in,  T>(t); }
		else         { return ease<out, T>(t); }
		}
	}