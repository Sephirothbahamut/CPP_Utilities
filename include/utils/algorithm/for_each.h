#pragma once

#include <cmath>
#include <algorithm>
#include <execution>

#include "../math/vec.h"
#include "../math/rect.h"
#include "../math/math.h"
#include "../logging/progress_bar.h"


namespace utils::algorithm::for_each
	{
	namespace details
		{
		struct partial_progress
			{
			utils::logging::progress_bar& progress_bar;
			float divide_progress_by;
			float base_progress;
			};
		}

	struct coords_callback_params
		{
		size_t index;
		utils::math::vec2s indices;
		};
	
	template <bool parallel>
	void coords(utils::math::vec2s sizes, details::partial_progress partial_progress, auto callback)
		{
		const auto indices_range{sizes.indices_range()};

		std::atomic_size_t completed{0};
		const auto indices_count{sizes.sizes_to_size()};

		const auto callback_coords_wrapper{[&](const size_t& index)
			{
			const utils::math::vec2s indices{sizes.index_to_coords(index)};
			callback(coords_callback_params{.index{index}, .indices{indices}});

			completed++;
			const float percent{completed / static_cast<float>(indices_count)};
			partial_progress.progress_bar.advance(partial_progress.base_progress + (percent / partial_progress.divide_progress_by));
			}};
	
		if constexpr (parallel)
			{
			std::for_each(std::execution::par_unseq, indices_range.begin(), indices_range.end(), callback_coords_wrapper);
			}
		else if constexpr (!parallel)
			{
			std::for_each(std::execution::seq, indices_range.begin(), indices_range.end(), callback_coords_wrapper);
			}
		};

	template <bool parallel>
	void coords(utils::math::vec2s sizes, auto callback)
		{
		utils::logging::progress_bar progress_bar{.01f, 50};
		coords<parallel>(sizes, details::partial_progress{progress_bar, 1.f, 0.f}, callback);
		progress_bar.complete();
		};

	struct coords_in_region_callback_params
		{
		size_t index;
		utils::math::vec2s indices;
		utils::math::vec2s in_region_indices;
		};
	
	inline utils::math::rect<size_t> clamp_region(const utils::math::vec2s& resolution, const utils::math::rect<size_t>& unclamped_region)
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

	template <bool parallel>
	void coords_in_region(utils::math::vec2s sizes, utils::math::rect<size_t> region, details::partial_progress partial_progress, auto callback)
		{
		const auto clamped_region{clamp_region(sizes, region)};
		const auto region_resolution{clamped_region.size()};
		const auto indices_range{region_resolution.indices_range()};
		if (region_resolution.x() == 0 || region_resolution.y() == 0) { return; }

		std::atomic_size_t completed{0};
		const auto indices_count{region_resolution.sizes_to_size()};

		const auto callback_coords_wrapper{[&](size_t region_index)
			{
			const utils::math::vec2s in_region_coords_indices{region_resolution.index_to_coords(region_index)};

			const utils::math::vec2s coords_indices{clamped_region.ul() + in_region_coords_indices};
			const size_t index{sizes.coords_to_index(coords_indices)};

			const coords_in_region_callback_params callback_params
				{
				.index{index}, 
				.indices{coords_indices},
				.in_region_indices{in_region_coords_indices}
				};
			callback(callback_params);

			completed++;
			const float percent{completed / static_cast<float>(indices_count)};
			partial_progress.progress_bar.advance(partial_progress.base_progress + (percent / partial_progress.divide_progress_by));
			}};

		if constexpr (parallel)
			{
			std::for_each(std::execution::par_unseq, indices_range.begin(), indices_range.end(), callback_coords_wrapper);
			}
		else if constexpr (!parallel)
			{
			std::for_each(std::execution::seq, indices_range.begin(), indices_range.end(), callback_coords_wrapper);
			}
		}

	template <bool parallel>
	void coords_in_region(utils::math::vec2s sizes, utils::math::rect<size_t> region, auto callback)
		{
		utils::logging::progress_bar progress_bar{.01f, 50};

		coords_in_region<parallel>(sizes, region, details::partial_progress{progress_bar, 1.f, 0.f}, callback);
		progress_bar.complete();
		}
	}
