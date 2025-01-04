#pragma once

#include <algorithm>
#include <execution>
#include <filesystem>

#include "colour.h"
#include "../matrix.h"

namespace utils::graphics::image
	{
	template <utils::graphics::colour::concepts::colour T>
	utils::matrix<T> load_from_file(const std::filesystem::path& path);

	template <>
	utils::matrix<utils::graphics::colour::rgba_u> load_from_file(const std::filesystem::path& path);

	template <utils::graphics::colour::concepts::colour T>
	utils::matrix<T> load_from_file(const std::filesystem::path& path)
		{
		const auto rgba_u{load_from_file<utils::graphics::colour::rgba_u>(path)};
		utils::matrix<T> ret(rgba_u.sizes());

		const auto indices{std::ranges::iota_view<size_t, size_t>(size_t{0}, rgba_u.sizes().sizes_to_size())};
		std::for_each(std::execution::par_unseq, indices.begin(), indices.end(), [&rgba_u, &ret](const size_t& index)
			{
			ret[index] = static_cast<T>(rgba_u[index]);
			});
		return ret;
		}

	void save_to_file(const utils::matrix<utils::graphics::colour::rgba_u>& image, const std::filesystem::path& path);

	//template <utils::concepts::matrix image_t>
	//	requires(utils::graphics::colour::concepts::colour<typename image_t::value_type>)
	void save_to_file(const utils::matrix<utils::graphics::colour::rgba_f>& image, const std::filesystem::path& path);
	}

#ifdef utils_implementation
#include "image.cpp"
#endif