#pragma once

#include "../math/vec.h"

namespace utils::console
	{
	char get_ch() noexcept;

	utils::math::vec2s size() noexcept;

	namespace string
		{
		std::string clear_line() noexcept;
		std::string cursor_up() noexcept;
		std::string clear_lines(size_t count) noexcept;
		}

	void clear_line() noexcept;
	void cursor_up() noexcept;
	void clear_lines(size_t count) noexcept;
	}

#ifdef utils_implementation
#include "io.cpp"
#endif