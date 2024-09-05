#pragma once

#include <ranges>
#include <concepts>
#include <algorithm>

#include "../math/vec2.h"
#include "../math/vec3.h"
#include "../math/vec_s.h"

namespace utils::graphics
	{
	namespace concepts
		{
		template <typename T>
		concept sample = requires(T t)
			{
					{ t / 0.f } -> std::same_as<T>;
					//{ t + t   } -> std::same_as<T>;
					{ t += t  } -> std::same_as<T&>;
			};
		}
	
	template <concepts::sample sample_t, size_t samples>
	sample_t multisample(utils::math::vec2f coordinates_f, auto callback)
		{
		if constexpr (samples > 1)
			{
			constexpr float per_sample_delta{1.f / static_cast<float>(samples - 1)};
			//*
			sample_t sum;
			
			for (size_t y{0}; y < samples; y++)
				{
				for (size_t x{0}; x < samples; x++)
					{
					const utils::math::vec2f offset{x * per_sample_delta, y * per_sample_delta};
					const utils::math::vec2f sample_coordinates{coordinates_f + offset};
					
					const auto sample{callback(sample_coordinates)};
					sum += sample;
					}
				}
			/*/
			std::mutex sample_mutex;
			sample_t sum;

			std::ranges::iota_view indices(size_t{0}, samples * samples);
			std::for_each(std::execution::par, indices.begin(), indices.end(), [&](size_t index)
				{
				size_t x{index / samples};
				size_t y{index % samples};

				const utils::math::vec2f offset{x * per_sample_delta, y * per_sample_delta};
				const utils::math::vec2f sample_coordinates{coordinates_f + offset};

				const auto sample{callback(sample_coordinates)};
				std::unique_lock lock{sample_mutex};
				sum += sample;
				});
			/**/

			return sum / static_cast<float>(samples * samples);
			}
		else 
			{
			sample_t ret{callback(coordinates_f)};
			return ret;
			}
		}
	}