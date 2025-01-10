#pragma once

#include "common.h"
#include "../shape/bezier.h"

namespace utils::math::geometry::shape::generic
	{
	template<storage::type storage_type, size_t extent, geometry::ends::optional_ab optional_ends>
	utils_gpu_available constexpr auto bezier<storage_type, extent, optional_ends>::bounding_box() const noexcept
		{
		assert(vertices.size() == 3 || vertices.size() == 4);
		//https://iquilezles.org/articles/bezierbbox/
		
		// extremes
		utils::math::vec2f min{utils::math::min(vertices[0], vertices[vertices.size() - 1])};
		utils::math::vec2f max{utils::math::max(vertices[0], vertices[vertices.size() - 1])};

		if (vertices.size() == 3)
			{
			if (vertices[1].x() < min.x() || vertices[1].x() > max.x() || vertices[1].y() < min.y() || vertices[1].y() > max.y())
				{
				const vec2 t{clamp((vertices[0] - vertices[1]) / (vertices[0] - (vertices[1] * 2.f) + vertices[2]), 0.f, 1.f)};
				const vec2 s{-t + 1.f};
				const vec2 q{s * s * vertices[0] + (s * t * vertices[1] * 2.f) + t * t * vertices[2]};
				min = utils::math::min(min, q);
				max = utils::math::max(max, q);
				}
			}
		else if (vertices.size() == 4)
			{
			const utils::math::vec2f k0{-(vertices[0] * 1.f) + (vertices[1] * 1.f)};
			const utils::math::vec2f k1{ (vertices[0] * 1.f) - (vertices[1] * 2.f) + (vertices[2] * 1.f)};
			const utils::math::vec2f k2{-(vertices[0] * 1.f) + (vertices[1] * 3.f) - (vertices[2] * 3.f) + (vertices[3] * 1.f)};
			
			utils::math::vec2f h{k1 * k1 - k0 * k2};

			if (h.x() > 0.f)
				{
				h.x() = sqrt(h.x());
				//float t = ( - k1.x() - h.x()) / k2.x();
				float t{k0.x() / (-k1.x() - h.x())};
				if (t > 0.f && t < 1.f)
					{
					const float s{1.f - t};
					const float q{s * s * s * vertices[0].x() + 3.f * s * s * t * vertices[1].x() + 3.f * s * t * t * vertices[2].x() + t * t * t * vertices[3].x()};
					min.x() = utils::math::min(min.x(), q);
					max.x() = utils::math::max(max.x(), q);
					}
					//t = ( - k1.x() + h.x()) / k2.x();
				t = k0.x() / (-k1.x() + h.x());
				if (t > 0.f && t < 1.f)
					{
					const float s{1.f - t};
					const float q{s * s * s * vertices[0].x() + 3.f * s * s * t * vertices[1].x() + 3.f * s * t * t * vertices[2].x() + t * t * t * vertices[3].x()};
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
					float q = s * s * s * vertices[0].y() + 3.f * s * s * t * vertices[1].y() + 3.f * s * t * t * vertices[2].y() + t * t * t * vertices[3].y();
					min.y() = utils::math::min(min.y(), q);
					max.y() = utils::math::max(max.y(), q);
					}
					//t = ( - k1.y() + h.y()) / k2.y();
				t = k0.y() / (-k1.y() + h.y());
				if (t > 0.f && t < 1.f)
					{
					float s = 1.f - t;
					float q = s * s * s * vertices[0].y() + 3.f * s * s * t * vertices[1].y() + 3.f * s * t * t * vertices[2].y() + t * t * t * vertices[3].y();
					min.y() = utils::math::min(min.y(), q);
					max.y() = utils::math::max(max.y(), q);
					}
				}
			}

		const auto ret{shape::aabb::create::from_ul_dr(min, max)};
		return ret;
		}
	}