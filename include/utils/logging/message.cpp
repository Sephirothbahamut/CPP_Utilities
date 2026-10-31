#pragma once
#include "message.h"

#include <iomanip>
#include <sstream>

//TODO output readable date-time +GMT delta instead of timestamp
//Ideally: yyyy/mm/dd hh:mm:ss:millis

namespace utils::logging
	{
	output_style_t message::output_style{output_style_t::on_line};
			

	constexpr std::string_view message::out_type() const noexcept
		{
		switch (type)
			{
			case msg_t::log          : return "[LOG]";
			case msg_t::dgn          : return "[DGN]";
			case msg_t::inf          : return "[INF]";
			case msg_t::wrn          : return "[WRN]";
			case msg_t::err          : return "[ERR]";
			case msg_t::suc          : return "[SUC]";
			case msg_t::section_enter: return " >>> ";
			case msg_t::section_leave: return " <<< ";
			default: return "[This error code should be impossible to get]";
			}
		}
	constexpr std::string_view message::out_type_verbose() const noexcept
		{
		switch (type)
			{
			case msg_t::log          : return "[LOG]"       ;
			case msg_t::dgn          : return "[DIAGNOSTIC]";
			case msg_t::inf          : return "[INFO]"      ;
			case msg_t::wrn          : return "[WARNING]"   ;
			case msg_t::err          : return "[ERROR]"     ;
			case msg_t::suc          : return "[SUCCEEDED]" ;
			case msg_t::section_enter: return " >>> ";
			case msg_t::section_leave: return " <<< ";
			default: return "[This error code should be impossible to get]";
			}
		}
	constexpr utils::graphics::colour::base message::out_type_colour() const noexcept
		{
		switch (type)
			{
			case msg_t::log          : return utils::graphics::colour::base::white  ;
			case msg_t::dgn          : return utils::graphics::colour::base::magenta;
			case msg_t::inf          : return utils::graphics::colour::base::cyan   ;
			case msg_t::wrn          : return utils::graphics::colour::base::yellow ;
			case msg_t::err          : return utils::graphics::colour::base::red    ;
			case msg_t::suc          : return utils::graphics::colour::base::green  ;
			case msg_t::section_enter: return utils::graphics::colour::base::green  ;
			case msg_t::section_leave: return utils::graphics::colour::base::green  ;
			default                  : return utils::graphics::colour::base::red    ;
			}
		}

	constexpr std::chrono::time_point<std::chrono::system_clock> message::get_timestamp() const noexcept { return time; }

	constexpr std::strong_ordering message::operator<=>(const message& other) const noexcept
		{
		return get_timestamp() <=> other.get_timestamp();
		}
	constexpr bool message::operator==(const message& other) const noexcept
		{
		return get_timestamp() == other.get_timestamp();
		}

	constexpr std::string message::filter_last_newline(const std::string& string) noexcept
		{
		if (string.length() > 0 && string[string.length() - 1] == '\n') { return string.substr(0, string.length() - 1); }
		else { return string; }
		}

	void message::output_indent(std::stringstream& ss) const noexcept
		{
		for (size_t i{0}; i < indents_count; i++) { ss << "    "; }
		}

	template <bool coloured>
	std::ostream& message::output(std::ostream& os) const noexcept
		{
		if (type == msg_t::raw)
			{
			return os << string;
			}

		constexpr size_t timestamp_digits{std::numeric_limits<decltype(time.time_since_epoch().count())>::digits10};

		std::stringstream ss;

		//Data line
		if (output_style == output_style_t::tag_as_separator)
			{
			output_indent(ss);
			ss << "_________________________________\n";
			output_indent(ss);
			ss << ' ';

			if constexpr (coloured) { ss << utils::console::colour::background{utils::console::colour::colour_8::dark(out_type_colour())}; }
			if constexpr (coloured) { ss << utils::console::colour::foreground{utils::console::colour::colour_8::bright(utils::graphics::colour::base::white)}; }
			ss << out_type_verbose();
			if constexpr (coloured) { ss << utils::console::colour::restore_defaults; }

			for (size_t i = 0; i < (12 - out_type_verbose().length()); i++) { ss << ' '; }

			ss << ' ' << std::right << std::setw(timestamp_digits) << time.time_since_epoch().count() << '\n' << std::left;
			}

		//First line
		bool first_line{true};

		size_t index_beg = 0;

		//Lines
		while (index_beg < string.size())
			{
			size_t index_end = string.find_first_of('\n', index_beg);
			if (index_end == std::string::npos) { index_end = string.size(); }
			else { index_end++; }

			output_indent(ss);

			if (output_style == output_style_t::on_line)
				{
				ss << " ";
				if constexpr (coloured) { ss << utils::console::colour::background{utils::console::colour::colour_8::dark(out_type_colour())}; }
				if constexpr (coloured) { ss << utils::console::colour::foreground{utils::console::colour::colour_8::bright(utils::graphics::colour::base::white)}; }
				ss << out_type();
				if constexpr (coloured) { ss << utils::console::colour::restore_defaults; }
				ss << " ";

				if (first_line)
					{
					if constexpr (coloured) { ss << utils::console::colour::foreground{utils::console::colour::colour_8::dark(utils::graphics::colour::base::white)}; }
					ss << std::right << std::setw(timestamp_digits) << time.time_since_epoch().count() << std::left;
					if constexpr (coloured) { ss << utils::console::colour::restore_defaults; }

					first_line = false;
					}
				else
					{
					for (size_t i = 0; i < timestamp_digits - 1; i++) { ss << ' '; }
					ss << "|";
					}
				}

			ss << ' ' << std::string_view(string).substr(index_beg, index_end - index_beg);

			index_beg = index_end;
			}

		ss << "\n";
		const auto output{ss.str()};
		return os << output;
		}
	}

template std::ostream& utils::logging::message::output<true >(std::ostream& os) const;
template std::ostream& utils::logging::message::output<false>(std::ostream& os) const;