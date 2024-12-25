#pragma once
#include <chrono>
#include <string>
#include <sstream>
#include <iomanip>
#include <string_view>

#include "../string.h"
#include "../console/colour.h"

//TODO output readable date-time +GMT delta instead of timestamp
//Ideally: yyyy/mm/dd hh:mm:ss:millis

namespace utils::logging
	{
	enum class output_style_t { on_line, tag_as_separator };
	enum class msg_t { log, dgn, inf, wrn, err, section_enter, section_leave };

	template <output_style_t OUTPUT_STYLE>
	class message
		{
		public:
			inline static constexpr output_style_t output_style = OUTPUT_STYLE;

			constexpr message           (const message&  copy) = default;
			constexpr message& operator=(const message&  copy) = default;
			constexpr message           (      message&& move) = default;
			constexpr message& operator=(      message&& move) = default;

			static constexpr message log          (const string::concepts::stringlike auto& string = "", size_t indents_count = 0) noexcept { return {msg_t::log          , string, indents_count}; }
			static constexpr message dgn          (const string::concepts::stringlike auto& string = "", size_t indents_count = 0) noexcept { return {msg_t::dgn          , string, indents_count}; }
			static constexpr message inf          (const string::concepts::stringlike auto& string = "", size_t indents_count = 0) noexcept { return {msg_t::inf          , string, indents_count}; }
			static constexpr message wrn          (const string::concepts::stringlike auto& string = "", size_t indents_count = 0) noexcept { return {msg_t::wrn          , string, indents_count}; }
			static constexpr message err          (const string::concepts::stringlike auto& string = "", size_t indents_count = 0) noexcept { return {msg_t::err          , string, indents_count}; }
			static constexpr message section_enter(const string::concepts::stringlike auto& string = "", size_t indents_count = 0) noexcept { return {msg_t::section_enter, string, indents_count}; }
			static constexpr message section_leave(const string::concepts::stringlike auto& string = "", size_t indents_count = 0) noexcept { return {msg_t::section_leave, string, indents_count}; }

			constexpr std::string_view out_type() const noexcept
				{
				switch (type)
					{
					case msg_t::log          : return "[LOG]";
					case msg_t::dgn          : return "[DGN]";
					case msg_t::inf          : return "[INF]";
					case msg_t::wrn          : return "[WRN]";
					case msg_t::err          : return "[ERR]";
					case msg_t::section_enter: return " >>> ";
					case msg_t::section_leave: return " <<< ";
					default: return "[This error code should be impossible to get]";
					}
				}
			constexpr std::string_view out_type_verbose() const noexcept
				{
				switch (type)
					{
					case msg_t::log          : return "[LOG]"       ;
					case msg_t::dgn          : return "[DIAGNOSTIC]";
					case msg_t::inf          : return "[INFO]"      ;
					case msg_t::wrn          : return "[WARNING]"   ;
					case msg_t::err          : return "[ERROR]"     ;
					case msg_t::section_enter: return " >>> ";
					case msg_t::section_leave: return " <<< ";
					default: return "[This error code should be impossible to get]";
					}
				}
			constexpr utils::graphics::colour::base out_type_colour() const noexcept
				{
				switch (type)
					{
					case msg_t::log          : return utils::graphics::colour::base::white  ;
					case msg_t::dgn          : return utils::graphics::colour::base::magenta;
					case msg_t::inf          : return utils::graphics::colour::base::cyan   ;
					case msg_t::wrn          : return utils::graphics::colour::base::yellow ;
					case msg_t::err          : return utils::graphics::colour::base::red    ;
					case msg_t::section_enter: return utils::graphics::colour::base::green  ;
					case msg_t::section_leave: return utils::graphics::colour::base::green  ;
					default                  : return utils::graphics::colour::base::red    ;
					}
				}

			constexpr std::chrono::time_point<std::chrono::system_clock> get_timestamp() const noexcept { return time; }

			constexpr std::strong_ordering operator<=>(const message<output_style>& other) const noexcept
				{
				return get_timestamp() <=> other.get_timestamp();
				}
			constexpr bool operator==(const message<output_style>& other) const noexcept
				{
				return get_timestamp() == other.get_timestamp();
				}

		private:
			constexpr message(msg_t type, const string::concepts::stringlike auto& string, std::chrono::time_point<std::chrono::system_clock> time, size_t indents_count = 0) noexcept
				: type{type}, string{string}, time{time}, indents_count{indents_count}
				{}
			constexpr message(msg_t type, const string::concepts::stringlike auto& string, size_t indents_count = 0) noexcept
				: message{type, string, std::chrono::system_clock::now(), indents_count}
				{}
			constexpr message(const string::concepts::stringlike auto& string, size_t indents_count = 0) noexcept
				: message{msg_t::log, string, std::chrono::system_clock::now(), indents_count}
				{}

			msg_t type{msg_t::log};
			std::string string{};
			std::chrono::time_point<std::chrono::system_clock> time;
			size_t indents_count{0};

			static constexpr std::string filter_last_newline(const std::string& string) noexcept
				{
				if (string.length() > 0 && string[string.length() - 1] == '\n') { return string.substr(0, string.length() - 1); }
				else { return string; }
				}

			void output_indent(std::stringstream& ss) const noexcept
				{
				for (size_t i{0}; i < indents_count; i++) { ss << "    "; }
				}

			friend std::ostream& operator<<(std::ostream& os, const message& m)
				{
				constexpr size_t timestamp_digits{std::numeric_limits<decltype(m.time.time_since_epoch().count())>::digits10};

				std::stringstream ss;

				//Data line
				if constexpr (output_style == output_style_t::tag_as_separator)
					{
					m.output_indent(ss);
					ss << "_________________________________\n";
					m.output_indent(ss);
					ss << ' ';

					ss << utils::console::colour::background{utils::console::colour::colour_8::dark(m.out_type_colour())};
					ss << utils::console::colour::foreground{utils::console::colour::colour_8::bright(utils::graphics::colour::base::white)};
					ss << m.out_type_verbose();
					ss << utils::console::colour::restore_defaults;

					for (size_t i = 0; i < (12 - m.out_type_verbose().length()); i++) { ss << ' '; }

					ss << ' ' << std::right << std::setw(timestamp_digits) << m.time.time_since_epoch().count() << '\n' << std::left;
					}

				//First line
				bool first_line{true};

				size_t index_beg = 0;

				//Lines
				while (index_beg < m.string.size())
					{
					size_t index_end = m.string.find_first_of('\n', index_beg);
					if (index_end == std::string::npos) { index_end = m.string.size(); }
					else { index_end++; }

					m.output_indent(ss);

					if constexpr (output_style == output_style_t::on_line)
						{
						ss << " ";
						ss << utils::console::colour::background{utils::console::colour::colour_8::dark(m.out_type_colour())};
						ss << utils::console::colour::foreground{utils::console::colour::colour_8::bright(utils::graphics::colour::base::white)};
						ss << m.out_type();
						ss << utils::console::colour::restore_defaults;
						ss << " ";

						if (first_line)
							{
							ss << utils::console::colour::foreground{utils::console::colour::colour_8::dark(utils::graphics::colour::base::white)};
							ss << std::right << std::setw(timestamp_digits) << m.time.time_since_epoch().count() << std::left;
							ss << utils::console::colour::restore_defaults;

							first_line = false;
							}
						else
							{
							for (size_t i = 0; i < timestamp_digits - 1; i++) { ss << ' '; }
							ss << "|";
							}
						}

					ss << ' ' << std::string_view(m.string).substr(index_beg, index_end - index_beg);

					index_beg = index_end;
					}

				ss << "\n";
				const auto output{ss.str()};
				return os << output;
				}
		};

	namespace concepts
		{
		template <typename T>
		concept message = std::same_as<T, utils::logging::message<T::output_style>>;
		}
	}