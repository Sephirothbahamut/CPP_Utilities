#pragma once

#include <optional>

#include "../../../memory.h"
#include "../../../storage.h"
#include "../../../math/math.h"
#include "../../../math/constants.h"
#include "../../../compilation/gpu.h"
#include "../../../template/optional.h"
#include "../../../oop/disable_move_copy.h"

namespace utils::math
	{
	template <typename T, size_t size>
	struct vec;

	using vec2f = vec<float, 2>;
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

		struct closeable
			{
			struct create : ::utils::oop::non_constructible
				{
				utils_gpu_available static consteval closeable open    (bool finite_a = false, bool finite_b = false) noexcept { return closeable{.open{true}, .ab{.finite_a{finite_a}, .finite_b{finite_b}}}; }
				utils_gpu_available static consteval closeable infinite(                                            ) noexcept { return closeable{.open{true}, .ab{.finite_a{false   }, .finite_b{false   }}}; }
				utils_gpu_available static consteval closeable closed  (                                            ) noexcept { return closeable{.open{false}}; }
				};

			bool open;
			ab ab;

			utils_gpu_available inline consteval bool operator==(const closeable& other) const noexcept = default;

			utils_gpu_available inline consteval bool is_open      () const noexcept { return open; }
			utils_gpu_available inline consteval bool is_closed    () const noexcept { return !is_open(); }
			utils_gpu_available inline consteval bool is_a_infinite() const noexcept { return  is_open  () && ab.is_a_infinite(); }
			utils_gpu_available inline consteval bool is_b_infinite() const noexcept { return  is_open  () && ab.is_b_infinite(); }
			utils_gpu_available inline consteval bool is_a_finite  () const noexcept { return  is_open  () && ab.is_a_finite  (); }
			utils_gpu_available inline consteval bool is_b_finite  () const noexcept { return  is_open  () && ab.is_b_finite  (); }
			utils_gpu_available inline consteval bool is_finite    () const noexcept { return  is_closed() || ab.is_finite    (); }
			utils_gpu_available inline consteval bool is_infinite  () const noexcept { return !is_finite(); }
			};

		using optional_ab = utils::template_wrapper::optional<ab>;
		}

	struct shape_flag {};
	struct piece_flag {};

	namespace shape::concepts
		{
		template <typename T>
		concept shape = std::derived_from<T, shape_flag>;

		template <typename T>
		concept piece = shape<T> && requires(T t)
			{
					{ t.begin_point  () } -> std::same_as<utils::math::vec2f>;
					{ t.begin_tangent() } -> std::same_as<utils::math::vec2f>;
					{ t.end_point    () } -> std::same_as<utils::math::vec2f>;
					{ t.end_tangent  () } -> std::same_as<utils::math::vec2f>;
			};


		}

	namespace shape
		{
		namespace owner         {}
		namespace observer      {}
		namespace const_observer{}

		using namespace owner;
		}
	}