#pragma once

#include <optional>

#include "../../../memory.h"
#include "../../../storage.h"
#include "../../../concepts.h"
#include "../../../math/math.h"
#include "../../../math/angle.h"
#include "../../../math/constants.h"
#include "../../../compilation/gpu.h"
#include "../../../template/optional.h"
#include "../../../oop/disable_move_copy.h"

namespace utils::math
	{
	template <typename T, size_t size>
	struct vec;

	struct transform2;
	}

namespace utils::math::geometry
	{
	namespace ends
		{
		struct ab
			{
			struct create : ::utils::oop::non_constructible
				{
				//TODO when C++23 static operator() is supported replace the "default_" constructor with that

				utils_gpu_available static consteval ab default_(bool a, bool b) noexcept { return ab{.finite_a{a    }, .finite_b{b    }}; }
				utils_gpu_available static consteval ab infinite(              ) noexcept { return ab{.finite_a{false}, .finite_b{false}}; }
				utils_gpu_available static consteval ab finite  (              ) noexcept { return ab{.finite_a{true }, .finite_b{true }}; }
				};

			bool finite_a;
			bool finite_b;

			utils_gpu_available inline consteval bool is_a_infinite() const noexcept { return !finite_a; }
			utils_gpu_available inline consteval bool is_b_infinite() const noexcept { return !finite_b; }
			utils_gpu_available inline consteval bool is_a_finite  () const noexcept { return  finite_a; }
			utils_gpu_available inline consteval bool is_b_finite  () const noexcept { return  finite_b; }
			utils_gpu_available inline consteval bool is_finite    () const noexcept { return  finite_a && finite_b; }
			utils_gpu_available inline consteval bool is_infinite  () const noexcept { return !is_finite(); }

			utils_gpu_available inline consteval bool operator==(const ab& other) const noexcept = default;

			};

		template <ab ends>
		utils_gpu_available inline static constexpr float clamp_t(float t) noexcept
			{
			if (ends.is_a_finite()) { if (t < 0.f) { return 0.f; } }
			if (ends.is_b_finite()) { if (t > 1.f) { return 1.f; } }
			return t;
			}
		template <ab ends>
		utils_gpu_available inline static constexpr float t_or(float t, float if_invalid) noexcept
			{
			if (ends.is_a_finite()) { if (t < 0.f) { return if_invalid; } }
			if (ends.is_b_finite()) { if (t > 1.f) { return if_invalid; } }
			return t;
			}

		struct closeable
			{
			struct create : ::utils::oop::non_constructible
				{
				utils_gpu_available static consteval closeable open    (bool finite_a = false, bool finite_b = false) noexcept { return closeable{.open{true}, .ab_ends{.finite_a{finite_a}, .finite_b{finite_b}}}; }
				utils_gpu_available static consteval closeable infinite(                                            ) noexcept { return closeable{.open{true}, .ab_ends{.finite_a{false   }, .finite_b{false   }}}; }
				utils_gpu_available static consteval closeable closed  (                                            ) noexcept { return closeable{.open{false}}; }
				};

			bool open;
			ab ab_ends;

			utils_gpu_available inline consteval bool operator==(const closeable& other) const noexcept = default;

			utils_gpu_available inline consteval bool is_open      () const noexcept { return open; }
			utils_gpu_available inline consteval bool is_closed    () const noexcept { return !is_open(); }
			utils_gpu_available inline consteval bool is_a_infinite() const noexcept { return  is_open  () && ab_ends.is_a_infinite(); }
			utils_gpu_available inline consteval bool is_b_infinite() const noexcept { return  is_open  () && ab_ends.is_b_infinite(); }
			utils_gpu_available inline consteval bool is_a_finite  () const noexcept { return  is_open  () && ab_ends.is_a_finite  (); }
			utils_gpu_available inline consteval bool is_b_finite  () const noexcept { return  is_open  () && ab_ends.is_b_finite  (); }
			utils_gpu_available inline consteval bool is_finite    () const noexcept { return  is_closed() || ab_ends.is_finite    (); }
			utils_gpu_available inline consteval bool is_infinite  () const noexcept { return !is_finite(); }
			};

		using optional_ab = utils::template_wrapper::optional<ab>;
		}

	struct shape_flag 
		{
		utils_gpu_available constexpr auto  scale         (this const auto& self, const float                    & scaling    ) noexcept;
		utils_gpu_available constexpr auto  scale         (this const auto& self, const vec<float, 2>            & scaling    ) noexcept;
		utils_gpu_available constexpr auto  rotate        (this const auto& self, const angle::base<float, 360.f>& rotation   ) noexcept;
		utils_gpu_available constexpr auto  translate     (this const auto& self, const vec<float, 2>            & translation) noexcept;
		utils_gpu_available constexpr auto  transform     (this const auto& self, const utils::math::transform2  & transform  ) noexcept;
		utils_gpu_available constexpr auto& transform_self(this utils::concepts::non_const auto& self, const utils::math::transform2  & transform  ) noexcept
			requires(!std::remove_cvref_t<decltype(self)>::storage_type.is_const());
		};
	struct piece_flag {};

	namespace shape::concepts
		{
		template <typename T>
		concept shape = std::derived_from<T, shape_flag>;
		}

	namespace shape
		{
		namespace owner         {}
		namespace observer      {}
		namespace const_observer{}

		using namespace owner;
		}
	}