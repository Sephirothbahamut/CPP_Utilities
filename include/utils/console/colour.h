#pragma once

#include <iostream>
#include <stdexcept>
#include "../graphics/colour.h"

namespace utils::console::colour
	{
	struct colour_8
		{
		graphics::colour::base base;
		bool is_dark{false};

		inline static constexpr colour_8 bright(const graphics::colour::base& base) noexcept { return {base, false}; }
		inline static constexpr colour_8 dark  (const graphics::colour::base& base) noexcept { return {base, true }; }

		static const colour_8 black;
		static const colour_8 white;
		static const colour_8 red;
		static const colour_8 green;
		static const colour_8 blue;
		static const colour_8 yellow;
		static const colour_8 cyan;
		static const colour_8 magenta;
		static const colour_8 dk_black;
		static const colour_8 dk_white;
		static const colour_8 dk_red;
		static const colour_8 dk_green;
		static const colour_8 dk_blue;
		static const colour_8 dk_yellow;
		static const colour_8 dk_cyan;
		static const colour_8 dk_magenta;
		};

	namespace details
		{
		namespace concepts
			{
			template <typename T>
			concept colour_data_t = std::same_as<T, graphics::colour::rgb_u> || std::same_as<T, colour_8>;
			}

		template <concepts::colour_data_t T>
		struct colour
			{
			using colour_data_t = T;
			constexpr colour(const colour_data_t& data) : data{data} {}
			colour_data_t data;
			};

		namespace concepts
			{
			template <typename T>
			concept colour = std::derived_from<T, utils::console::colour::details::colour<typename T::colour_data_t>>;
			}

		struct restore_defaults_t {};

		inline std::ostream& operator<<(std::ostream& stream, const utils::console::colour::details::restore_defaults_t&)
			{
			return stream << "\033[0m";
			}
		}

	template <details::concepts::colour_data_t T>
	struct foreground : details::colour<T>
		{
		constexpr foreground(const T& data) noexcept : details::colour<T>{data} {}; 
		};

	template <details::concepts::colour_data_t T>
	struct background : details::colour<T>
		{ 
		constexpr background(const T& data) noexcept : details::colour<T>{data} {}; 
		};

	namespace concepts
		{
		template <typename T> concept foreground = std::same_as<T, utils::console::colour::foreground<typename T::colour_data_t>>;
		template <typename T> concept background = std::same_as<T, utils::console::colour::background<typename T::colour_data_t>>;
		}

	template <typename char_t, details::concepts::colour colour_t>
	inline constexpr std::basic_string<char_t> to_string(const colour_t& colour) noexcept
		{
		std::basic_stringstream<char_t> ss;
		if constexpr (std::same_as<typename colour_t::colour_data_t, utils::console::colour::colour_8>)
			{
			if constexpr (concepts::foreground<colour_t>)
				{
				switch (colour.data.base)
					{
					case utils::graphics::colour::base::black  : ss << "\x1B[" << (colour.data.is_dark ? "30" : "90") << "m"; break;
					case utils::graphics::colour::base::red    : ss << "\x1B[" << (colour.data.is_dark ? "31" : "91") << "m"; break;
					case utils::graphics::colour::base::green  : ss << "\x1B[" << (colour.data.is_dark ? "32" : "92") << "m"; break;
					case utils::graphics::colour::base::yellow : ss << "\x1B[" << (colour.data.is_dark ? "33" : "93") << "m"; break;
					case utils::graphics::colour::base::blue   : ss << "\x1B[" << (colour.data.is_dark ? "34" : "94") << "m"; break;
					case utils::graphics::colour::base::magenta: ss << "\x1B[" << (colour.data.is_dark ? "35" : "95") << "m"; break;
					case utils::graphics::colour::base::cyan   : ss << "\x1B[" << (colour.data.is_dark ? "36" : "96") << "m"; break;
					case utils::graphics::colour::base::white  : ss << "\x1B[" << (colour.data.is_dark ? "37" : "97") << "m"; break;
					}
				}
			else if constexpr (concepts::background<colour_t>)
				{
				switch (colour.data.base)
					{
					case utils::graphics::colour::base::black  : ss << "\x1B[" << (colour.data.is_dark ? "40" : "100") << "m"; break;
					case utils::graphics::colour::base::red    : ss << "\x1B[" << (colour.data.is_dark ? "41" : "101") << "m"; break;
					case utils::graphics::colour::base::green  : ss << "\x1B[" << (colour.data.is_dark ? "42" : "102") << "m"; break;
					case utils::graphics::colour::base::yellow : ss << "\x1B[" << (colour.data.is_dark ? "43" : "103") << "m"; break;
					case utils::graphics::colour::base::blue   : ss << "\x1B[" << (colour.data.is_dark ? "44" : "104") << "m"; break;
					case utils::graphics::colour::base::magenta: ss << "\x1B[" << (colour.data.is_dark ? "45" : "105") << "m"; break;
					case utils::graphics::colour::base::cyan   : ss << "\x1B[" << (colour.data.is_dark ? "46" : "106") << "m"; break;
					case utils::graphics::colour::base::white  : ss << "\x1B[" << (colour.data.is_dark ? "47" : "107") << "m"; break;
					}
				}
			}
		else if constexpr (std::same_as<typename colour_t::colour_data_t, utils::graphics::colour::rgb_u>)
			{
			ss << "\033[";
			if constexpr (concepts::foreground<colour_t>) { ss << "38"; }
			else if constexpr (concepts::background<colour_t>) { ss << "48"; }

			ss << ";2;" << +colour.data.r() << ";" << +colour.data.g() << ";" << +colour.data.b() << "m";
			}

		return ss.str();
		}

	template <typename char_t, details::concepts::colour colour_t>
	std::basic_ostream<char_t>& operator<<(std::basic_ostream<char_t>& stream, const colour_t& colour) noexcept
		{
		return stream << to_string<char_t>(colour);
		}

	inline static constexpr details::restore_defaults_t restore_defaults;

	inline constexpr colour_8 colour_8::black      = {colour_8::bright(utils::graphics::colour::base::black  )};
	inline constexpr colour_8 colour_8::white      = {colour_8::bright(utils::graphics::colour::base::white  )};
	inline constexpr colour_8 colour_8::red        = {colour_8::bright(utils::graphics::colour::base::red    )};
	inline constexpr colour_8 colour_8::green      = {colour_8::bright(utils::graphics::colour::base::green  )};
	inline constexpr colour_8 colour_8::blue       = {colour_8::bright(utils::graphics::colour::base::blue   )};
	inline constexpr colour_8 colour_8::yellow     = {colour_8::bright(utils::graphics::colour::base::yellow )};
	inline constexpr colour_8 colour_8::cyan       = {colour_8::bright(utils::graphics::colour::base::cyan   )};
	inline constexpr colour_8 colour_8::magenta    = {colour_8::bright(utils::graphics::colour::base::magenta)};
	inline constexpr colour_8 colour_8::dk_black   = {colour_8::dark  (utils::graphics::colour::base::black  )};
	inline constexpr colour_8 colour_8::dk_white   = {colour_8::dark  (utils::graphics::colour::base::white  )};
	inline constexpr colour_8 colour_8::dk_red     = {colour_8::dark  (utils::graphics::colour::base::red    )};
	inline constexpr colour_8 colour_8::dk_green   = {colour_8::dark  (utils::graphics::colour::base::green  )};
	inline constexpr colour_8 colour_8::dk_blue    = {colour_8::dark  (utils::graphics::colour::base::blue   )};
	inline constexpr colour_8 colour_8::dk_yellow  = {colour_8::dark  (utils::graphics::colour::base::yellow )};
	inline constexpr colour_8 colour_8::dk_cyan    = {colour_8::dark  (utils::graphics::colour::base::cyan   )};
	inline constexpr colour_8 colour_8::dk_magenta = {colour_8::dark  (utils::graphics::colour::base::magenta)};
	
	inline static constexpr foreground<colour_8> brace     {colour_8::bright(utils::graphics::colour::base::black)};
	inline static constexpr foreground<colour_8> separ     {colour_8::dark  (utils::graphics::colour::base::white)};
	inline static constexpr foreground<colour_8> value     {colour_8::bright(utils::graphics::colour::base::white)};
	inline static constexpr foreground<colour_8> type      {colour_8::dark  (utils::graphics::colour::base::green)};
	}