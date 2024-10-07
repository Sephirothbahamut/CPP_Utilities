#pragma once

#include <optional>

#include "../../../memory.h"
#include "../../../storage.h"
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

	using vec2f = vec<float, 2>;

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

	struct shape_flag 
		{
		utils_gpu_available constexpr auto  scale         (this const auto& self, const float                    & scaling    ) noexcept;
		utils_gpu_available constexpr auto  rotate        (this const auto& self, const angle::base<float, 360.f>& rotation   ) noexcept;
		utils_gpu_available constexpr auto  translate     (this const auto& self, const vec2f                    & translation) noexcept;
		utils_gpu_available constexpr auto  transform     (this const auto& self, const utils::math::transform2  & transform  ) noexcept;
		utils_gpu_available constexpr auto& transform_self(this       auto& self, const utils::math::transform2  & transform  ) noexcept
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

		template <concepts::shape T, storage::type desired_storage_type>
		struct cast_storage_type;

		template <storage::type desired_storage_type, concepts::shape shape_t>
		auto cast_storage(const shape_t& shape)
			{
			//Note: the type alias make it so that intellisense shows "return_type" as type of the returned value assigned to an "auto" variable.
			// hence I'm not aliasing the type so we get a full readable type in the popup windows.
			//using return_type = typename cast_storage_type<shape_t, desired_storage_type>::type;
			const typename cast_storage_type<shape_t, desired_storage_type>::type ret{shape};
			return ret;
			}
		template <storage::type desired_storage_type, concepts::shape shape_t>
		auto cast_storage(shape_t& shape)
			{
			//Note: the type alias make it so that intellisense shows "return_type" as type of the returned value assigned to an "auto" variable.
			// hence I'm not aliasing the type so we get a full readable type in the popup windows.
			//using return_type = typename cast_storage_type<shape_t, desired_storage_type>::type;
			typename cast_storage_type<shape_t, desired_storage_type>::type ret{shape};
			return ret;
			}
		}
	}