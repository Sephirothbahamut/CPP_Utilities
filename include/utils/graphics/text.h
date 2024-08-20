#pragma once

#include <vector>

#include "../math/geometry/shape/mixed.h"

namespace utils::graphics::text
	{
	using glyph_t = utils::math::geometry::shape::mixed<utils::math::geometry::ends::closeable::create::closed()>;
	std::vector<glyph_t> glyphs_from_string(const std::string& text, const std::wstring& font = L"Gabriola");
	}

#ifdef utils_implementation
#include "text.cpp"
#endif