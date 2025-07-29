#pragma once
#include <cmath>
#include <limits>
#include <cassert>
#include <concepts>
#include <algorithm>

#include "../compilation/warnings.h"
#include "../compilation/gpu.h"
#include "../math/math.h"
#include "../math/angle.h"
#include "../oop/conditional_inheritance.h"

#include "../storage.h"

// colour conversions from: https://axonflux.com/handy-rgb-to-hsl-and-rgb-to-hsv-color-model-c

//TODO write test cases

namespace utils::graphics::colour
	{
	enum class base
		{
		black , white, 
		red   , green, blue   , 
		yellow, cyan , magenta, 
		};

	namespace details
		{
		template <utils::math::concepts::undecorated_number T, size_t extent>
		struct additive;

		template <utils::math::concepts::undecorated_floating_point T, size_t extent>
		struct hsv;
		}

	template <utils::math::concepts::undecorated_number T = float>
	using rgb    = details::additive<T, 3>;
	using rgb_f  = rgb<float  >;
	using rgb_d  = rgb<double >;
	using rgb_u  = rgb<uint8_t>;
	template <utils::math::concepts::undecorated_number T = float>
	using rgba   = details::additive<T, 4>;
	using rgba_f = rgba<float  >;
	using rgba_d = rgba<double >;
	using rgba_u = rgba<uint8_t>;

	template <utils::math::concepts::undecorated_floating_point T = float, bool has_alpha = false>
	using hsv    = details::hsv<T, has_alpha ? 4 : 3>;
	using hsva   = hsv<float , true >;
	using hsv_f  = hsv<float , false>;
	using hsva_f = hsv<float , true >;
	using hsv_d  = hsv<double, false>;
	using hsva_d = hsv<double, true >;

	namespace concepts
		{
		template <typename T>
		concept additive = std::same_as<std::remove_cvref_t<T>, details::additive<typename T::template_type, T::extent>>;
		template <typename T>
		concept rgba = additive<T> &&  T::static_has_alpha;
		template <typename T>
		concept rgb  = additive<T> && !T::static_has_alpha;

		template <typename T>
		concept hsv = std::same_as<std::remove_cvref_t<T>, details::hsv<typename T::template_type, T::extent>>;
		template <typename T>
		concept hsva = hsv<T> && T::static_has_alpha;

		template <typename T, typename source_t>
		concept compatible_additive =
			utils::storage::concepts::compatible_multiple<T, source_t> &&
			additive<T>;
		template <typename T, typename source_t>
		concept compatible_hsv =
			utils::storage::concepts::compatible_multiple<T, source_t> &&
			hsv<T>;

		template <typename T>
		concept colour = additive<T> || hsv<T>;
		}

	namespace details
		{
		struct colour_cast_flag_t {};

		inline extern constexpr const char name_rgb[]{"rgb"};
		inline extern constexpr const char name_hsv[]{"hsv"};

		template<utils::math::concepts::undecorated_number T, size_t SIZE>
		struct utils_oop_empty_bases additive final : storage::multiple<T, SIZE>
			{
			using base_t = storage::multiple<T, SIZE>;
			using base_t::extent;
			static_assert(extent == 3 || extent == 4);
			using base_t::storage_type;
			using typename base_t::value_type;
			using typename base_t::const_aware_value_type;
			using typename base_t::template_type;
			using self_t       = additive<value_type, extent>;
			using owner_self_t = additive<value_type, extent>;

			using range = utils::math::type_based_numeric_range<value_type>;
			inline static constexpr bool static_has_alpha{extent == 4};

			using storage::multiple<T, extent>::multiple;

			utils_gpu_available constexpr additive(colour_cast_flag_t, const concepts::compatible_hsv<additive<T, extent>> auto& hsv) noexcept
				requires(storage_type.is_owner());
			utils_gpu_available constexpr additive(const concepts::compatible_hsv<additive<T, extent>> auto& hsv) noexcept : additive{colour_cast_flag_t{}, hsv} {}
			utils_gpu_available constexpr additive(      concepts::compatible_hsv<additive<T, extent>> auto& hsv) noexcept : additive{colour_cast_flag_t{}, hsv} {}
			
			self_t& operator=(const additive<T, extent>& copy) noexcept
				{
				using other_range = std::remove_cvref_t<decltype(copy)>::range;

				for (size_t i{0}; i < 3; i++)
					{
					(*this)[i] = other_range::template cast_to<range>(copy[i]);
					}
				if constexpr (static_has_alpha)
					{
					if constexpr (copy.static_has_alpha)
						{
						a() = other_range::template cast_to<range>(copy.a());
						}
					else
						{
						a() = range::full_value;
						}
					}
				return *this;
				}

			utils_gpu_available constexpr const const_aware_value_type& r() const noexcept { return (*this)[0]; }
			utils_gpu_available constexpr       const_aware_value_type& r()       noexcept { return (*this)[0]; }
			utils_gpu_available constexpr const const_aware_value_type& g() const noexcept { return (*this)[1]; }
			utils_gpu_available constexpr       const_aware_value_type& g()       noexcept { return (*this)[1]; }
			utils_gpu_available constexpr const const_aware_value_type& b() const noexcept { return (*this)[2]; }
			utils_gpu_available constexpr       const_aware_value_type& b()       noexcept { return (*this)[2]; }
			utils_gpu_available constexpr const const_aware_value_type& a() const noexcept { if constexpr (extent == 4) { return (*this)[3]; } else { return range::full_value; } }
			utils_gpu_available constexpr       const_aware_value_type& a()       noexcept requires(static_has_alpha) { return (*this)[3]; }
			utils_gpu_available constexpr const additive<value_type, 3> rgb() const noexcept { return {r(), g(), b()}; }
			//utils_gpu_available constexpr const additive<const const_aware_value_type&, 3> rgb() const noexcept { return {r(), g(), b()}; }
			//utils_gpu_available constexpr       additive<      const_aware_value_type&, 3> rgb()       noexcept { return {r(), g(), b()}; }

			utils_gpu_available constexpr additive(enum base base, value_type components_multiplier = range::full_value, value_type alpha = range::full_value) noexcept
				requires(storage_type.is_owner())
				{
				r() = components_multiplier * (base == base::white || base == base::red   || base == base::yellow  || base == base::magenta);
				g() = components_multiplier * (base == base::white || base == base::green || base == base::yellow  || base == base::cyan);
				b() = components_multiplier * (base == base::white || base == base::blue  || base == base::magenta || base == base::cyan);
				if constexpr (static_has_alpha) { a() = alpha; }
				}

			utils_gpu_available constexpr additive(const concepts::additive auto& other) noexcept
				requires(storage_type.is_owner())
				{
				using other_range = std::remove_cvref_t<decltype(other)>::range;

				for (size_t i = 0; i < 3; i++)
					{
					(*this)[i] = other_range::template cast_to<range>(other[i]);
					}
				if constexpr (static_has_alpha)
					{
					if constexpr (other.static_has_alpha)
						{
						a() = other_range::template cast_to<range>(other.a());
						}
					else
						{
						a() = range::full_value;
						}
					}
				}

			utils_gpu_available constexpr additive(const concepts::additive auto& other, float alpha) noexcept
				requires(storage_type.is_owner() && static_has_alpha)
				{
				using other_range = std::remove_cvref_t<decltype(other)>::range;

				for (size_t i = 0; i < 3; i++)
					{
					(*this)[i] = other_range::template cast_to<range>(other[i]);
					}
				a() = alpha;
				}

			utils_gpu_available constexpr owner_self_t blend(const concepts::colour auto& foreground) const noexcept
				requires(std::remove_cvref_t<decltype(foreground)>::static_has_alpha)
				{
				using floating_t = std::conditional_t<std::floating_point<value_type>, value_type, float>;
				using rgba_t = additive<floating_t, 4>;
				const rgba_t remapped_foreground{static_cast<rgba_t>(foreground)};
				const rgba_t remapped_self{*this};

				rgba_t remapped_ret;

				const value_type multiplier_a{[this]() -> value_type
					{
					if constexpr (static_has_alpha) 
						{
						return (static_cast<value_type>(1) - a());
						}
					else 
						{ 
						return static_cast<value_type>(1);
						} 
					}()};

				const floating_t ret_a{floating_t{1.f} - (floating_t{1.f} - remapped_foreground.a()) * (floating_t{1.f} - remapped_self.a())};
				if (ret_a < utils::math::constants::epsilon) { return {}; } // Fully transparent -- r,g,b not important

				for (size_t i{0}; i < 3; i++)
					{
					remapped_ret[i] = remapped_foreground[i] * remapped_foreground.a() / ret_a + remapped_self[i] * remapped_self.a() * (1.f - remapped_foreground.a()) / ret_a;
					}
				if constexpr (static_has_alpha) { remapped_ret.a() = ret_a; }

				const owner_self_t ret{remapped_ret};
				return ret;
				}

			//utils_gpu_available constexpr rgb<T, 4> test_blend(const rgb<T, 4>& foreground) const noexcept
			//	requires(has_alpha)
			//	{
			//	rgb<T, 4> ret;
			//	ret.a() = utils::math::min(1.f, a() + foreground.a());
			//	const float remaining_a{1.f - foreground.a()};
			//	const float background_a{utils::math::min(remaining_a, a())};
			//
			//	for (size_t i{0}; i < 3; i++)
			//		{
			//		ret[i] = (foreground[i] * foreground.a()) + (background_a * (*this)[i]);
			//		}
			//
			//	return ret;
			//	}
			};

		template <utils::math::concepts::undecorated_floating_point T, size_t SIZE>
		struct utils_oop_empty_bases hsv final : storage::multiple<T, SIZE>
			{
			using base_t = storage::multiple<T, SIZE>;
			using base_t::extent;
			static_assert(extent == 3 || extent == 4);
			using base_t::storage_type;
			using typename base_t::value_type;
			using typename base_t::const_aware_value_type;
			using typename base_t::template_type;
			using self_t       = hsv<value_type, extent>;
			using owner_self_t = hsv<value_type, extent>;

			using range = utils::math::type_based_numeric_range<T>;
			inline static constexpr bool static_has_alpha{extent == 4};

			
			utils_gpu_available constexpr hsv(colour_cast_flag_t, const concepts::compatible_additive<hsv<T, extent>> auto& rgb) noexcept
				requires(storage_type.is_owner());
			utils_gpu_available constexpr hsv(const concepts::compatible_additive<hsv<T, extent>> auto& rgb) noexcept : hsv{colour_cast_flag_t{}, rgb} {}
			utils_gpu_available constexpr hsv(      concepts::compatible_additive<hsv<T, extent>> auto& rgb) noexcept : hsv{colour_cast_flag_t{}, rgb} {}

			using storage::multiple<T, extent>::multiple;

			utils_gpu_available constexpr hsv(enum base base, T components_multiplier = range::full_value, T alpha = range::full_value)
				requires(storage_type.is_owner())
				{
				if (base != base::black && components_multiplier != T{0.f}) 
					{
					utils::math::angle::degf angle{0.f};
					using namespace utils::math::angle::literals;
					switch (base)
						{
						case base::black  : h() =   0_deg; break;
						case base::white  : h() =   0_deg; break;
						case base::red    : h() =   0_deg; break;
						case base::green  : h() = 120_deg; break;
						case base::blue   : h() = 240_deg; break;
						case base::yellow : h() =  60_deg; break;
						case base::cyan   : h() = 180_deg; break;
						case base::magenta: h() = 300_deg; break;
						}

					s() = range::full_value; //TODO make correct calculation
					v() = range::full_value; //TODO make correct calculation
					}
				else { h() = value_type{0.f}; s() = value_type{0.f}; v() = value_type{0.f}; }
				if constexpr (static_has_alpha) { a() = alpha; }
				}

			utils_gpu_available constexpr hsv(const concepts::hsv auto& other) noexcept
				requires(storage_type.is_owner())
				{
				for (size_t i = 0; i < 3; i++)
					{
					using other_range = std::remove_cvref_t<decltype(other)>::range;
					(*this)[i] = other_range::template convert_to<range>(other[i]);
					}
				if constexpr (static_has_alpha) { a() = other.a(); }
				}

			utils_gpu_available constexpr hsv(const utils::math::angle::degf& hue, const float& saturation, const float& value) noexcept
				requires(storage_type.is_owner()) : 
				base_t
					{
					::utils::math::angle::base<const_aware_value_type, range::full_value>{hue}.value(),
					saturation, 
					value
					}
				{}

			utils_gpu_available constexpr ::utils::math::angle::base<const const_aware_value_type&, range::full_value> h() const noexcept { return {(*this)[0]}; }
			utils_gpu_available constexpr ::utils::math::angle::base<      const_aware_value_type&, range::full_value> h()       noexcept { return {(*this)[0]}; }
			utils_gpu_available constexpr const const_aware_value_type& s() const noexcept { return (*this)[1]; }
			utils_gpu_available constexpr       const_aware_value_type& s()       noexcept { return (*this)[1]; }
			utils_gpu_available constexpr const const_aware_value_type& v() const noexcept { return (*this)[2]; }
			utils_gpu_available constexpr       const_aware_value_type& v()       noexcept { return (*this)[2]; }
			utils_gpu_available constexpr const const_aware_value_type& a() const noexcept { if constexpr (extent == 4) { return (*this)[3]; } else { return range::full_value; } }
			utils_gpu_available constexpr       const_aware_value_type& a()       noexcept requires(static_has_alpha) { return (*this)[3]; }
			};

		template<utils::math::concepts::undecorated_number T, size_t extent>
		utils_gpu_available constexpr additive<T, extent>::additive(colour_cast_flag_t, const concepts::compatible_hsv<additive<T, extent>> auto& hsv) noexcept
			requires(storage_type.is_owner())
			{
			if (hsv.s() == 0)
				{
				for (size_t i{0}; i < 3; i++)
					{
					(*this) = hsv.v();
					}
				}
			else
				{
				float tmp_r, tmp_g, tmp_b;

				float tmp_h{hsv.h().value()};
				tmp_h *= 6.f;

				const unsigned i{static_cast<unsigned>(tmp_h)};
				const float f{tmp_h - i                };
				const float v{hsv.v()};
				const float p{v * (1.f - hsv.s()            )};
				const float q{v * (1.f - hsv.s() *        f )};
				const float t{v * (1.f - hsv.s() * (1.f - f))};

				switch (i % 6)
					{
					case 0: tmp_r = v, tmp_g = t, tmp_b = p; break;
					case 1: tmp_r = q, tmp_g = v, tmp_b = p; break;
					case 2: tmp_r = p, tmp_g = v, tmp_b = t; break;
					case 3: tmp_r = p, tmp_g = q, tmp_b = v; break;
					case 4: tmp_r = t, tmp_g = p, tmp_b = v; break;
					case 5: tmp_r = v, tmp_g = p, tmp_b = q; break;
					default: //Actually unreachable, but the compiler doesn't realize that 0 <= i < 6, so it thinks tmp_r, tmp_g and tmp_b aren't always initialized
						tmp_r = 0, tmp_g = 0, tmp_b = 0;
					}

				r() = tmp_r;
				g() = tmp_g;
				b() = tmp_b;
				}

			if constexpr (static_has_alpha) { a() = hsv.a(); }
			}

		template<utils::math::concepts::undecorated_floating_point T, size_t extent>
		utils_gpu_available constexpr hsv<T, extent>::hsv(colour_cast_flag_t, const concepts::compatible_additive<hsv<T, extent>> auto& rgb) noexcept
			requires(storage_type.is_owner())
			{//https://stackoverflow.com/questions/3018313/algorithm-to-convert-rgb-to-hsv-and-hsv-to-rgb-in-range-0-255-for-both
			using from_t = std::remove_cvref_t<decltype(rgb)>;
			const details::additive<value_type, extent> remapped_rgb{rgb};

			const value_type min{std::min({remapped_rgb.r(), remapped_rgb.g(), remapped_rgb.b()})};
			const value_type max{std::max({remapped_rgb.r(), remapped_rgb.g(), remapped_rgb.b()})};

			v() = max;                                // v
			const value_type delta{max - min};

			value_type tmp_hue{static_cast<value_type>(0.)};

			if (delta < 0.00001f)
				{
				s() = 0.f;
				tmp_hue = 0.f; // undefined, maybe nan?
				}
			if (max > 0.0f) 
				{ // NOTE: if Max is == 0, this divide would cause a crash
				s() = (delta / max);                  // s
				}
			else 
				{
				// if max is 0, then r = g = b = 0              
				// s = 0, h is undefined
				s() = 0.0;
				tmp_hue = std::numeric_limits<value_type>::quiet_NaN(); // its now undefined
				}
			if (remapped_rgb.r() >= max)                           // > is bogus, just keeps compilor happy
				{
				tmp_hue = (remapped_rgb.g() - remapped_rgb.b()) / delta;        // between yellow & magenta
				}
			else
				{
				if (remapped_rgb.g() >= max)
					{
					tmp_hue = static_cast<value_type>(2.) + (remapped_rgb.b() - remapped_rgb.r()) / delta;  // between cyan & yellow
					}
				else
					{
					tmp_hue = static_cast<value_type>(4.) + (remapped_rgb.r() - remapped_rgb.g()) / delta;  // between magenta & cyan
					}
				}
		
			tmp_hue = (tmp_hue / 6.f) * range::full_value;
			h().value() = tmp_hue;

			h().clamp_self();

			if constexpr (static_has_alpha) { a() = rgb.a(); }
			}
		}
	};

