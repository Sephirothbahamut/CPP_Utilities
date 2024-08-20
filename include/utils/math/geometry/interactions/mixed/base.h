#pragma once

#include "../base_types.h"
#include "../../shape/mixed.h"

namespace utils::math::geometry::interactions
	{
	utils_gpu_available constexpr return_types::bounding_box bounding_box(const shape::concepts::mixed auto& mixed) noexcept
		{
		const auto pieces{mixed.get_pieces()};
		auto ret{return_types::bounding_box::create::infinite()};

		pieces.for_each([&ret](const auto& piece)
			{
			const auto piece_bounding_box{interactions::bounding_box(piece)};
			ret = return_types::bounding_box::create::bounding_rect(ret, piece_bounding_box);
			});

		return ret;
		}
	}