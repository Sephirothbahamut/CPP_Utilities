#include "progress_bar.h"

namespace utils::logging
	{
	std::string bar_parameters::evaluate_string(float percent) const noexcept
		{
		assert(percent >= 0.f && percent <= 1.f);

		const size_t text_start_index{static_cast<size_t>(static_cast<float>(width) / 2.f) - 2};

		std::string ret;

		const std::string percent_string{[&percent]()
			{
			auto ret{std::to_string(static_cast<size_t>(std::round(percent * 100.f))) + "%"};
			while (ret.size() < 4) { ret = ' ' + ret; }
			return ret;
			}()};

		const size_t bar_current_index{static_cast<size_t>(static_cast<float>(width) * percent)};

		ret += utils::console::colour::to_string(utils::console::colour::restore_defaults) + "[";
		for (size_t i{0}; i < width; i++)
			{
			if (i == bar_current_index)
				{
				ret += utils::console::colour::background{utils::graphics::colour::rgb_u{uint8_t{50}, uint8_t{50}, uint8_t{255}}};
				}
			else
				{
				const float colour_multiplier = (i < bar_current_index) ? 127.f : 32.f;
				const float index_percent{static_cast<float>(i) / static_cast<float>(width)};
				const uint8_t r{static_cast<uint8_t>(std::pow((1.f - index_percent), 1.f / 2.2f) * colour_multiplier)};
				const uint8_t g{static_cast<uint8_t>(std::pow(index_percent , 1.f / 2.2f) * colour_multiplier)};

				ret += utils::console::colour::background{utils::graphics::colour::rgb_u{r, g, uint8_t{0}}};
				}

			if ((i >= text_start_index) && (i < (text_start_index + 4)))
				{
				ret += percent_string[i - text_start_index];
				}
			else
				{
				ret += ' ';
				}
			}
		ret += utils::console::colour::to_string(utils::console::colour::restore_defaults) + "]";
		return ret;
		}

	}