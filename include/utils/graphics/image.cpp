#include "image.h"

#include <ranges>
#include <algorithm>
#include <execution>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable: 4996)

#include "../third_party/stb_image.h"
#include "../third_party/stb_image_write.h"

namespace utils::graphics::image
	{
	template <>
	utils::matrix<utils::graphics::colour::rgba_u> load_from_file(const std::filesystem::path& path)
		{
		int width, height, channels;
		unsigned char* bytes{stbi_load(path.string().c_str(), &width, &height, &channels, 0)};

		utils::matrix<utils::graphics::colour::rgba_u> ret{utils::math::vec2s{static_cast<size_t>(width), static_cast<size_t>(height)}};

		size_t bytes_index{0};
		for (size_t y = 0; y < height; y ++)
			{
			for (size_t x = 0; x < width; x++)
				{
				const utils::math::vec2s coords{x, y};
				auto& pixel{ret[coords]};

				for (size_t c = 0; c < channels; c++)
					{
					pixel[c] = bytes[bytes_index];
					bytes_index++;
					}
				}
			}

		return ret;
		}


	void save_to_file(const utils::matrix<utils::graphics::colour::rgba_u>& image, const std::filesystem::path& path)
		{
		std::filesystem::path directory{path};
		directory.remove_filename();
		std::filesystem::create_directories(directory);
		stbi_write_png(path.string().c_str(), static_cast<int>(image.width()), static_cast<int>(image.height()), 4, image.data(), static_cast<int>(image.width() * 4));
		}
	
	void save_to_file(const utils::matrix<utils::graphics::colour::rgba_f>& image, const std::filesystem::path& path)
		{
		utils::matrix<utils::graphics::colour::rgba_u> image_u(image.sizes());

		std::ranges::iota_view indices(size_t{0}, image.sizes().sizes_to_size());
		std::for_each(std::execution::par, indices.begin(), indices.end(), [&image_u, &image](size_t index)
			{
			const utils::graphics::colour::rgba_f& colour{image[index]};
			const utils::graphics::colour::rgba_u colour_u
				{
				utils::math::cast_clamp<uint8_t>(colour[0] * 255.f),
				utils::math::cast_clamp<uint8_t>(colour[1] * 255.f),
				utils::math::cast_clamp<uint8_t>(colour[2] * 255.f),
				utils::math::cast_clamp<uint8_t>(colour[3] * 255.f)
				};
			image_u[index] = colour_u;
			});

		save_to_file(image_u, path);
		}
	}