#pragma once

#include <cmath>
#include <optional>
#include <algorithm>
#include <execution>

#include "../oop/conditional_inheritance.h"

#include "../flags.h"
#include "../math/vec.h"
#include "../math/rect.h"
#include "../math/geometry/transform/aabb.h"
#include "../math/math.h"
#include "../logging/progress_bar.h"

namespace utils::algorithm::for_each
	{
	namespace details
		{
		inline utils::math::rect<size_t> clamp_region(const utils::math::vec2s& resolution, const utils::math::rect<size_t>& unclamped_region) noexcept
			{
			const utils::math::rect<size_t> ret
				{
				std::min(unclamped_region.ll(), resolution.x()),
				std::min(unclamped_region.up(), resolution.y()),
				std::min(unclamped_region.rr(), resolution.x()),
				std::min(unclamped_region.dw(), resolution.y())
				};
			return ret;
			}

		inline utils::math::vec2f coords_indices_to_scaled(const utils::math::vec2s& coords_indices, const utils::math::vec2f& floating_per_index) noexcept
			{
			const utils::math::vec2f coords_pixel_center{static_cast<utils::math::vec2f>(coords_indices) + .5f};
			const utils::math::vec2f coords_scaled{coords_pixel_center / floating_per_index};
			return coords_scaled;
			}

		struct coords_t
			{
			utils::math::vec2s indices;
			utils::math::vec2f floating;
			};


		

		struct in_region_scale_progress_t
			{
			logging::partial_progress& partial_progress;
			utils::math::vec2s sizes;
			utils::math::vec2f floating_per_index;
			utils::math::rect<size_t> region;

			struct callback_params
				{
				size_t index;
				coords_t global;
				coords_t region;
				};

			template <bool parallel = true>
			inline void execute(auto callback) const noexcept
				{
				const utils::math::vec2s region_resolution{region.size()};
				const auto indices_range{region_resolution.indices_range()};
				const auto indices_count{region_resolution.sizes_to_size()};
				logging::partial_progress inner_progress{partial_progress.split_partial_progress(indices_count)};

				const auto callback_with_params_wrapper{[&](size_t region_index)
					{
					const utils::math::vec2s in_region_indices{region_resolution.index_to_coords(region_index)};

					const utils::math::vec2s indices{region.ul() + in_region_indices};
					const size_t index{sizes.coords_to_index(indices)};

					const utils::math::vec2f floating          {coords_indices_to_scaled(indices          , floating_per_index)};
					const utils::math::vec2f in_region_floating{coords_indices_to_scaled(in_region_indices, floating_per_index)};

					const callback_params callback_params
						{
						.index{index},
						.global
							{
							.indices{indices},
							.floating{floating}
							},
						.region
							{
							.indices{in_region_indices},
							.floating{in_region_floating}
							}
						};
					callback(callback_params);

					inner_progress.advance();
					}};

				if constexpr (parallel)
					{
					std::for_each(std::execution::par_unseq, indices_range.begin(), indices_range.end(), callback_with_params_wrapper);
					}
				else if constexpr (!parallel)
					{
					std::for_each(std::execution::seq, indices_range.begin(), indices_range.end(), callback_with_params_wrapper);
					}
				}
			};

		struct in_sizes_scale_progress_t
			{
			logging::partial_progress& partial_progress;
			utils::math::vec2s sizes;
			utils::math::vec2f floating_per_index;

			struct callback_params
				{
				size_t index;
				coords_t global;
				};

			template <bool parallel = true>
			inline void execute(auto callback) const noexcept
				{
				const auto indices_range{sizes.indices_range()};
				const auto indices_count{sizes.sizes_to_size()};

				logging::partial_progress inner_progress{partial_progress.split_partial_progress(indices_count)};

				const auto callback_with_params_wrapper{[&](const size_t& index)
					{
					const utils::math::vec2s indices{sizes.index_to_coords(index)};
					const utils::math::vec2f floating{coords_indices_to_scaled(indices, floating_per_index)};

					const callback_params callback_params
						{
						.index{index},
						.global
							{
							.indices{indices},
							.floating{floating}
							}
						};
					callback(callback_params);

					inner_progress.advance();
					}};

				if constexpr (parallel)
					{
					std::for_each(std::execution::par_unseq, indices_range.begin(), indices_range.end(), callback_with_params_wrapper);
					}
				else if constexpr (!parallel)
					{
					std::for_each(std::execution::seq, indices_range.begin(), indices_range.end(), callback_with_params_wrapper);
					}
				}

			inline in_region_scale_progress_t region(const utils::math::rect<size_t>& value) const noexcept
				{
				const auto clamped_region{clamp_region(sizes, value)};
				return in_region_scale_progress_t{partial_progress, sizes, floating_per_index, clamped_region};
				}
			inline in_region_scale_progress_t scaled_region(const utils::math::rect<float>& value) const noexcept
				{
				const auto descaled_region{value.scale(floating_per_index)};
				const auto size_t_region{descaled_region.wrapping_round<size_t>()};
				const auto clamped_region{clamp_region(sizes, size_t_region)};
				return in_region_scale_progress_t{partial_progress, sizes, floating_per_index, clamped_region};
				}
			};
	
		struct in_region_scale_t
			{
			utils::math::vec2s sizes;
			utils::math::vec2f floating_per_index;
			utils::math::rect<size_t> region;

			inline in_region_scale_progress_t partial_progress(utils::logging::partial_progress& value) const noexcept
				{
				return in_region_scale_progress_t{value, sizes, floating_per_index, region};
				}

			template <bool parallel = true>
			inline void execute(auto callback) const noexcept
				{
				utils::logging::progress_bar progress_bar{.01f, 50};
				utils::logging::partial_progress _partial_progress{progress_bar.partial_progress(1)};
				partial_progress(_partial_progress).execute<parallel>(callback);
				}

			using callback_params = in_region_scale_progress_t::callback_params;
			};

		struct in_sizes_scale_t
			{
			utils::math::vec2s sizes;
			utils::math::vec2f floating_per_index;

			inline in_sizes_scale_progress_t partial_progress(utils::logging::partial_progress& value) const noexcept
				{
				return in_sizes_scale_progress_t{value, sizes, floating_per_index};
				}

			inline in_region_scale_t region(const utils::math::rect<size_t>& value) const noexcept
				{
				const auto clamped_region{clamp_region(sizes, value)};
				return in_region_scale_t{sizes, floating_per_index, clamped_region};
				}
			inline in_region_scale_t scaled_region(const utils::math::rect<float>& value) const noexcept
				{
				const auto descaled_region{value.scale(floating_per_index)};
				const auto size_t_region{descaled_region.wrapping_round<size_t>()};
				const auto clamped_region{clamp_region(sizes, size_t_region)};
				return in_region_scale_t{sizes, floating_per_index, clamped_region};
				}

			template <bool parallel = true>
			inline void execute(auto callback) const noexcept
				{
				utils::logging::progress_bar progress_bar{.01f, 50};
				utils::logging::partial_progress _partial_progress{progress_bar.partial_progress(1)};
				partial_progress(_partial_progress).execute<parallel>(callback);
				}

			using callback_params = in_sizes_scale_progress_t::callback_params;
			};
		
		struct in_region_t
			{
			utils::math::vec2s sizes;
			utils::math::rect<size_t> region;

			inline in_region_scale_t scale(const utils::math::vec2f& value) const noexcept
				{
				return in_region_scale_t{sizes, value, region};
				}

			template <bool parallel = true>
			inline void execute(auto callback) const noexcept
				{
				scale({1.f, 1.f}).execute<parallel>(callback);
				}

			inline in_region_scale_progress_t partial_progress(utils::logging::partial_progress& value) const noexcept
				{
				return scale({1.f, 1.f}).partial_progress(value);
				}

			using callback_params = in_region_scale_t::callback_params;
			};

		struct in_sizes_t
			{
			utils::math::vec2s sizes;
			inline in_sizes_scale_t scale(const utils::math::vec2f& value) const noexcept
				{
				return in_sizes_scale_t{sizes, value};
				}

			inline in_region_t region(const utils::math::rect<size_t>& value) const noexcept
				{
				const auto clamped_region{clamp_region(sizes, value)};
				return in_region_t{sizes, clamped_region};
				}

			template <bool parallel = true>
			inline void execute(auto callback) const noexcept
				{
				scale({1.f, 1.f}).execute<parallel>(callback);
				}

			inline in_sizes_scale_progress_t partial_progress(utils::logging::partial_progress& value) const noexcept
				{
				return scale({1.f, 1.f}).partial_progress(value);
				}

			using callback_params = in_sizes_scale_t::callback_params;
			};
		}

	inline details::in_sizes_t in_sizes(const utils::math::vec2s& sizes) noexcept
		{
		return details::in_sizes_t(sizes);
		}
	};