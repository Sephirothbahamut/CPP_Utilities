#include "progress_bar.h"

//TODO remove inlines and split into .cpp

namespace utils::logging
	{
	progress_bar::progress_bar() = default;
	progress_bar::progress_bar(float update_step, size_t bar_width) : update_step{update_step}, bar_width{bar_width} {}

	void progress_bar::advance(float new_state) noexcept
		{
		std::unique_lock lock{mutex};
		if (new_state <= state) { return; }
		state = new_state;

		const bool should_draw{(state == 1.f) || ((state - last_drawn_state) >= update_step)};
		if (!should_draw) { return; }

		last_drawn_state = state;
		message_queue.emplace(state);
		}

	void progress_bar::complete() noexcept
		{
		advance(1.f);
		message_queue.flush();
		std::cout << std::endl;
		}

	partial_progress progress_bar::partial_progress(size_t steps_count) noexcept
		{
		return utils::logging::partial_progress{*this, 0.f, 1.f, steps_count};
		}

	void progress_bar::draw(const float& percent) const noexcept
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
				const float index_percent{static_cast<float>(i) / static_cast<float>(bar_width)};
				const uint8_t r{static_cast<uint8_t>(std::pow((1.f - index_percent), 1.f / 2.2f) * 127.f)};
				const uint8_t g{static_cast<uint8_t>(std::pow(       index_percent , 1.f / 2.2f) * 127.f)};

				ss << utils::console::colour::background{utils::graphics::colour::rgb_u{r, g, uint8_t{0}}};
				}
			else if (i == bar_current_index)
				{
				ss << utils::console::colour::background{utils::graphics::colour::rgb_u{uint8_t{50}, uint8_t{50}, uint8_t{255}}};
				}
			else //if (i > bar_current_index)
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

	void partial_progress::advance() noexcept
		{
		std::unique_lock lock{mutex};
		index++;
		progress_bar.advance(from + (step * static_cast<float>(index)) );
		}

	partial_progress partial_progress::split_partial_progress(size_t steps_count) noexcept
		{
		const float split_from{from + (step * index)};
		const float split_to  {to   + (step * index)};
		return utils::logging::partial_progress{progress_bar, split_from, split_to, steps_count};
		}

	partial_progress::partial_progress(utils::logging::progress_bar& progress_bar, float from, float to, size_t steps_count) noexcept :
		progress_bar{progress_bar},
		from{from},
		to  {to},
		step{evaluate_step(from, to, steps_count)}
		{}

	float partial_progress::evaluate_step(float from, float to, size_t steps_count) noexcept
		{
		const float range{to - from};
		const float step{range / static_cast<float>(steps_count)};
		return step;
		}
	}