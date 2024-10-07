#pragma once

#include <algorithm>
#include <execution>
#include <filesystem>

#include "colour.h"
#include "multisampling.h"

#include "../matrix.h"
#include "../math/rect.h"
#include "../math/vec2.h"
#include "../math/geometry/shape/sdf/return_types.h"
#include "../math/geometry/shape/shapes_group.h"

namespace utils::graphics::sdf
	{
	template <typename T>
	struct renderer
		{
		using value_type = T;

		utils_gpu_available constexpr renderer() noexcept = default;
		utils_gpu_available constexpr renderer(float shape_padding) noexcept : shape_padding{-shape_padding, -shape_padding, shape_padding, shape_padding} {};
		utils_gpu_available constexpr renderer(const utils::math::rect<float>& shape_padding) noexcept : shape_padding{shape_padding} {};

		utils::math::rect<float> shape_padding{-1.f, -1.f, 1.f, 1.f};

		utils_gpu_available constexpr virtual value_type sample(const utils::math::vec2f& coords, const utils::math::geometry::sdf::gradient_signed_distance& gsdf) const noexcept = 0;

		template <bool parallel = true>
		constexpr utils::matrix<T> render(const utils::matrix<utils::math::geometry::sdf::gradient_signed_distance>& gradient_signed_distance_field)
			{
			const auto resolution{gradient_signed_distance_field.sizes()};
			utils::matrix<T, matrix_size::create::dynamic()> ret(resolution);

			std::ranges::iota_view indices(size_t{0}, resolution.sizes_to_size());

			const auto callback{[&gradient_signed_distance_field, &ret, &resolution, this](size_t index)
				{
				const utils::math::vec2s coords_indices{ret.sizes().index_to_coords(index)};
				const utils::math::vec2f coords_f
					{
					static_cast<float>(coords_indices.x()),
					static_cast<float>(coords_indices.y())
					};

				auto& pixel{ret[index]};
				const auto& gradient_signed_distance{gradient_signed_distance_field[index]};

				pixel = sample(coords_f, gradient_signed_distance);
				}};

			if constexpr (parallel)
				{
				std::for_each(std::execution::par, indices.begin(), indices.end(), callback);
				}
			else if constexpr (!parallel)
				{
				std::for_each(indices.begin(), indices.end(), callback);
				}

			return ret;
			}
		};

	struct debug : renderer<utils::graphics::colour::rgba_f>
		{
		utils_gpu_available static constexpr float smoothstep(float edge0, float edge1, float x) noexcept
			{
			// Scale, bias and saturate x to 0..1 range
			x = std::clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
			// Evaluate polynomial
			return x * x * (3.f - 2.f * x);
			}

		utils_gpu_available constexpr debug() noexcept : renderer<utils::graphics::colour::rgba_f>{32.f} {};

		utils_gpu_available constexpr virtual utils::graphics::colour::rgba_f sample(const utils::math::vec2f& coords, const utils::math::geometry::sdf::gradient_signed_distance& gradient_signed_distance) const noexcept final override
			{
			const auto grad {gradient_signed_distance.gradient};
			const auto side {gradient_signed_distance.distance.side()};
			auto sdist{gradient_signed_distance.distance.value};

			sdist *= .006f;
			const auto dist{utils::math::abs(sdist)};
			if (dist == utils::math::constants::finf)
				{
				return utils::graphics::colour::rgba_f{0.f, 0.f, 0.f, 0.f};
				}

			// Inigo Quilez fancy colors
			utils::math::vec3f colour = (side.is_outside()) ? utils::math::vec3f{.9f, .6f, .3f} : utils::math::vec3f{.4f, .7f, .85f};
			colour = utils::math::vec3f{grad.x() * .5f + .5f, grad.y() * .5f + .5f, 1.f};
			colour *= 1.0f - 0.5f * std::exp(-16.0f *  dist);
			colour *= 0.9f + 0.1f * std::cos(150.0f * sdist);
			colour = utils::math::lerp(colour, utils::math::vec3f{1.f}, 1.f - smoothstep(0.f, .01f, dist));

			if (side.is_inside())
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

	template <typename T, typename execution_policy_t>
	constexpr utils::matrix<T> render
		(
		execution_policy_t execution_policy, 
		const renderer<T>& renderer, 
		utils::math::vec2s resolution,
		const utils::math::geometry::shapes_group::observers& shapes
		) noexcept
		{
		auto bounding_boxes{shapes.evaluate_bounding_boxes()};
		std::for_each(std::execution::par, bounding_boxes.begin(), bounding_boxes.end(), [&renderer](auto& bounding_box)
			{
			bounding_box.ll() += renderer.shape_padding.ll();
			bounding_box.up() += renderer.shape_padding.up();
			bounding_box.rr() += renderer.shape_padding.rr();
			bounding_box.dw() += renderer.shape_padding.dw();
			});

		utils::matrix<T, matrix_size::create::dynamic()> ret(resolution);

		std::ranges::iota_view indices(size_t{0}, resolution.sizes_to_size());
		std::for_each(execution_policy, indices.begin(), indices.end(), [&ret, &renderer, &resolution, &shapes, &bounding_boxes](size_t index)
			{
			const utils::math::vec2s coords_indices{resolution.index_to_coords(index)};
			const utils::math::vec2f coords_f
				{
				static_cast<float>(coords_indices.x()),
				static_cast<float>(coords_indices.y())
				};

			T& pixel{ret[index]};

			//utils::graphics::multisample
			utils::math::geometry::sdf::gradient_signed_distance gradient_signed_distance;

			for (size_t i{0}; i < shapes.observer_shapes.size(); i++)
				{
				const auto& bounding_box {bounding_boxes        [i]};
				if (!bounding_box.contains(coords_f)) { continue; }

				const auto& shape_variant{shapes.observer_shapes[i]};

				std::visit([&coords_f, &gradient_signed_distance](const auto& shape)
					{
					const auto shape_gradient_signed_distance{shape.sdf(coords_f).gradient_signed_distance()};
					gradient_signed_distance = utils::math::geometry::sdf::gradient_signed_distance::merge(gradient_signed_distance, shape_gradient_signed_distance);
					}, shape_variant);
				}

			pixel = renderer.sample(coords_f, gradient_signed_distance);
			});

		return ret;
		}

	template <typename T>
	constexpr utils::matrix<T> render(const renderer<T>& renderer, utils::math::vec2s resolution, const utils::math::geometry::shapes_group::observers& shapes) noexcept
		{
		return render(std::execution::seq, renderer, resolution, shapes);
		}






	using merge_function_signature = utils::math::geometry::sdf::gradient_signed_distance(utils::math::geometry::sdf::gradient_signed_distance, utils::math::geometry::sdf::gradient_signed_distance);
	using merge_function_t = std::function<merge_function_signature>;

	struct evaluate_sdf_params
		{
		utils::math::geometry::shape::aabb                                   shape_padding {-1.f, -1.f, 0.f, 0.f};
		const merge_function_t                                             & merge_function{&utils::math::geometry::sdf::gradient_signed_distance::merge};
		utils::matrix<utils::math::geometry::sdf::gradient_signed_distance>& gradient_signed_distance_field;
		};

	template <bool parallel = true>
	constexpr utils::matrix<utils::math::geometry::sdf::gradient_signed_distance>& evaluate_sdf
		(
		evaluate_sdf_params params,
		const utils::math::geometry::shape::concepts::shape auto& shape
		) noexcept
		{
		auto bounding_box{shape.bounding_box()};
		bounding_box.ll() += params.shape_padding.ll();
		bounding_box.up() += params.shape_padding.up();
		bounding_box.rr() += params.shape_padding.rr();
		bounding_box.dw() += params.shape_padding.dw();
		const utils::math::rect<size_t> pixels_region
			{
			         utils::math::cast_clamp<size_t>(std::floor(bounding_box.ll())),
			         utils::math::cast_clamp<size_t>(std::floor(bounding_box.up())),
			std::min(utils::math::cast_clamp<size_t>(std::ceil (bounding_box.rr())), params.gradient_signed_distance_field.sizes().x()),
			std::min(utils::math::cast_clamp<size_t>(std::ceil (bounding_box.dw())), params.gradient_signed_distance_field.sizes().y()),
			};
		const size_t indices_end{pixels_region.size().sizes_to_size()};

		std::ranges::iota_view indices(size_t{0}, indices_end);
		const auto callback{[&shape, &params, &pixels_region](size_t index)
			{
			const utils::math::vec2s coords_indices{pixels_region.ul() + pixels_region.size().index_to_coords(index)};
			const utils::math::vec2f coords_f
				{
				static_cast<float>(coords_indices.x()),
				static_cast<float>(coords_indices.y())
				};

			utils::math::geometry::sdf::gradient_signed_distance& value_at_pixel{params.gradient_signed_distance_field[coords_indices]};

			const utils::math::geometry::sdf::gradient_signed_distance shape_gradient_signed_distance{shape.sdf(coords_f).gradient_signed_distance()};
			value_at_pixel = params.merge_function(value_at_pixel, shape_gradient_signed_distance);
			}};
		
		if constexpr (parallel)
			{
			std::for_each(std::execution::par, indices.begin(), indices.end(), callback);
			}
		else if constexpr (!parallel)
			{
			std::for_each(indices.begin(), indices.end(), callback);
			}

		return params.gradient_signed_distance_field;
		}
	}