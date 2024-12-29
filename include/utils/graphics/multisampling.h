#pragma once

#include <ranges>
#include <concepts>
#include <algorithm>

#include "../math/vec.h"

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

			sample_t sum{callback(coordinates_f)};

			const auto inner_sample{[&](size_t x, size_t y) noexcept
				{
				const utils::math::vec2f offset{x * per_sample_delta, y * per_sample_delta};
				const utils::math::vec2f sample_coordinates{coordinates_f + offset};

				const auto sample{callback(sample_coordinates)};
				return sample;
				}};

			for (size_t x{1}; x < samples; x++)
				{
				sum += inner_sample(x, 0);
				}

			for (size_t y{1}; y < samples; y++)
				{
				for (size_t x{0}; x < samples; x++)
					{
					sum += inner_sample(x, 0);
					}
				}

			const auto ret{sum / static_cast<float>(samples * samples)};
			return ret;
			}
		else 
			{
			const sample_t ret{callback(coordinates_f)};
			return ret;
			}
		}
	}