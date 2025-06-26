#pragma once

#include "../math/vec.h"
#include "../math/rect.h"

namespace utils::graphics::dpi_conversions
	{
	namespace multipliers
		{
		utils_gpu_available inline constexpr float in_to_mm  () noexcept { return 25.4f; }
		utils_gpu_available inline constexpr float mm_to_in  () noexcept { return 1.f / 25.4f; }

		utils_gpu_available inline constexpr utils::math::vec2f px_to_in(const utils::math::vec2f& dpi) noexcept { return 1.f / dpi; }
		utils_gpu_available inline constexpr utils::math::vec2f in_to_px(const utils::math::vec2f& dpi) noexcept { return dpi / 1.f; }
		 
		utils_gpu_available inline constexpr utils::math::vec2f mm_to_px(const utils::math::vec2f& dpi) noexcept { return in_to_px(dpi) * mm_to_in(); }
		utils_gpu_available inline constexpr utils::math::vec2f px_to_mm(const utils::math::vec2f& dpi) noexcept { return px_to_in(dpi) * in_to_mm(); }
		}
	
	utils_gpu_available inline constexpr auto dpi_to_dpmm(const auto& value) noexcept { return value * multipliers::mm_to_in(); }
	utils_gpu_available inline constexpr auto dpmm_to_dpi(const auto& value) noexcept { return value * multipliers::in_to_mm(); }

	utils_gpu_available inline constexpr utils::math::vec2f px_to_in(const auto& value, const utils::math::vec2f& dpi) noexcept { return value * multipliers::px_to_in(dpi); }
	utils_gpu_available inline constexpr utils::math::vec2f in_to_px(const auto& value, const utils::math::vec2f& dpi) noexcept { return value * multipliers::in_to_px(dpi); }
	
	utils_gpu_available inline constexpr auto mm_to_in(const auto& value) noexcept { return value * multipliers::mm_to_in(); }
	utils_gpu_available inline constexpr auto in_to_mm(const auto& value) noexcept { return value * multipliers::in_to_mm(); }

	utils_gpu_available inline constexpr utils::math::vec2f mm_to_px(const auto& value, const utils::math::vec2f& dpi) noexcept { return value * multipliers::mm_to_px(dpi); }
	utils_gpu_available inline constexpr utils::math::vec2f px_to_mm(const auto& value, const utils::math::vec2f& dpi) noexcept { return value * multipliers::px_to_mm(dpi); }

	utils_gpu_available inline constexpr utils::math::rect<float> px_to_mm(const utils::math::rect<float>& rect, const utils::math::vec2f& dpi) noexcept
		{
		const auto multiplier{utils::graphics::dpi_conversions::multipliers::px_to_mm(dpi)};
		return utils::math::rect<float>
			{
			rect.ll() * multiplier.x(),
			rect.up() * multiplier.y(),
			rect.rr() * multiplier.x(),
			rect.dw() * multiplier.y()
			};
		}
	
	utils_gpu_available inline constexpr utils::math::rect<float> mm_to_px(const utils::math::rect<float>& rect, const utils::math::vec2f& dpi) noexcept
		{
		const auto multiplier{utils::graphics::dpi_conversions::multipliers::mm_to_px(dpi)};
		return utils::math::rect<float>
			{
			rect.ll() * multiplier.x(),
			rect.up() * multiplier.y(),
			rect.rr() * multiplier.x(),
			rect.dw() * multiplier.y()
			};
		}
	}