#pragma once

#include "common.h"

#include "../details/base_types.h"
#include "../shape/point.h"
#include "../shape/mixed.h"
#include "../shape/ab.h"

#include "ab_ab.h"
#include "ab_bezier.h"

namespace utils::math::geometry::details
	{
	template <shape::concepts::mixed a_t, shape::concepts::ab b_t>
	struct interactions_mixed_ab
		{
		a_t& a;
		b_t& b;

		bool intersects(float t_step = .01f) const noexcept
			{
			return intersections_count(t_step) > 0;
			}

		size_t intersections_count(float t_step = .01f) const noexcept
			{
			size_t ret{0};

			a.get_pieces().for_each([this, &ret](const auto& piece, size_t first_index, size_t last_index)
				{
				using piece_t = std::remove_cvref_t<decltype(piece)>;
				if constexpr (shape::concepts::bezier<piece_t>)
					{
					ret += utils::math::geometry::interactions(b, piece).intersections_count(t_step);
					}
				else if constexpr (shape::concepts::ab<piece_t>)
					{
					if (utils::math::geometry::interactions(b, piece).intersects())
						{
						ret++;
						}
					}
				});
			return ret;
			}
		};

	template <shape::concepts::ab a_t, shape::concepts::mixed b_t>
	struct interactions_ab_mixed
		{
		a_t& a;
		b_t& b;

		bool intersects(float t_step = .01f) const noexcept
			{
			return intersections_count(t_step) > 0;
			}

		size_t intersections_count(float t_step = .01f) const noexcept
			{
			return interactions_mixed_ab{b, a}.intersections_count(t_step);
			}
		};
	}

namespace utils::math::geometry
	{
	template <shape::concepts::mixed a_t, shape::concepts::ab b_t>
	utils_gpu_available inline static constexpr auto interactions(const a_t& a, const b_t& b) noexcept
		{
		return details::interactions_mixed_ab{a, b};
		}
	template <shape::concepts::ab a_t, shape::concepts::mixed b_t>
	utils_gpu_available inline static constexpr auto interactions(const a_t& a, const b_t& b) noexcept
		{
		return details::interactions_ab_mixed{a, b};
		}
	}