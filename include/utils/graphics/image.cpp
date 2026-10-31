#include "image.h"

#include <ranges>
#include <algorithm>
#include <execution>

#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable: 4996)

#include "../third_party/stb_image.h"
#include "../third_party/stb_image_write.h"

#ifdef utils_graphics_image_use_libpng
#include <libpng16/png.h>
#include "../third_party/png++/png.hpp"
#endif 

namespace utils::graphics::image
	{
	template <>
	std::expected<utils::matrix<utils::graphics::colour::rgba_u>, std::string> load_from_file(const std::filesystem::path& path)
		{
		int width, height, channels;
		unsigned char* bytes{stbi_load(path.string().c_str(), &width, &height, &channels, 0)};

		if (!bytes)
			{
			return std::unexpected{stbi_failure_reason()};
			}

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

		stbi_image_free(bytes);

		return ret;
		}


	template <utils::graphics::colour::concepts::colour pixel_t>
	void save_to_file(const utils::matrix<pixel_t>& image, const std::filesystem::path& path)
		{
		std::filesystem::path directory{path};
		directory.remove_filename();
		if (!directory.empty())
			{
			std::filesystem::create_directories(directory);
			}

		#ifdef utils_graphics_image_use_libpng

		using png_pixel_type = std::conditional_t
			<
			std::same_as<pixel_t, utils::graphics::colour::rgb_u>,
			png::rgb_pixel,
			std::conditional_t
				<
				std::same_as<pixel_t, utils::graphics::colour::rgba_u>,
				png::rgba_pixel,
				std::conditional_t
					<
					std::same_as<pixel_t, utils::graphics::colour::greyscale_u>,
					png::gray_pixel,
					std::conditional_t
						<
						std::same_as<pixel_t, utils::graphics::colour::greyscale_a_u>,
						png::ga_pixel,
						void
						>
					>
				>
			>;

		if constexpr(std::same_as<png_pixel_type, void>)
			{
			png::image<png_pixel_type> png_image{static_cast<uint32_t>(image.width()), static_cast<uint32_t>(image.height())};
			
			for (size_t y = 0; y < image.height(); y++)
				{
				for (size_t x = 0; x < image.width(); x++)
					{
					const auto& source_pixel{image[{x, y}]};
					auto& png_pixel{png_image[y][x]};

					if constexpr (utils::graphics::colour::concepts::additive<pixel_t>)
						{
						png_pixel.red   = source_pixel.r();
						png_pixel.green = source_pixel.g();
						png_pixel.blue  = source_pixel.b();
						}
					else if constexpr (std::same_as<pixel_t, utils::graphics::colour::greyscale_a_u>)
						{
						png_pixel.value = source_pixel.w();
						}
					else if constexpr (std::same_as<pixel_t, utils::graphics::colour::greyscale_u>)
						{
						png_pixel = source_pixel.w();;
						}

					if (pixel_t::static_has_alpha)
						{
						png_pixel.alpha = source_pixel.a();
						}
					}
				}

			png_image.write(path.string().c_str());
			return;
			}

		
		#else
		if constexpr (std::same_as<pixel_t, utils::graphics::colour::rgba_u>)
			{
			stbi_write_png(path.string().c_str(), static_cast<int>(image.width()), static_cast<int>(image.height()), 4, image.data(), static_cast<int>(image.width() * 4));
			return;
			}
		else if constexpr (std::same_as<pixel_t, utils::graphics::colour::rgb_u>)
			{
			stbi_write_png(path.string().c_str(), static_cast<int>(image.width()), static_cast<int>(image.height()), 4, image.data(), static_cast<int>(image.width() * 3));
			return;
			}
		else if constexpr (std::same_as<pixel_t, utils::graphics::colour::greyscale_a_u>)
			{
			stbi_write_png(path.string().c_str(), static_cast<int>(image.width()), static_cast<int>(image.height()), 4, image.data(), static_cast<int>(image.width() * 2));
			return;
			}
		else if constexpr (std::same_as<pixel_t, utils::graphics::colour::greyscale_u>)
			{
			stbi_write_png(path.string().c_str(), static_cast<int>(image.width()), static_cast<int>(image.height()), 4, image.data(), static_cast<int>(image.width() * 1));
			return;
			}
		#endif

		using unsigned_pixel_type = std::conditional
			<
			utils::graphics::colour::concepts::hsv<pixel_t>, //?
			utils::graphics::colour::rgb<uint8_t, pixel_t::static_has_alpha>, //:
			typename pixel_t::related_types::storage_unsigned
			>::type;
		
		utils::matrix<unsigned_pixel_type>image_unsigned{image.sizes()};
		image.for_each_to_other(image_unsigned, [](const pixel_t& source_pixel)
			{
			return unsigned_pixel_type{source_pixel};
			});

		save_to_file(image_unsigned, path);
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



#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "../third_party/stb_image.h"
#include "../third_party/stb_image_write.h"