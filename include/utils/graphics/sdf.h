#pragma once

#include <cmath>
#include <algorithm>
#include <execution>
#include <filesystem>

#include "colour.h"
#include "multisampling.h"

#include "../matrix.h"
#include "../math/rect.h"
#include "../math/vec.h"
#include "../math/transform2.h"
#include "../math/geometry/shape/aabb.h"
#include "../math/geometry/transform/aabb.h"
#include "../math/geometry/transform/point.h"
#include "../math/geometry/sdf/common.h"

namespace utils::graphics::sdf
	{
	template <typename T>
	using per_pixel_signature = void(T&, const utils::math::vec2f& coords_f);
	template <typename T>
	using per_pixel_callback = std::function<per_pixel_signature<T>>;

	template <typename T>
	struct callback_at_coords
		{
		const utils::math::transform2 camera_transform{};
		const float supersampling{1.f};
		const per_pixel_callback<T>& per_pixel_callback;
		utils::matrix<T>& image;

		T& operator()(size_t index) noexcept
			{
			return operator()(image.sizes().index_to_coords(index));
			}
		T& operator()(const utils::math::vec2s coords_indices) noexcept
			{
			const utils::math::vec2f coords_f
				{
				utils::math::vec2f
					{
					static_cast<float>(coords_indices.x()),
					static_cast<float>(coords_indices.y())
					}
				.transform(camera_transform)
				.scale(1.f / supersampling)
				};

			T& value_at_pixel{image[coords_indices]};
			per_pixel_callback(value_at_pixel, coords_f);
			return value_at_pixel;
			}
		};
	
	template <typename T, bool parallel = true>
	constexpr utils::matrix<T>& evaluate_full_image
		(
		const utils::math::transform2& camera_transform,
		const per_pixel_callback<T>& per_pixel_callback,
		utils::matrix<T>& image,
		float supersampling = 1.f
		) noexcept
		{
		const size_t indices_end{image.sizes().sizes_to_size()};
		std::ranges::iota_view indices(size_t{0}, indices_end);

		callback_at_coords<T> callback_at_coords
			{
			.camera_transform  {camera_transform  },
			.supersampling     {supersampling     },
			.per_pixel_callback{per_pixel_callback},
			.image             {image}
			};

		if constexpr (parallel)
			{
			std::for_each(std::execution::par, indices.begin(), indices.end(), callback_at_coords);
			}
		else if constexpr (!parallel)
			{
			std::for_each(indices.begin(), indices.end(), callback_at_coords);
			}
		}

	template <typename T, bool parallel = true>
	constexpr utils::matrix<T>& evaluate_in_region
		(
		const utils::math::transform2& camera_transform,
		const per_pixel_callback<T>& per_pixel_callback,
		const utils::math::geometry::shape::aabb& pixels_region_f,
		utils::matrix<T>& image,
		float supersampling = 1.f
		) noexcept
		{
		const utils::math::rect<size_t> pixels_region
			{
			utils::math::rect<size_t>
				{
				         utils::math::cast_clamp<size_t>(std::floor(pixels_region_f.ll())),
				         utils::math::cast_clamp<size_t>(std::floor(pixels_region_f.up())),
				std::min(utils::math::cast_clamp<size_t>(std::ceil (pixels_region_f.rr())), image.sizes().x()),
				std::min(utils::math::cast_clamp<size_t>(std::ceil (pixels_region_f.dw())), image.sizes().y())
				}
			};
		const size_t indices_end{pixels_region.size().sizes_to_size()};
		std::ranges::iota_view indices(size_t{0}, indices_end);
		
		callback_at_coords<T> callback_at_coords
			{
			.camera_transform  {camera_transform  },
			.supersampling     {supersampling     },
			.per_pixel_callback{per_pixel_callback},
			.image             {image}
			};

		const auto callback_at_coords_wrapper{[&](size_t index)
			{
			const utils::math::vec2s coords_indices{pixels_region.ul() + pixels_region.size().index_to_coords(index)};
			callback_at_coords(coords_indices);
			}};
		
		if constexpr (parallel)
			{
			std::for_each(std::execution::par, indices.begin(), indices.end(), callback_at_coords_wrapper);
			}
		else if constexpr (!parallel)
			{
			std::for_each(indices.begin(), indices.end(), callback_at_coords_wrapper);
			}

		return image;
		}















	using sample_dsdf_signature = utils::math::geometry::sdf::direction_signed_distance(const utils::math::vec2f& coords_f);
	using sample_dsdf_callback  = std::function<sample_dsdf_signature>;

	using merge_signature = utils::math::geometry::sdf::direction_signed_distance(utils::math::geometry::sdf::direction_signed_distance, utils::math::geometry::sdf::direction_signed_distance);
	using merge_callback  = std::function<merge_signature>;

	template <typename T>
	struct renderer
		{
		using value_type = T;

		utils_gpu_available constexpr renderer() noexcept = default;
		utils_gpu_available constexpr renderer(float shape_padding) noexcept : shape_padding{-shape_padding, -shape_padding, shape_padding, shape_padding} {};
		utils_gpu_available constexpr renderer(const utils::math::rect<float>& shape_padding) noexcept : shape_padding{shape_padding} {};

		utils::math::rect<float> shape_padding{-1.f, -1.f, 1.f, 1.f};

		utils_gpu_available constexpr virtual value_type sample(const utils::math::vec2f& coords, const utils::math::geometry::sdf::direction_signed_distance& dsdf) const noexcept = 0;

		/// <summary>
		/// Applies the renderer to a precalculated direction signed distance field.
		/// Use together with `utils::graphics::sdf::evaluate_dsdf`.
		/// </summary>
		/// <typeparam name="parallel">Self explanatory :)</typeparam>
		/// <param name="direction_signed_distance_field"></param>
		/// <returns>An image with the same resolution as the input direction signed distance field</returns>
		template <bool parallel = true>
		constexpr utils::matrix<T> render(const utils::math::transform2& camera_transform, const utils::matrix<utils::math::geometry::sdf::direction_signed_distance>& direction_signed_distance_field, float supersampling = 1.f)
			{
			const auto resolution{direction_signed_distance_field.sizes()};
			utils::matrix<T, matrix_size::create::dynamic()> ret(resolution);

			const auto callback{[&, this](size_t index)
				{
				const utils::math::vec2s coords_indices{ret.sizes().index_to_coords(index)};
				const utils::math::vec2f coords_f
					{
					utils::math::vec2f
						{
						static_cast<float>(coords_indices.x()),
						static_cast<float>(coords_indices.y())
						}
					.transform(camera_transform)
					.scale    (1.f / supersampling)
					};

				auto& pixel{ret[index]};
				const auto& direction_signed_distance{direction_signed_distance_field[index]};

				pixel = sample(coords_f, direction_signed_distance);
				}};

			std::ranges::iota_view indices(size_t{0}, resolution.sizes_to_size());
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



		/// <summary>
		/// Applies the renderer sampling the direction distances for each pixel all at once.
		/// </summary>
		/// <typeparam name="parallel"></typeparam>
		/// <param name="resolution"></param>
		/// <param name="sample_dsdf_callback">A lambda which takes coordinates to be sampled. It's up to the lambda to capture the shapes and calculate the direction signed distance and perform spatial optimizations.</param>
		/// <returns></returns>
		template <bool parallel = true>
		constexpr utils::matrix<T> render(const utils::math::transform2& camera_transform, const utils::math::vec2s& resolution, sample_dsdf_callback sample_dsdf_callback, float supersampling = 1.f)
			{
			utils::matrix<T> ret(resolution);

			const auto callback{[&, this](size_t index)
				{
				const utils::math::vec2s coords_indices{resolution.index_to_coords(index)};
				const utils::math::vec2f coords_f
					{
					utils::math::vec2f
						{
						static_cast<float>(coords_indices.x()),
						static_cast<float>(coords_indices.y())
						}
					.transform(camera_transform)
					.scale    (1.f / supersampling)
					};

				const utils::math::geometry::sdf::direction_signed_distance direction_signed_distance{sample_dsdf_callback(coords_f)};

				T& pixel{ret[index]};
				pixel = sample(coords_f, direction_signed_distance);
				}};

			std::ranges::iota_view indices(size_t{0}, resolution.sizes_to_size());
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

		utils_gpu_available constexpr virtual utils::graphics::colour::rgba_f sample(const utils::math::vec2f& coords, const utils::math::geometry::sdf::direction_signed_distance& direction_signed_distance) const noexcept final override
			{
			const auto grad {direction_signed_distance.direction};
			const auto side {direction_signed_distance.distance.side()};
			auto sdist{direction_signed_distance.distance.value};

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

	template <utils::math::geometry::shape::concepts::shape shape_t>
	class shape_bounding_box_wrapper
		{
		public:
			utils_gpu_available constexpr shape_bounding_box_wrapper(const shape_t& shape, utils::math::geometry::shape::aabb shape_padding = {-1.f, -1.f, 0.f, 0.f}) :
				shape_ptr{std::addressof(shape)},
				bounding_box{shape.bounding_box() + shape_padding}
				{}

			utils_gpu_available constexpr utils::math::geometry::sdf::direction_signed_distance evaluate_direction_signed_distance(const utils::math::vec2f& coords_f) const noexcept
				{
				if (bounding_box.contains(coords_f))
					{
					const auto ret{shape_ptr->sdf(coords_f).direction_signed_distance()};
					return ret;
					}
				return {};
				}

			/// <summary> Populates a direction signed distance field with the direction distance field of an additional shape. </summary>
			template <bool parallel = true>
			constexpr utils::matrix<utils::math::geometry::sdf::direction_signed_distance>& evaluate_dsdf
				(
				const utils::math::transform2& camera_transform,
				const merge_callback& merge_callback,
				utils::matrix<utils::math::geometry::sdf::direction_signed_distance>& direction_signed_distance_field,
				float supersampling = 1.f
				) const noexcept
				{
				const auto pixels_region_max{direction_signed_distance_field.sizes()};
				const utils::math::rect<float> pixels_region_f{bounding_box.transform(camera_transform).scale(supersampling)};

				if (pixels_region_f.rr() <  0.f                   || pixels_region_f.dw() <  0.f                  ) { return direction_signed_distance_field; }
				if (pixels_region_f.ll() >= pixels_region_max.x() || pixels_region_f.up() >= pixels_region_max.y()) { return direction_signed_distance_field; }

				const utils::math::rect<size_t> pixels_region_cast
					{
					utils::math::rect<size_t>
						{
								 utils::math::cast_clamp<size_t>(std::floor(pixels_region_f.ll())),
								 utils::math::cast_clamp<size_t>(std::floor(pixels_region_f.up())),
						std::min(utils::math::cast_clamp<size_t>(std::ceil (pixels_region_f.rr())), direction_signed_distance_field.sizes().x()),
						std::min(utils::math::cast_clamp<size_t>(std::ceil (pixels_region_f.dw())), direction_signed_distance_field.sizes().y())
						}
					};
				const utils::math::rect<size_t> pixels_region
					{
					utils::math::rect<size_t>
						{
								 pixels_region_cast.ll(),
								 pixels_region_cast.up(),
						std::min(pixels_region_cast.rr(), pixels_region_max.x()),
						std::min(pixels_region_cast.dw(), pixels_region_max.y())
						}
					};
				const size_t indices_end{pixels_region.size().sizes_to_size()};

				std::ranges::iota_view indices(size_t{0}, indices_end);
				const auto callback{[&, this](size_t index)
					{
					const utils::math::vec2s coords_indices{pixels_region.ul() + pixels_region.size().index_to_coords(index)};
					const utils::math::vec2f coords_f
						{
						utils::math::vec2f
							{
							static_cast<float>(coords_indices.x()),
							static_cast<float>(coords_indices.y())
							}
						.transform(camera_transform)
						.scale(1.f / supersampling)
						};

					utils::math::geometry::sdf::direction_signed_distance& value_at_pixel{direction_signed_distance_field[coords_indices]};

					const utils::math::geometry::sdf::direction_signed_distance shape_direction_signed_distance{shape_ptr->sdf(coords_f).direction_signed_distance()};
					value_at_pixel = merge_callback(value_at_pixel, shape_direction_signed_distance);
					}};
		
				if constexpr (parallel)
					{
					std::for_each(std::execution::par, indices.begin(), indices.end(), callback);
					}
				else if constexpr (!parallel)
					{
					std::for_each(indices.begin(), indices.end(), callback);
					}

				return direction_signed_distance_field;
				}

			utils::math::geometry::shape::aabb get_bounding_box() const noexcept { return bounding_box; }
			shape_t get_shape() const noexcept { return *shape_ptr; }
		private:
			const shape_t* shape_ptr;
			const utils::math::geometry::shape::aabb bounding_box;
		};
	


	inline utils::graphics::colour::rgba_f debug_sample_direction_sdf_scaled(utils::math::geometry::sdf::direction_signed_distance gdist, float distance_scale = 1.f)
		{
		if (gdist.distance.value ==  utils::math::constants::finf) { return {1.f, 1.f, 1.f, 0.f}; }
		if (gdist.distance.value == -utils::math::constants::finf) { return {0.f, 0.f, 0.f, 0.f}; }

		const auto smoothstep{[](float edge0, float edge1, float x) -> float
			{
			// Scale, bias and saturate x to 0..1 range
			x = std::clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
			// Evaluate polynomial
			return x * x * (3.f - 2.f * x);
			}};

		// Inigo Quilez inspired fancy colors
		gdist.distance.value *= distance_scale;
		utils::math::vec3f colour{utils::math::vec3f{gdist.direction.x() * .5f + .5f, gdist.direction.y() * .5f + .5f, gdist.distance.side().is_inside() ? 1.f : 0.f}};
		colour *= 1.0f - 0.5f * std::exp(-16.0f * gdist.distance.absolute());
		colour *= 0.9f + 0.1f * std::cos(150.0f * gdist.distance.value);
		colour = utils::math::lerp(colour, utils::math::vec3f{1.f}, 1.f - smoothstep(0.f, .01f, gdist.distance.absolute()));

		return utils::graphics::colour::rgba_f
			{
			colour[0],
			colour[1],
			colour[2],
			1.f
			};
		}

	inline utils::graphics::colour::rgba_f debug_sample_direction_sdf(utils::math::geometry::sdf::direction_signed_distance gdist)
		{
		return debug_sample_direction_sdf_scaled(gdist, .006f);
		}
	}