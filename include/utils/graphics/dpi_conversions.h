#pragma once

#include "../math/vec.h"
#include "../math/rect.h"

namespace utils::graphics::dpi_conversions
	{
	template <typename T>
	concept float_vec_or_rect = std::same_as<T, float> || std::same_as<T, utils::math::vec2f> || std::same_as<T, utils::math::rect<float>>;

	template <typename T>
	concept dots_per_unit_cpt = std::same_as<T, float> || std::same_as<T, utils::math::vec2f>;

	namespace multipliers
		{
		utils_gpu_available inline constexpr float in_to_mm  () noexcept { return       25.4f; }
		utils_gpu_available inline constexpr float mm_to_in  () noexcept { return 1.f / 25.4f; }

		utils_gpu_available inline constexpr auto px_to_in(const dots_per_unit_cpt auto& dpi) noexcept { return 1.f / dpi; }
		utils_gpu_available inline constexpr auto in_to_px(const dots_per_unit_cpt auto& dpi) noexcept { return dpi / 1.f; }

		utils_gpu_available inline constexpr auto px_to_mm(const dots_per_unit_cpt auto& dpmm) noexcept { return 1.f  / dpmm; }
		utils_gpu_available inline constexpr auto mm_to_px(const dots_per_unit_cpt auto& dpmm) noexcept { return dpmm / 1.f ; }
		}
	
	utils_gpu_available inline constexpr auto dpi_to_dpmm(const dots_per_unit_cpt auto& value) noexcept { return value * multipliers::mm_to_in(); }
	utils_gpu_available inline constexpr auto dpmm_to_dpi(const dots_per_unit_cpt auto& value) noexcept { return value * multipliers::in_to_mm(); }

	utils_gpu_available inline constexpr auto px_to_in(const float_vec_or_rect auto& value, const dots_per_unit_cpt auto& dpi) noexcept { return value * multipliers::px_to_in(dpi); }
	utils_gpu_available inline constexpr auto in_to_px(const float_vec_or_rect auto& value, const dots_per_unit_cpt auto& dpi) noexcept { return value * multipliers::in_to_px(dpi); }
	
	utils_gpu_available inline constexpr auto mm_to_in(const float_vec_or_rect auto& value) noexcept { return value * multipliers::mm_to_in(); }
	utils_gpu_available inline constexpr auto in_to_mm(const float_vec_or_rect auto& value) noexcept { return value * multipliers::in_to_mm(); }

	utils_gpu_available inline constexpr auto mm_to_px(const float_vec_or_rect auto& value, const dots_per_unit_cpt auto& dpmm) noexcept { return value * multipliers::mm_to_px(dpmm); }
	utils_gpu_available inline constexpr auto px_to_mm(const float_vec_or_rect auto& value, const dots_per_unit_cpt auto& dpmm) noexcept { return value * multipliers::px_to_mm(dpmm); }

	utils_gpu_available inline constexpr utils::math::rect<float> px_to_mm(const utils::math::rect<float>& rect, const dots_per_unit_cpt auto& dpmm) noexcept
		{
		const auto multiplier{[&]() -> utils::math::vec2f
			{
			const auto base{utils::graphics::dpi_conversions::multipliers::px_to_mm(dpmm)};
			if constexpr (std::same_as<std::remove_cvref_t<decltype(dpmm)>, float>)
				{
				return {base, base};
				}
			return base;
			}()};

		return utils::math::rect<float>
			{
			rect.ll() * multiplier.x(),
			rect.up() * multiplier.y(),
			rect.rr() * multiplier.x(),
			rect.dw() * multiplier.y()
			};
		}
	
	utils_gpu_available inline constexpr utils::math::rect<float> mm_to_px(const utils::math::rect<float>& rect, const dots_per_unit_cpt auto& dpmm) noexcept
		{
		const auto multiplier{[&]() -> utils::math::vec2f
			{
			const auto base{utils::graphics::dpi_conversions::multipliers::mm_to_px(dpmm)};
			if constexpr (std::same_as<std::remove_cvref_t<decltype(dpmm)>, float>)
				{
				return {base, base};
				}
			return base;
			}()};
		return utils::math::rect<float>
			{
			rect.ll() * multiplier.x(),
			rect.up() * multiplier.y(),
			rect.rr() * multiplier.x(),
			rect.dw() * multiplier.y()
			};
		}
	}