//namespace utils::output
//	{
//	namespace typeless
//		{
//		template <typename T, size_t SIZE>
//		inline ::std::ostream& operator<<(::std::ostream& os, const utils::graphics::colour::rgb<T, SIZE>& colour)
//			{
//			namespace ucc = utils::console::colour;
//
//			os << ucc::brace << "(";
//
//			if constexpr (SIZE >= 1) { os                       << ucc::foreground::red   << std::to_string(colour[0]); }
//			if constexpr (SIZE >= 2) { os << ucc::separ << ", " << ucc::foreground::green << std::to_string(colour[1]); }
//			if constexpr (SIZE >= 3) { os << ucc::separ << ", " << ucc::foreground::blue  << std::to_string(colour[2]); }
//			if constexpr (SIZE >= 4) { os << ucc::separ << ", " << ucc::foreground::white << std::to_string(colour[3]); }
//
//			os << ucc::brace << ")";
//
//			return os;
//			}
//		}
//
//	template <typename T, size_t SIZE>
//	inline ::std::ostream& operator<<(::std::ostream& os, const utils::graphics::colour::rgb<T, SIZE>& colour)
//		{
//		namespace ucc = utils::console::colour;
//		os << ucc::type << "rgb" << SIZE << typeid(T).name();
//		return utils::output::typeless::operator<<(os, colour);
//		}
//	}



	

