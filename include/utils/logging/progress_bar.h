#pragma once

#include <queue>
#include <mutex>
#include <atomic>
#include <thread>
#include <format>
#include <fstream>
#include <iostream>
#include <concepts>

#include "message.h"
#include "../console/colour.h"
#include "../containers/multithreading/self_consuming_queue.h"

#include "../oop/disable_move_copy.h"

namespace utils::logging
	{
	class progress_bar : utils::oop::non_copyable, utils::oop::non_movable
		{
		public:
			progress_bar() = default;
			progress_bar(float update_step, size_t bar_width) : update_step{update_step}, bar_width{bar_width} {}

			void advance(float new_state) noexcept
				{
				std::unique_lock lock{mutex};
				if (new_state <= state) { return; }
				state = new_state;

				const bool should_draw{(state == 1.f) || ((state - last_drawn_state) >= update_step)};
				if (!should_draw) { return; }

				last_drawn_state = state;
				message_queue.emplace(state);
				}

			void complete() noexcept
				{
				advance(1.f);
				message_queue.flush();
				std::cout << std::endl;
				}

		private:
			float state{0.f};
			float last_drawn_state{0.f};
			const float update_step{.01f};
			const size_t bar_width{20};
			std::mutex mutex;

			utils::containers::multithreading::self_consuming_queue<float, utils::containers::multithreading::operation_flag_bits::none> message_queue
				{
				[this](const float& element) -> void
					{
					draw(element);
					}
				};

			void draw(const float& percent) const noexcept
				{
				const size_t text_start_index{static_cast<size_t>(static_cast<float>(bar_width) / 2.f) - 2};
				const std::string percent_string{[&percent]()
					{
					auto ret{std::to_string(static_cast<size_t>(std::round(percent * 100.f))) + "%"};
					while (ret.size() < 4) { ret = ' ' + ret; }
					return ret;
					}()};

				const size_t bar_current_index{static_cast<size_t>(static_cast<float>(bar_width) * percent)};

				std::stringstream ss;
				ss << '\r';
				ss << utils::console::colour::restore_defaults << "[";
				for (size_t i{0}; i < bar_width; i++)
					{
					if (i < bar_current_index)
						{
						const uint8_t r{static_cast<uint8_t>(std::pow((1.f - percent), 1.f/2.2f) * 127.f)};
						const uint8_t g{static_cast<uint8_t>(std::pow(       percent , 1.f/2.2f) * 127.f)};
						//const uint8_t r{static_cast<uint8_t>((1.f - percent) * 127.f)};
						//const uint8_t g{static_cast<uint8_t>(       percent  * 127.f)};

						ss << utils::console::colour::background{utils::graphics::colour::rgb_u{r, g, uint8_t{0}}};
						}
					else if(i == bar_current_index)
						{
						ss << utils::console::colour::background{utils::graphics::colour::rgb_u{uint8_t{50}, uint8_t{50}, uint8_t{50}}};
						}

					if ((i >= text_start_index) && (i < (text_start_index + 4)))
						{
						ss << percent_string[i - text_start_index];
						}
					else
						{
						ss << ' ';
						}
					}
				ss << utils::console::colour::restore_defaults << "]";
				if (percent == 1.f) { ss << "\n\n"; }
				std::cout << ss.str() << std::flush;
				}
		};
	}