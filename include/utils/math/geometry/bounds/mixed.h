#pragma once

#include "common.h"
#include "../shape/mixed.h"
#include "ab.h"
#include "bezier.h"

namespace utils::math::geometry::shape::generic
	{
	template <storage::type storage_type, geometry::ends::closeable ends>
	utils_gpu_available constexpr auto mixed<storage_type, ends>::bounding_box() const noexcept
		{
		const auto pieces{get_pieces()};
		auto ret{shape::aabb::create::inverse_infinite()};

		pieces.for_each([&ret](const auto& piece)
			{
			const auto piece_bounding_box{piece.bounding_box()};
			ret.merge_self(piece_bounding_box);
			});

		return ret;
		}
	}