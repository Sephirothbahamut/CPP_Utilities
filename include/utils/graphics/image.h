#pragma once

#include <filesystem>

#include "colour.h"
#include "../matrix.h"

namespace utils::graphics::image
	{
	void save_to_file(const utils::matrix<utils::graphics::colour::rgba_u>& image, const std::filesystem::path& path);

	//template <utils::concepts::matrix image_t>
	//	requires(utils::graphics::colour::concepts::colour<typename image_t::value_type>)
	void save_to_file(const utils::matrix<utils::graphics::colour::rgba_f>& image, const std::filesystem::path& path);
	}

#ifdef utils_implementation
#include "image.cpp"
#endif