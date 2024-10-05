#pragma once

#include "common.h"
#include "../bezier.h"

namespace utils::math::geometry::shape::generic
	{
	template<storage::type storage_type, size_t extent, geometry::ends::optional_ab optional_ends>
	utils_gpu_available constexpr auto bezier<storage_type, extent, optional_ends>::bounding_box() const noexcept
		{
		assert(bezier.vertices.size() == 3 || bezier.vertices.size() == 4);
		//https://iquilezles.org/articles/bezierbbox/
		
		// extremes
		utils::math::vec2f min{utils::math::min(bezier.vertices[0], bezier.vertices[bezier.vertices.size() - 1])};
		utils::math::vec2f max{utils::math::max(bezier.vertices[0], bezier.vertices[bezier.vertices.size() - 1])};

		if (bezier.vertices.size() == 3)
			{
			if (bezier.vertices[1].x() < min.x() || bezier.vertices[1].x() > max.x() || bezier.vertices[1].y() < min.y() || bezier.vertices[1].y() > max.y())
				{
				const vec2 t{clamp((bezier.vertices[0] - bezier.vertices[1]) / (bezier.vertices[0] - (bezier.vertices[1] * 2.f) + bezier.vertices[2]), 0.f, 1.f)};
				const vec2 s{-t + 1.f};
				const vec2 q{s * s * bezier.vertices[0] + (s * t * bezier.vertices[1] * 2.f) + t * t * bezier.vertices[2]};
				min = utils::math::min(min, q);
				max = utils::math::max(max, q);
				}
			}
		else if (bezier.vertices.size() == 4)
			{
			const utils::math::vec2f k0{-(bezier.vertices[0] * 1.f) + (bezier.vertices[1] * 1.f)};
			const utils::math::vec2f k1{ (bezier.vertices[0] * 1.f) - (bezier.vertices[1] * 2.f) + (bezier.vertices[2] * 1.f)};
			const utils::math::vec2f k2{-(bezier.vertices[0] * 1.f) + (bezier.vertices[1] * 3.f) - (bezier.vertices[2] * 3.f) + (bezier.vertices[3] * 1.f)};
			
			utils::math::vec2f h{k1 * k1 - k0 * k2};

			if (h.x() > 0.f)
				{
				h.x() = sqrt(h.x());
				//float t = ( - k1.x() - h.x()) / k2.x();
				float t{k0.x() / (-k1.x() - h.x())};
				if (t > 0.f && t < 1.f)
					{
					const float s{1.f - t};
					const float q{s * s * s * bezier.vertices[0].x() + 3.f * s * s * t * bezier.vertices[1].x() + 3.f * s * t * t * bezier.vertices[2].x() + t * t * t * bezier.vertices[3].x()};
					min.x() = utils::math::min(min.x(), q);
					max.x() = utils::math::max(max.x(), q);
					}
					//t = ( - k1.x() + h.x()) / k2.x();
				t = k0.x() / (-k1.x() + h.x());
				if (t > 0.f && t < 1.f)
					{
					const float s{1.f - t};
					const float q{s * s * s * bezier.vertices[0].x() + 3.f * s * s * t * bezier.vertices[1].x() + 3.f * s * t * t * bezier.vertices[2].x() + t * t * t * bezier.vertices[3].x()};
					min.x() = utils::math::min(min.x(), q);
					max.x() = utils::math::max(max.x(), q);
					}
				}

			if (h.y() > 0.f)
				{
				h.y() = sqrt(h.y());
				//float t = ( - k1.y() - h.y()) / k2.y();
				float t{k0.y() / (-k1.y() - h.y())};
				if (t > 0.f && t < 1.f)
					{
					float s = 1.f - t;
					float q = s * s * s * bezier.vertices[0].y() + 3.f * s * s * t * bezier.vertices[1].y() + 3.f * s * t * t * bezier.vertices[2].y() + t * t * t * bezier.vertices[3].y();
					min.y() = utils::math::min(min.y(), q);
					max.y() = utils::math::max(max.y(), q);
					}
					//t = ( - k1.y() + h.y()) / k2.y();
				t = k0.y() / (-k1.y() + h.y());
				if (t > 0.f && t < 1.f)
					{
					float s = 1.f - t;
					float q = s * s * s * bezier.vertices[0].y() + 3.f * s * s * t * bezier.vertices[1].y() + 3.f * s * t * t * bezier.vertices[2].y() + t * t * t * bezier.vertices[3].y();
					min.y() = utils::math::min(min.y(), q);
					max.y() = utils::math::max(max.y(), q);
					}
				}
			}

		return return_types::bounding_box::create::from_ul_dr(min, max);
		}
	}