#pragma once

#include <optional>

#include "../../../memory.h"
#include "../../../storage.h"
#include "../../../math/math.h"
#include "../../../compilation/gpu.h"
#include "../../../math/constants.h"
#include "../../../oop/disable_move_copy.h"

namespace utils::math
	{
	template <typename T, size_t size>
	struct vec;

	using vec2f = vec<float, 2>;
	}

namespace utils::math::geometry
	{
	struct ends
		{
		struct create : ::utils::oop::non_constructible
			{
			utils_gpu_available static consteval ends open    (bool finite_a = false, bool finite_b = false) noexcept { return ends{.finite_a{finite_a}, .finite_b{finite_b}, .open{true }}; }
			utils_gpu_available static consteval ends infinite(                                            ) noexcept { return ends{.finite_a{false   }, .finite_b{false   }, .open{true }}; }
			utils_gpu_available static consteval ends closed  (                                            ) noexcept { return ends{.finite_a{true    }, .finite_b{true    }, .open{false}}; }
			};

		bool finite_a;
		bool finite_b;
		bool open;

		utils_gpu_available inline consteval bool operator==(const ends& other) const noexcept = default;

		utils_gpu_available inline consteval bool is_open      () const noexcept { return open; }
		utils_gpu_available inline consteval bool is_closed    () const noexcept { return !is_open(); }
		utils_gpu_available inline consteval bool is_a_infinite() const noexcept { return is_open() && !finite_a; }
		utils_gpu_available inline consteval bool is_b_infinite() const noexcept { return is_open() && !finite_b; }
		utils_gpu_available inline consteval bool is_a_finite  () const noexcept { return is_open() &&  finite_a; }
		utils_gpu_available inline consteval bool is_b_finite  () const noexcept { return is_open() &&  finite_b; }
		utils_gpu_available inline consteval bool is_finite    () const noexcept { return is_closed() || (finite_a && finite_b); }
		utils_gpu_available inline consteval bool is_infinite  () const noexcept { return !is_finite(); }
		};

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