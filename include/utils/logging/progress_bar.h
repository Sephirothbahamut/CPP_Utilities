#pragma once

#include <mutex>
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
	

	template <bar_parameters bar = bar_parameters{}>
	class global_progress_bar_stack
		{
		public:
			using logger_t = ::utils::logging::logger<::utils::logging::message<::utils::logging::output_style_t::tag_as_separator>>;

		private:
			struct element_t
				{
				const size_t steps_count{100};
				const std::string label{};
				std::string step_label{};
				const std::chrono::time_point<std::chrono::system_clock> start_time;
				size_t current_step{0};
				float substep_percent{0.f};
				float progress_percent{0.f};
				
				float evaluate_progress_percent() const noexcept
					{
					const float current_step_f{static_cast<float>(current_step)};
					const float steps_count_f {static_cast<float>(steps_count )};
					const float ret{(current_step_f / steps_count_f) + (substep_percent / steps_count_f)};
					return ret;
					}

				float step() noexcept
					{
					assert(current_step < steps_count);
					current_step++;
					substep_percent = 0.f;
					return progress_percent = evaluate_progress_percent();
					}

				size_t lines_to_clear() const noexcept { return 2 + (label.empty() ? 0 : 1) + (step_label.empty() ? 0 : 1); }

				std::string state_as_string(size_t indent) const noexcept
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
					ret += string_indent + bar.evaluate_string(progress_percent) + "\n";
					ret += string_indent + string_count + ", " + string_time + "\n";
					if (step_label.size())
						{
						ret += string_indent + step_label + "\n";
						}
					return ret;
					}
				};

			std::vector<element_t> elements;
			std::mutex multithread_step_mutex;
			logger_t& logger;

			const element_t& last() const noexcept { return *elements.rbegin(); }
			      element_t& last()       noexcept { return *elements.rbegin(); }

			void remove_last() noexcept
				{
				elements.resize(elements.size() - 1);
				}
			void remove_last_no_exception() noexcept
				{
				std::unique_lock _{multithread_step_mutex};
				output_to_logger(clear_string(1));
				remove_last();
				}

			std::string clear_string(size_t quantity) noexcept
				{
				assert(quantity <= elements.size());
				size_t lines_to_clear{0};

				for (size_t i{elements.size() - quantity}; i < elements.size(); i++)
					{
					const auto& element{elements[i]};
					lines_to_clear += element.lines_to_clear();
					}
				const std::string ret{::utils::console::string::clear_lines(1 + lines_to_clear)};
				return ret;
				}
			std::string draw_string(size_t quantity) noexcept
				{
				assert(quantity <= elements.size());
				std::string ret;

				for (size_t i{elements.size() - quantity}; i < elements.size(); i++)
					{
					const auto& element{elements[i]};
					ret += element.state_as_string(i);
					}

				return ret;
				}

			/// <summary> Clears and redraws a specific amount of progress bars. </summary>
			void clear_and_draw(size_t quantity) noexcept
				{
				output_to_logger(clear_string(quantity) + draw_string(quantity));
				}

			void output_to_logger(const std::string& string) const noexcept
				{
				logger.raw(string);
				if constexpr (utils::compilation::debug)
					{
					logger.flush();
					}
				}

		public:
			global_progress_bar_stack(logger_t& logger) : logger{logger} {}

			struct scoped_bar_t
				{
				friend class global_progress_bar_stack;
				public:
					~scoped_bar_t()
						{
						if (!std::uncaught_exceptions())
							{
							owner.remove_last_no_exception();
							}
						else
							{
							owner.remove_last();
							}
						}

				private:
					scoped_bar_t(global_progress_bar_stack& owner) : owner{owner} {}
					global_progress_bar_stack& owner;
				};


			struct scoped_step_t
				{
				friend class global_progress_bar_stack;
				public:
					~scoped_step_t()
						{
						if (!std::uncaught_exceptions())
							{
							owner.step();
							}
						}

				private:
					scoped_step_t(global_progress_bar_stack& owner) : owner{owner} {}
					global_progress_bar_stack& owner;
				};

			/// <summary> Utility class to automatically advance steps of the current progress bar when the current scope is completed. </summary>
			[[nodiscard]] scoped_step_t scoped_step(const std::string_view& step_label = "") noexcept
				{
				const size_t reserve{last().step_label.size() + step_label.size()};
				if (reserve)
					{
					std::string output_string;
					output_string.reserve(reserve);
					if (last().step_label.size()) { output_string += ::utils::console::string::clear_lines(1 + 1); }
					if (step_label       .size()) 
						{
						const std::string string_indent((elements.size() - 1) * 4, ' ');
						output_string += string_indent + std::string{step_label} + "\n";
						}
					output_to_logger(output_string);
					}

				last().step_label = step_label;
				return scoped_step_t{*this};
				}

			/// <summary> Adds a child progress bar to the current progress bar </summary>
			/// <param name="steps_count"> How many steps the child progress bar will have </param>
			/// <param name="label">Label for the child progress bar. If empty the label line will be skipped entirely. </param>
			/// <returns></returns>
			[[nodiscard]] scoped_bar_t scoped_bar(size_t steps_count, const std::string_view& label = "") noexcept
				{
				elements.push_back(element_t
					{
					.steps_count{steps_count},
					.label      {label},
					.start_time {std::chrono::system_clock::now()}
					});
				
				output_to_logger(draw_string(1));
				return scoped_bar_t{*this};
				}

			/// <summary> 
			/// Completes one step of the leaf progress bar, updates the substep progress in parents until the difference is meaningless, then redraws the bars that changed.
			/// Change the template bar update_step parameter to change how parents are redrawn.
			/// </summary>
			void step() noexcept
				{
				assert(!elements.empty());
				std::unique_lock _{multithread_step_mutex};
				float child_progress_percent{last().step()};

				size_t i{1};
				for (; i < elements.size(); i++)
					{
					auto& element{*(elements.rbegin() + i)};
					element.substep_percent = child_progress_percent;
					const float previous_percent{element.progress_percent};
					const float current_percent {element.evaluate_progress_percent()};
					if ((current_percent - previous_percent) < bar.update_step)
						{
						break;
						}
					else
						{
						child_progress_percent = element.progress_percent = current_percent;
						}
					}
				clear_and_draw(i);
				}

			/// <summary> 
			/// Completes one step of the leaf progress bar and redraws it. Does not update any parent along the tree.
			/// Useful for when there's many fast steps to improve performance, like iterating the pixels of an image.
			/// </summary>
			void fast_step() noexcept
				{
				assert(!elements.empty());
				std::unique_lock _{multithread_step_mutex};
				last().step();
				clear_and_draw(1);
				}

			/// <summary> Redraws the entire tree without erasing previous lines. Use after something else logged to console (for example logging exceptions after exception recovery).</summary>
			void redraw()
				{
				output_to_logger(draw_string(elements.size()));
				};
		};
	}

#ifdef utils_implementation
#include "progress_bar.cpp"
#endif