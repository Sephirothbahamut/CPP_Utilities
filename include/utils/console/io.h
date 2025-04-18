#pragma once

#include "../math/vec.h"

namespace utils::console
	{
	char get_ch() noexcept;

	utils::math::vec2s size() noexcept;
	}

#ifdef utils_implementation
#include "io.cpp"
#endif