namespace utils::math
	{
	template <graphics::colour::concepts::colour T>
	utils_gpu_available constexpr auto abs(const T& v) noexcept { return v.for_each_to_new([](const auto& value) { return utils::math::abs(value); }); }

	template <graphics::colour::concepts::colour T>
	utils_gpu_available constexpr auto floor(const T& v) noexcept { return v.for_each_to_new([](const auto& value) { return utils::math::floor(value); }); }

	template <graphics::colour::concepts::colour T>
	utils_gpu_available constexpr auto ceil(const T& v) noexcept { return v.for_each_to_new([](const auto& value) { return utils::math::ceil(value); }); }

	template <graphics::colour::concepts::colour T>
	utils_gpu_available constexpr auto pow(const T& a, const storage::concepts::type_compatible_with_storage<T> auto& b)
		{
		return a.operator_to_new(b, [](const auto& a, const auto& b) { return std::pow(a, b); });
		}

	template <graphics::colour::concepts::colour T>
	utils_gpu_available constexpr typename T::owner_self_t pow(const T& a, const storage::concepts::type_compatible_with_storage<T> auto& b)
		{
		return a.operator_to_new(b, [](const auto& a, const auto& b) { return std::pow(a, b); });
		}

	template <graphics::colour::concepts::colour T>
	utils_gpu_available constexpr typename T::owner_self_t lerp(const T& a, const T& b, float t)
		{
		typename T::owner_self_t ret;
		for (size_t i = 0; i < T::extent; i++)
			{
			ret[i] = utils::math::lerp(a[i], b[i], t);
			}
		return ret;
		}

	template <graphics::colour::concepts::colour T>
	utils_gpu_available constexpr typename T::owner_self_t clamp(const T& in, const T& min, const T& max)
		{
		typename T::owner_self_t ret;
		for (size_t i = 0; i < T::extent; i++)
			{
			ret[i] = utils::math::clamp(in[i], min[i], max[i]);
			}
		return ret;
		}

	template <graphics::colour::concepts::colour T>
	utils_gpu_available constexpr typename T::owner_self_t min(const T& a, const T& b)
		{
		return a.operator_to_new(b, [](const auto& a, const auto& b) { return utils::math::min(a, b); });
		}

	template <graphics::colour::concepts::colour T>
	utils_gpu_available constexpr typename T::owner_self_t max(const T& a, const T& b)
		{
		return a.operator_to_new(b, [](const auto& a, const auto& b) { return utils::math::max(a, b); });
		}
	
	template <graphics::colour::concepts::colour T>
	utils_gpu_available constexpr typename T::owner_self_t clamp(const T& in, const storage::concepts::type_compatible_with_storage<T> auto& min, const storage::concepts::type_compatible_with_storage<T> auto& max)
		{
		typename T::owner_self_t ret;
		for (size_t i = 0; i < T::extent; i++)
			{
			ret[i] = utils::math::clamp(in[i], min, max);
			}
		return ret;
		}

	template <graphics::colour::concepts::colour T>
	utils_gpu_available constexpr typename T::owner_self_t min(const T& a, const storage::concepts::type_compatible_with_storage<T> auto& b)
		{
		return a.operator_to_new(b, [](const auto& a, const auto& b) { return utils::math::min(a, b); });
		}

	template <graphics::colour::concepts::colour T>
	utils_gpu_available constexpr typename T::owner_self_t max(const T& a, const storage::concepts::type_compatible_with_storage<T> auto& b)
		{
		return a.operator_to_new(b, [](const auto& a, const auto& b) { return utils::math::max(a, b); });
		}
	}