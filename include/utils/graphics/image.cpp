#include "image.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable: 4996)

#include "../third_party/stb_image.h"
#include "../third_party/stb_image_write.h"

namespace utils::graphics::image
	{
	/*template <utils::concepts::matrix image_t>
		requires(utils::graphics::colour::concepts::colour<typename image_t::value_type>)
	void save_to_file(const image_t& image, const std::filesystem::path& path)
		{
		}*/
	void save_to_file(const utils::matrix<utils::graphics::colour::rgba_u>& image, const std::filesystem::path& path)
		{
		stbi_write_png(path.string().c_str(), static_cast<int>(image.width()), static_cast<int>(image.height()), 4, image.data(), static_cast<int>(image.width() * 4));
		}
	}