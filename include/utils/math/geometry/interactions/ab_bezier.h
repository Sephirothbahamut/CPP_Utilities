#pragma once

#include "common.h"

#include "../details/base_types.h"
#include "../shape/point.h"
#include "../shape/bezier.h"
#include "../shape/ab.h"


namespace utils::math::geometry::details
	{
	template <shape::concepts::bezier a_t, shape::concepts::ab b_t>
	struct interactions_bezier_ab
		{
		a_t& a;
		b_t& b;

		std::pair<float, float> intersection_ts_approximate_first(float t_step = .01f) const noexcept
			{
			for (float ta{0.f}; ta < 1.f; ta += t_step)
				{
				const shape::segment sa{a.at(ta).point(), a.at(ta + t_step).point()};
				
				const std::pair<float, float> intersection_ts{interactions(sa, b).intersection_ts()};
				if (!std::isnan(intersection_ts.first) && !std::isnan(intersection_ts.second))
					{
					const float curve_ta{ta + (t_step * intersection_ts.first)};
					return {curve_ta, intersection_ts.second};
					}
				}
			return {utils::math::constants::fnan, utils::math::constants::fnan};
			}
		};

	template <shape::concepts::ab a_t, shape::concepts::bezier b_t>
	struct interactions_ab_bezier
		{
		a_t& a;
		b_t& b;

		std::pair<float, float> intersection_ts_approximate_first(float t_step = .01f) const noexcept
			{
			for (float tb{1.f - t_step}; tb > t_step; tb -= t_step)
				{
				const shape::segment sb{b.at(tb).point(), b.at(tb + t_step).point()};

				const std::pair<float, float> intersection_ts{interactions(a, sb).intersection_ts()};
				if (!std::isnan(intersection_ts.first) && !std::isnan(intersection_ts.second))
					{
					const float curve_tb{tb + (t_step * intersection_ts.second)};
					return {intersection_ts.first, curve_tb};
					}
				}

			return {utils::math::constants::fnan, utils::math::constants::fnan};
			}
		};
	}

namespace utils::math::geometry
	{
	template <shape::concepts::bezier a_t, shape::concepts::ab b_t>
	utils_gpu_available inline static constexpr auto interactions(const a_t& a, const b_t& b) noexcept
		{
		return details::interactions_bezier_ab{a, b};
		}
	template <shape::concepts::ab a_t, shape::concepts::bezier b_t>
	utils_gpu_available inline static constexpr auto interactions(const a_t& a, const b_t& b) noexcept
		{
		return details::interactions_ab_bezier{a, b};
		}
	}