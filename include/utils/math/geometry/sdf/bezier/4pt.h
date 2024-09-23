#pragma once

#include "../return_types.h"
#include "../../shape/bezier.h"

namespace utils::math::geometry::sdf::details::bezier::_4pt
	{
	template<storage::type storage_type, geometry::ends::optional_ab optional_ends>
	struct sdf_proxy
		{
		using shape_t = geometry::shape::generic::bezier<storage_type, 4, optional_ends>;
		sdf_proxy(const shape_t& shape, const vec2f& point) : shape{shape}, point{point} {};
		const shape_t& shape;
		const vec2f point;

		template <ends::ab ends>
		utils_gpu_available constexpr float closest_t() const noexcept
			{
			inline static constexpr float bezier_stepping_t{0.05f};

			float found_t;
			float min_distance2{utils::math::constants::finf};

			if (true)
				{
				const auto candidate{bezier.vertices[0]};
				const auto distance2{utils::math::vec2f::distance2(candidate, point)};
				if (distance2 < min_distance2)
					{
					found_t = 0.f;
					min_distance2 = distance2;
					}
				}
			for (float t{bezier_stepping_t}; t < (1.f - bezier_stepping_t); t += bezier_stepping_t)
				{
				const auto candidate{bezier.at(t).point()};
				const auto distance2{utils::math::vec2f::distance2(candidate, point)};
				if (distance2 < min_distance2)
					{
					found_t = t;
					min_distance2 = distance2;
					}
				}
			if (true)
				{
				const auto candidate{bezier.vertices[3]};
				const auto distance2{utils::math::vec2f::distance2(candidate, point)};
				if (distance2 < min_distance2)
					{
					found_t = 1.f;
					min_distance2 = distance2;
					}
				}

			return found_t;
			}
		};
	}