#pragma once

#include <utility>

#include "../compilation/compiler.h"

namespace utils::oop
	{
	struct utils_oop_empty_bases empty {};

	template <auto condition, typename type>
	using type_or_nothing = std::conditional_t<condition, type, empty>;
	}