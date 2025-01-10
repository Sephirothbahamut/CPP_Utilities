#pragma once

#include "common.h"

#include "../details/base_types.h"
#include "../shape/point.h"
#include "../shape/bezier.h"

namespace utils::math::geometry::details
	{
	template <shape::concepts::ab a_t, shape::concepts::ab b_t>
	struct interactions_ab_ab
		{
		a_t& a;
		b_t& b;

		template <ends::ab ends_a, ends::ab ends_b>
		utils_gpu_available constexpr std::pair<float, float> intersection_ts() const noexcept
			{//https://stackoverflow.com/questions/563198/how-do-you-detect-where-two-line-segments-intersect

			const utils::math::vec2f s1
				{
				a.b.x() - a.a.x(),
				a.b.y() - a.a.y()
				};
			const utils::math::vec2f s2
				{
				b.b.x() - b.a.x(),
				b.b.y() - b.a.y()
				};

			const float denominator{-s2.x() * s1.y() + s1.x() * s2.y()};
			if (utils::math::almost_equal(denominator, 0.f)) 
				{
				//Colinear, could be coincident or never touching
				return {utils::math::constants::fnan, utils::math::constants::fnan};
				}

			const float s{(-s1.y() * (a.a.x() - b.a.x()) + s1.x() * (a.a.y() - b.a.y())) / denominator};
			const float t{( s2.x() * (a.a.y() - b.a.y()) - s2.y() * (a.a.x() - b.a.x())) / denominator};

			return {ends::t_or<ends_a>(s, utils::math::constants::fnan), ends::t_or<ends_b>(t, utils::math::constants::fnan)};
			}

		utils_gpu_available constexpr std::pair<float, float> intersection_ts() const noexcept
			requires (shape::concepts::ab_ends_aware<a_t>, shape::concepts::ab_ends_aware<b_t>)
			{
			return intersection_ts<a.optional_ends.value(), b.optional_ends.value()>();
			}

		template <ends::ab ends_a, ends::ab ends_b>
		utils_gpu_available constexpr float intersection_t() const noexcept
			{
			const float ret{intersection_ts<a.optional_ends.value(), b.optional_ends.value()>().first};
			return ret;
			}

		utils_gpu_available constexpr float intersection_t() const noexcept
			requires (shape::concepts::ab_ends_aware<a_t>, shape::concepts::ab_ends_aware<b_t>)
			{
			return intersection_t<a.optional_ends.value(), b.optional_ends.value()>();
			}

		template <ends::ab ends_a, ends::ab ends_b>
		utils_gpu_available constexpr std::optional<shape::point> intersection() const noexcept
			{
			const float t{intersection_t<ends_a, ends_b>()};
			if (t != utils::math::constants::fnan)
				{
				const shape::point ret{a.at(t).point()};
				return ret;
				}
			return std::nullopt;
			}

		utils_gpu_available constexpr std::optional<shape::point> intersection() const noexcept
			requires (shape::concepts::ab_ends_aware<a_t>, shape::concepts::ab_ends_aware<b_t>)
			{
			return intersection<a.optional_ends.value(), b.optional_ends.value()>();
			}
		};
	}

namespace utils::math::geometry
	{
	template <shape::concepts::ab a_t, shape::concepts::ab b_t>
	utils_gpu_available inline static constexpr auto interactions(const a_t& a, const b_t& b) noexcept
		{
		return details::interactions_ab_ab{a, b};
		}
	}