#pragma once

#include <chrono>
#include <string>
#include <iostream>
#include <string_view>

#include "../string.h"
#include "../console/colour.h"


//TODO output readable date-time +GMT delta instead of timestamp
//Ideally: yyyy/mm/dd hh:mm:ss:millis

namespace utils::logging
	{
	enum class output_style_t { on_line, tag_as_separator };
	enum class msg_t { raw, log, dgn, inf, wrn, err, suc, section_enter, section_leave };

	class message
		{
		public:
			static output_style_t output_style;

			constexpr message           (const message&  copy) = default;
			constexpr message& operator=(const message&  copy) = default;
			constexpr message           (      message&& move) = default;
			constexpr message& operator=(      message&& move) = default;

			inline static constexpr message raw          (const string::concepts::stringlike auto& string, size_t indents_count) noexcept { return {msg_t::raw          , string, indents_count}; }
			inline static constexpr message log          (const string::concepts::stringlike auto& string, size_t indents_count) noexcept { return {msg_t::log          , string, indents_count}; }
			inline static constexpr message dgn          (const string::concepts::stringlike auto& string, size_t indents_count) noexcept { return {msg_t::dgn          , string, indents_count}; }
			inline static constexpr message inf          (const string::concepts::stringlike auto& string, size_t indents_count) noexcept { return {msg_t::inf          , string, indents_count}; }
			inline static constexpr message wrn          (const string::concepts::stringlike auto& string, size_t indents_count) noexcept { return {msg_t::wrn          , string, indents_count}; }
			inline static constexpr message err          (const string::concepts::stringlike auto& string, size_t indents_count) noexcept { return {msg_t::err          , string, indents_count}; }
			inline static constexpr message suc          (const string::concepts::stringlike auto& string, size_t indents_count) noexcept { return {msg_t::suc          , string, indents_count}; }
			inline static constexpr message section_enter(const string::concepts::stringlike auto& string, size_t indents_count) noexcept { return {msg_t::section_enter, string, indents_count}; }
			inline static constexpr message section_leave(const string::concepts::stringlike auto& string, size_t indents_count) noexcept { return {msg_t::section_leave, string, indents_count}; }

			constexpr std::string_view out_type() const noexcept;
			constexpr std::string_view out_type_verbose() const noexcept;
			constexpr utils::graphics::colour::base out_type_colour() const noexcept;

			constexpr std::chrono::time_point<std::chrono::system_clock> get_timestamp() const noexcept;

			constexpr std::strong_ordering operator<=>(const message& other) const noexcept;
			constexpr bool operator==(const message& other) const noexcept;

		private:
			inline constexpr message(msg_t type, const string::concepts::stringlike auto& string, std::chrono::time_point<std::chrono::system_clock> time, size_t indents_count) noexcept
				: type{type}, string{string}, time{time}, indents_count{indents_count}
				{}
			inline constexpr message(msg_t type, const string::concepts::stringlike auto& string, size_t indents_count) noexcept
				: message{type, string, std::chrono::system_clock::now(), indents_count}
				{}
			inline constexpr message(const string::concepts::stringlike auto& string, size_t indents_count) noexcept
				: message{msg_t::log, string, std::chrono::system_clock::now(), indents_count}
				{}

			msg_t type{msg_t::log};
			std::string string{};
			std::chrono::time_point<std::chrono::system_clock> time;
			size_t indents_count{0};

			static constexpr std::string filter_last_newline(const std::string& string) noexcept;

			void output_indent(std::stringstream& ss) const noexcept;

		public:
			template <bool coloured>
			std::ostream& output(std::ostream& os) const noexcept;
		};
	}

#ifdef utils_implementation
#include "message.cpp"
#endif