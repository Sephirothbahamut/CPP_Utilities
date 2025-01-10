#pragma once

#include "common.h"

#include "../details/base_types.h"
#include "../shape/point.h"
#include "../shape/bezier.h"

#include "ab_ab.h"

namespace utils::math::geometry::details
	{
	template <shape::concepts::bezier a_t, shape::concepts::bezier b_t>
	struct interactions_bezier_bezier
		{
		a_t& a;
		b_t& b;

		std::pair<float, float> intersection_ts_approximate_first(float t_step = .01f) const noexcept
			{
			for (float ta{t_step}; ta < 1.f - t_step; ta += t_step)
				{
				const shape::segment sa{a.at(ta).point(), a.at(ta + t_step).point()};
				for (float tb{1.f - t_step}; tb > t_step; tb -= t_step)
					{
					const shape::segment sb{b.at(tb).point(), b.at(tb + t_step).point()};

					const std::pair<float, float> intersection_ts{interactions(sa, sb).intersection_ts()};
					if (!std::isnan(intersection_ts.first) && !std::isnan(intersection_ts.second))
						{
						const float curve_ta{ta + (t_step * intersection_ts.first )};
						const float curve_tb{tb + (t_step * intersection_ts.second)};
						return {curve_ta, curve_tb};
						}
					}
				}
			return {utils::math::constants::fnan, utils::math::constants::fnan};
			}
		//std::pair<float, float> intersection_ts_approximate_last(float t_step = .01f) const noexcept
		//	{
		//	}
		//shape::point intersection_approximate_first(float t_step = .01f) const noexcept
		//	{
		//	return a.at(intersection_t_approximate_first(t_step)).point();
		//	}
		//shape::point intersection_approximate_last(float t_step = .01f) const noexcept
		//	{
		//	return a.at(intersection_t_approximate_last(t_step)).point();
		//	}
		};
	}



namespace utils::math::geometry
	{
	template <shape::concepts::bezier a_t, shape::concepts::bezier b_t>
	utils_gpu_available inline static constexpr auto interactions(const a_t& a, const b_t& b) noexcept
		{
		return details::interactions_bezier_bezier{a, b};
		}
	}