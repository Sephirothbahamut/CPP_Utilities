#pragma once

#include "../common.h"
#include "../../shape/bezier.h"
#include "../../../vec.h"

namespace utils::math::geometry::sdf::details::bezier::other
	{
	template <ends::ab ends>
	utils_gpu_available constexpr float closest_t(const utils::math::vec2f& point, const shape::concepts::bezier auto& shape, float t_step) noexcept
		{
		float found_t;
		float min_distance2{utils::math::constants::finf};

		if (true)
			{
			const auto candidate{shape.vertices[0]};
			const auto distance2{utils::math::vec2f::distance2(candidate, point)};
			if (distance2 < min_distance2)
				{
				found_t = 0.f;
				min_distance2 = distance2;
				}
			}
		for (float t{t_step}; t < (1.f - t_step); t += t_step)
			{
			const auto candidate{shape.at(t).point()};
			const auto distance2{utils::math::vec2f::distance2(candidate, point)};
			if (distance2 < min_distance2)
				{
				found_t = t;
				min_distance2 = distance2;
				}
			}
		if (true)
			{
			const auto candidate{shape.vertices[shape.vertices.size() - 1]};
			const auto distance2{utils::math::vec2f::distance2(candidate, point)};
			if (distance2 < min_distance2)
				{
				found_t = 1.f;
				min_distance2 = distance2;
				}
			}

		return found_t;
		}

	}