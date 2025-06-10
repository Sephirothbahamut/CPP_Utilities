#include "string.h"
#include "compilation/OS.h"
#include "third_party/utf8.h"

#include "memory.h"

#include <sstream>

namespace utils::string
	{
	namespace details
		{
		template <typename char_t>
		struct utf_check
			{
			//Tell me what platform would even be this crazy...
			// constexpr bool wchar_is_utf8 {sizeof(wchar_t) == sizeof(char8_t )};
			inline static constexpr const bool wchar_is_utf16{sizeof(wchar_t) == sizeof(char16_t)};
			inline static constexpr const bool wchar_is_utf32{sizeof(wchar_t) == sizeof(char32_t)};
		
			inline static constexpr const bool utf8 {std::same_as<char_t, char8_t > ||                   std::same_as<char_t, char   >};
			inline static constexpr const bool utf16{std::same_as<char_t, char16_t> || (wchar_is_utf16 ? std::same_as<char_t, wchar_t> : false)};
			inline static constexpr const bool utf32{std::same_as<char_t, char32_t> || (wchar_is_utf32 ? std::same_as<char_t, wchar_t> : false)};
			};
		}


	template <typename to_char_t, typename from_char_t>
	constexpr std::basic_string<to_char_t> cast(const std::basic_string_view<from_char_t> in)
		{
		if constexpr (std::same_as<from_char_t, to_char_t>)
			{
			return std::basic_string<to_char_t>{in};
			}
		else if constexpr (sizeof(from_char_t) == sizeof(to_char_t))
			{
			std::basic_string<from_char_t> in_string_own{in};
			return cast<to_char_t, from_char_t>(in_string_own);
			}



		std::basic_string<to_char_t> ret;

		using input_utf  = details::utf_check<from_char_t>;
		using output_utf = details::utf_check<  to_char_t>;
		
		if constexpr (input_utf::utf8 && output_utf::utf16)
			{
			utf8::utf8to16(in.begin(), in.end(), std::back_inserter(ret));
			}
		else if constexpr (input_utf::utf16 && output_utf::utf8)
			{
			utf8::utf16to8(in.begin(), in.end(), std::back_inserter(ret));
			}
		else if constexpr (input_utf::utf8 && output_utf::utf32)
			{
			utf8::utf8to32(in.begin(), in.end(), std::back_inserter(ret));
			}
		else if constexpr (input_utf::utf32 && output_utf::utf8)
			{
			utf8::utf32to8(in.begin(), in.end(), std::back_inserter(ret));
			}
		return ret;
		}
	
	template std::basic_string<char    > cast<char    , char    >(const std::basic_string_view<char    > in);
	template std::basic_string<char8_t > cast<char8_t , char8_t >(const std::basic_string_view<char8_t > in);
	template std::basic_string<char16_t> cast<char16_t, char16_t>(const std::basic_string_view<char16_t> in);
	template std::basic_string<char32_t> cast<char32_t, char32_t>(const std::basic_string_view<char32_t> in);
	template std::basic_string<wchar_t > cast<wchar_t , wchar_t >(const std::basic_string_view<wchar_t > in);

	//TODO is there any standard macro to check the size of wchar_t?
	#ifdef utils_compilation_os_windows
	template std::basic_string<char16_t> cast<char16_t, wchar_t >(const std::basic_string_view<wchar_t > in);
	template std::basic_string<wchar_t > cast<wchar_t , char16_t>(const std::basic_string_view<char16_t> in);
	#else
	template std::basic_string<char32_t> cast<char32_t, wchar_t >(const std::basic_string_view<wchar_t > in);
	template std::basic_string<wchar_t > cast<wchar_t , char32_t>(const std::basic_string_view<char32_t> in);
	#endif


	template std::basic_string<char    > cast<char    , char16_t>(const std::basic_string_view<char16_t> in);
	template std::basic_string<char8_t > cast<char8_t , char16_t>(const std::basic_string_view<char16_t> in);
	template std::basic_string<char    > cast<char    , char32_t>(const std::basic_string_view<char32_t> in);
	template std::basic_string<char8_t > cast<char8_t , char32_t>(const std::basic_string_view<char32_t> in);
	template std::basic_string<char    > cast<char    , wchar_t >(const std::basic_string_view<wchar_t > in);
	template std::basic_string<char8_t > cast<char8_t , wchar_t >(const std::basic_string_view<wchar_t > in);
	
	template std::basic_string<char16_t> cast<char16_t, char    >(const std::basic_string_view<char    > in);
	template std::basic_string<char16_t> cast<char16_t, char8_t >(const std::basic_string_view<char8_t > in);
	template std::basic_string<char32_t> cast<char32_t, char    >(const std::basic_string_view<char    > in);
	template std::basic_string<char32_t> cast<char32_t, char8_t >(const std::basic_string_view<char8_t > in);
	template std::basic_string<wchar_t > cast<wchar_t , char    >(const std::basic_string_view<char    > in);
	template std::basic_string<wchar_t > cast<wchar_t , char8_t >(const std::basic_string_view<char8_t > in);

	template <typename from_char_t>
	constexpr char32_t parse_codepoint(const std::basic_string_view<from_char_t> in)
		{
		if constexpr (std::same_as<from_char_t, char>)
			{
			std::basic_stringstream<from_char_t> ss;
			ss << std::hex << in;
			std::uint32_t codepoint_value;
			ss >> codepoint_value;

			const char32_t codepoint{static_cast<char32_t>(codepoint_value)};
			return codepoint;
			}
		else if constexpr (!std::same_as<from_char_t, char>)
			{
			const std::string cast_string{cast<char, from_char_t>(in)};
			return parse_codepoint<char>(cast_string);
			}
		}

	template char32_t parse_codepoint<char    >(const std::basic_string_view<char    > in);
	template char32_t parse_codepoint<char8_t >(const std::basic_string_view<char8_t > in);
	template char32_t parse_codepoint<char16_t>(const std::basic_string_view<char16_t> in);
	template char32_t parse_codepoint<char32_t>(const std::basic_string_view<char32_t> in);
	template char32_t parse_codepoint<wchar_t >(const std::basic_string_view<wchar_t > in);


	template <typename to_char_t>
	constexpr std::basic_string<to_char_t> codepoint_to_string(const char32_t& codepoint)
		{
		using output_utf = details::utf_check<to_char_t>;

		if constexpr (output_utf::utf8)
			{
			const utils::observer_ptr<const char32_t> codepoint_begin{std::addressof(codepoint)};
			const utils::observer_ptr<const char32_t> codepoint_end  {codepoint_begin + 1};

			std::basic_string<to_char_t> ret;
			utf8::utf32to8(codepoint_begin, codepoint_end, std::back_inserter(ret));
			return ret;
			}
		else if constexpr (output_utf::utf16)
			{
			return cast<to_char_t, char8_t>(codepoint_to_string<char8_t>(codepoint));
			}
		else if constexpr (output_utf::utf32)
			{
			return {codepoint};
			}
		}
	
	template std::basic_string<char    > codepoint_to_string(const char32_t& codepoint);
	template std::basic_string<char8_t > codepoint_to_string(const char32_t& codepoint);
	template std::basic_string<char16_t> codepoint_to_string(const char32_t& codepoint);
	template std::basic_string<char32_t> codepoint_to_string(const char32_t& codepoint);
	template std::basic_string<wchar_t > codepoint_to_string(const char32_t& codepoint);
	}


