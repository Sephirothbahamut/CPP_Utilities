#pragma once

#include <filesystem>

#include "colour.h"
#include "../matrix.h"
#include "../math/rect.h"
#include "../math/vec2.h"
#include "../math/geometry/sdf/return_types.h"

namespace utils::graphics::sdf
	{
	template <typename T>
	struct material
		{
		using value_type = T;

		utils_gpu_available constexpr material() noexcept = default;
		utils_gpu_available constexpr material(float shape_padding) noexcept : shape_padding{-shape_padding, -shape_padding, shape_padding, shape_padding} {};
		utils_gpu_available constexpr material(const utils::math::rect<float>& shape_padding) noexcept : shape_padding{shape_padding} {};

		utils::math::rect<float> shape_padding{-1.f, -1.f, 1.f, 1.f};

		utils_gpu_available constexpr virtual value_type sample(const utils::math::vec2f& coords, const utils::math::geometry::sdf::gradient_signed_distance& gsdf) = 0;
		};

	struct debug : material<utils::graphics::colour::rgba_f>
		{
		utils_gpu_available static constexpr float smoothstep(float edge0, float edge1, float x) noexcept
			{
			// Scale, bias and saturate x to 0..1 range
			x = std::clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
			// Evaluate polynomial
			return x * x * (3.f - 2.f * x);
			}

		utils_gpu_available constexpr debug() noexcept : material<utils::graphics::colour::rgba_f>{32.f} {};

		utils_gpu_available constexpr utils::graphics::colour::rgba_f gradient_sdf_from_gdist(utils::math::geometry::interactions::return_types::gradient_signed_distance gdist) noexcept
			{
			// Inigo Quilez fancy colors
			gdist.distance.value *= .006f;
			utils::math::vec3f colour = (gdist.distance.side().is_outside()) ? utils::math::vec3f{.9f, .6f, .3f} : utils::math::vec3f{.4f, .7f, .85f};
			colour = utils::math::vec3f{gdist.gradient.x() * .5f + .5f, gdist.gradient.y() * .5f + .5f, 1.f};
			colour *= 1.0f - 0.5f * std::exp(-16.0f * gdist.distance.absolute());
			colour *= 0.9f + 0.1f * std::cos(150.0f * gdist.distance.value);
			colour = utils::math::lerp(colour, utils::math::vec3f{1.f}, 1.f - smoothstep(0.f, .01f, gdist.distance.absolute()));

			if (gdist.distance.side().is_inside())
				{
				colour *= .5f;
				}

			return utils::graphics::colour::rgba_f
				{
				colour[0],
				colour[1],
				colour[2],
				1.f
				};
			}
		};

	template <typename T>
	constexpr utils::matrix<T> render(const material<T>& material, utils::math::vec2s resolution) noexcept
		{
		
		}
	}