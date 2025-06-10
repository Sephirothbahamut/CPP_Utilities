#pragma once

#include <string>

namespace utils::string
	{
	template <typename char_T>
	struct types
		{
		types() = delete;

		using char_t = char_T;

		using string = std::basic_string<char_t>;
		using view   = std::basic_string_view<char_t>;
		using c      = char_t*;

		struct hash;
		};

	namespace concepts
		{
		template <typename T, typename char_t = char>
		concept stringlike = std::convertible_to<T, std::basic_string_view<char_t>>;
		}

	template <typename char_T>
	struct types<char_T>::hash
		{
		using string_t = std::basic_string<char_t>;
		using string_view_t = std::basic_string_view<char_t>;

		using hash_type = std::hash<view>;
		using is_transparent = void;

		template <concepts::stringlike<char_t> T>
		constexpr std::size_t operator()(const T& str) const noexcept { return hash_type{}(str); }
		};

	/// <summary>
	/// Converts strings from utf8 to utf16 or utf32 and from utf16 or utf32 to utf8.
	/// utf8 types: char, char8_t
	/// utf16 types: char16_t, wchar_t (on Windows)
	/// utf32 types: char32_t, wchar_t (on non-Windows)
	/// Can also convert wchar_t strings to their char16/32_t counterpart. This conversion is UGLY (reinterpret cast warning). But Marineshi said it's fine. :) 
	/// </summary>
	template <typename to_char_t, typename from_char_t>
	constexpr std::basic_string<to_char_t> cast(const std::basic_string_view<from_char_t> in);

	/// <summary>
	/// Converts strings from utf8 to utf16 or utf32 and from utf16 or utf32 to utf8.
	/// utf8 types: char, char8_t
	/// utf16 types: char16_t, wchar_t (on Windows)
	/// utf32 types: char32_t, wchar_t (on non-Windows)
	/// Can also convert wchar_t strings to their char16/32_t counterpart. This conversion is UGLY (reinterpret cast warning). But Marineshi said it's fine. :) 
	/// </summary>
	template <typename to_char_t, typename from_char_t>
	constexpr std::basic_string<to_char_t> cast(const std::basic_string<from_char_t>& in)
		{
		if constexpr (std::same_as<from_char_t, to_char_t>)
			{
			return in;
			}
		else if constexpr (sizeof(from_char_t) == sizeof(to_char_t))
			{
			const std::basic_string<to_char_t> ret{reinterpret_cast<const to_char_t*>(in.c_str())};
			return ret;
			}
		else
			{
			return cast<to_char_t, from_char_t>(std::basic_string_view<from_char_t>{in});
			}
		}

	/// <summary>
	/// Converts strings from utf8 to utf16 or utf32 and from utf16 or utf32 to utf8.
	/// utf8 types: char, char8_t
	/// utf16 types: char16_t, wchar_t (on Windows)
	/// utf32 types: char32_t, wchar_t (on non-Windows)
	/// Can also convert wchar_t strings to their char16/32_t counterpart. This conversion is UGLY (reinterpret cast warning). But Marineshi said it's fine. :) 
	/// </summary>
	template <typename to_char_t, typename from_char_t>
	constexpr std::basic_string<to_char_t> cast(const from_char_t* in)
		{
		return cast<to_char_t, from_char_t>(std::basic_string_view<from_char_t>{in});
		}


	/// <summary>
	/// Parses a string representing a codepoint as hex values.
	/// The values you usually see in escaped sequences (the "1F604" part of "\u1F604")
	/// </summary>
	template <typename from_char_t>
	constexpr char32_t parse_codepoint(const std::basic_string_view<from_char_t> in);

	template <typename to_char_t>
	constexpr std::basic_string<to_char_t> codepoint_to_string(const char32_t& codepoint);
	}

#ifdef utils_implementation
#include "string.cpp"
#endif