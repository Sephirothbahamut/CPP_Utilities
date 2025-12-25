#pragma once

#include <chrono>
#include <string>
#include <format>
#include <iostream>

#include "logger.h"
#include "../console/io.h"
#include "../console/colour.h"
#include "../oop/disable_move_copy.h"

namespace utils::logging
	{
	struct bar_parameters
		{
		const size_t width{80};
		const float colour_multiplier_complete{127.f};
		const float colour_multiplier_pending{32.f};
		const float update_step{.01f};

		std::string evaluate_string(float percent) const noexcept;
		};

	template <bar_parameters bar = bar_parameters{} >
	class progress_bar : utils::oop::non_copyable, utils::oop::non_movable
		{
		public:
			using logger_t = utils::logging::logger<utils::logging::message<utils::logging::output_style_t::tag_as_separator>>;
			using self_t = progress_bar<bar>;
		private:
			struct child_create_info
				{
				const utils::observer_ptr<logger_t> logger_ptr{nullptr};
				const std::string label{};
				const size_t steps_count{100};

				const utils::observer_ptr<self_t> parent_ptr{nullptr};
				const size_t indent{0};
				};

			progress_bar(const child_create_info& create_info) :
				logger_ptr {create_info.logger_ptr},
				label      {create_info.label},
				steps_count{create_info.steps_count},
				parent_ptr {create_info.parent_ptr},
				indent     {create_info.indent},
				start_time{std::chrono::system_clock::now()}
				{
				print_string(evaluate_self_draw_string());
				}

		public:
			struct create_info
				{
				const utils::observer_ptr<logger_t> logger_ptr{nullptr};
				const std::string label{};
				const size_t steps_count{100};
				};

			const utils::observer_ptr<logger_t> logger_ptr{nullptr};
			const std::string label{};
			const size_t steps_count{100};

			const utils::observer_ptr<self_t> parent_ptr{nullptr};
			const size_t indent{0};

		private:
			size_t current_step{0};
			float substep_percent{0.f};

			float last_drawn_percent{-bar.update_step};
			float current_percent{0.f};
			std::chrono::time_point<std::chrono::system_clock> start_time;

		public:
			progress_bar(const create_info& create_info) :
				logger_ptr{create_info.logger_ptr},
				label{create_info.label},
				steps_count{create_info.steps_count},
				start_time{std::chrono::system_clock::now()}
				{
				print_string(evaluate_self_draw_string());
				}

			~progress_bar()
				{
				if (!std::uncaught_exceptions())
					{
					print_string(evaluate_self_erase_string());
					}
				}

			void advance() noexcept
				{
				assert(current_step < steps_count);
				current_step++;
				substep_percent = 0.f;
				current_percent = evaluate_progress_percent();
				if (parent_ptr)
					{
					parent_ptr->set_substep_percent(current_percent);
					}

				draw();
				}

			void draw() noexcept
				{
				const std::string output{evaluate_tree_erase_draw_string()};
				if (output.size())
					{
					print_string(output);
					}
				}
			void draw_new_lines() const noexcept
				{
				print_string(evaluate_tree_draw_string());
				}

			self_t split(size_t steps_count, std::string label = "") noexcept
				{
				return self_t{child_create_info
					{
					.logger_ptr {logger_ptr},
					.label      {label},
					.steps_count{steps_count},
					.parent_ptr {this},
					.indent     {indent + 1}
					}};
				}

		private:
			void print_string(const std::string& string) const noexcept
				{
				if (logger_ptr)
					{
					logger_ptr->raw(string);
					//logger_ptr->flush();
					}
				else
					{
					std::cout << string << std::flush;
					}
				}
			float evaluate_progress_percent() const noexcept
				{
				const float current_step_f{static_cast<float>(current_step)};
				const float steps_count_f{static_cast<float>(steps_count)};
				return (current_step_f / steps_count_f) + (substep_percent / steps_count_f);
				}

			void set_substep_percent(const float percent) noexcept
				{
				assert(percent >= 0.f && percent <= 1.f);

				substep_percent = percent;
				current_percent = evaluate_progress_percent();
				if (parent_ptr)
					{
					parent_ptr->set_substep_percent(current_percent);
					}
				}

			std::string evaluate_tree_erase_draw_string() noexcept
				{
				const float percent_delta{current_percent - last_drawn_percent};
				const bool should_draw{(current_percent == 1.f) || (percent_delta >= bar.update_step)};
				if (!should_draw)
					{
					return {};
					}
				else
					{
					last_drawn_percent = current_percent;
					}

				std::string ret;
				ret += evaluate_self_erase_string();
				ret += evaluate_parent_erase_draw_string();
				ret += evaluate_self_draw_string();

				return ret;
				}

			std::string evaluate_tree_erase_string() const noexcept
				{
				std::string ret;
				ret += evaluate_self_erase_string();
				if (parent_ptr) { ret += parent_ptr->evaluate_tree_erase_string(); }
				return ret;
				}
			std::string evaluate_tree_draw_string() const noexcept
				{
				std::string ret;
				if (parent_ptr) { ret += parent_ptr->evaluate_tree_draw_string(); }
				ret += evaluate_self_draw_string();
				return ret;
				}

			std::string evaluate_self_draw_string() const noexcept
				{
				const auto now{std::chrono::system_clock::now()};
				const auto delta_time{now - start_time};
				const std::string string_time{std::format("duration: {:%T}", delta_time)};
				const std::string string_count{"completed " + std::to_string(current_step) + " / " + std::to_string(steps_count)};
				const std::string string_indent(indent * 4, ' ');

				std::string ret;
				if (label.size())
					{
					ret += string_indent + " " + label + "\n";
					}
				ret += string_indent + bar.evaluate_string(evaluate_progress_percent()) + "\n";
				ret += string_indent + string_count + ", " + string_time + "\n";
				return ret;
				}
			std::string evaluate_parent_erase_draw_string() const noexcept
				{
				if (parent_ptr)
					{
					return parent_ptr->evaluate_tree_erase_draw_string();
					}
				return {};
				}
			std::string evaluate_self_erase_string() const noexcept
				{
				std::string ret{utils::console::string::cursor_up() + utils::console::string::clear_line() + utils::console::string::cursor_up() + utils::console::string::clear_line()};
				if (label.size()) { ret += utils::console::string::cursor_up() + utils::console::string::clear_line(); }
				return ret;
				}

		public:
			struct step_t
				{
				std::reference_wrapper<progress_bar> parent_ref;
				~step_t()
					{
					if (!std::uncaught_exceptions())
						{
						parent_ref.get().advance();
						}
					}
				};
			step_t step() noexcept { return step_t{*this}; }
		};
	}

#ifdef utils_implementation
#include "progress_bar.cpp"
#endif