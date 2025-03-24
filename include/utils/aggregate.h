#pragma once

#include "variadic.h"

namespace utils::aggregate
	{
	struct accessors_helper_flag {};
	template <auto ...ACCESSORS>
	struct accessors_helper : accessors_helper_flag
		{
		inline static constexpr std::tuple accessors{ACCESSORS...};
		};

	struct accessors_recursive_helper_flag {};
	template <auto inner_accessor, typename ACCESSORS_RECURSIVE_HELPER>
	struct accessors_recursive_helper : accessors_recursive_helper_flag
		{
		constexpr auto& operator()(auto& instance) const noexcept { return inner_accessor(instance); }
		using accessors_helper = ACCESSORS_RECURSIVE_HELPER;
		};

	template <typename aggregate_accessors, typename callback_t>
	constexpr void apply(callback_t callback, auto& ...aggregates)
		{
		utils::tuple::for_each_in_tuple(aggregate_accessors::accessors, [&](auto& accessor)
			{
			//std::tuple fields_refs{std::ref(accessor(aggregates)), ...};

			using accessor_t = std::remove_cvref_t<decltype(accessor)>;

			if constexpr (std::derived_from<accessor_t, accessors_recursive_helper_flag>)
				{
				//aggregate_apply<typename accessor_t::accessors_helper>(callback, fields_refs);
				apply<typename accessor_t::accessors_helper>(callback, accessor(aggregates)...);
				}
			if constexpr (!std::derived_from<accessor_t, accessors_recursive_helper_flag>)
				{
				if constexpr (sizeof...(aggregates) == 0)
					{
					callback();
					}
				else
					{
					callback(accessor(aggregates)...);
					}
				}
			});
		}
	}



// Example:
// 
//	struct aggregate_t
//		{
//		struct inner_t
//			{
//			int x, y;
//
//			};
//
//		inner_t a;
//		int b;
//		inner_t c;
//		};
//	struct aggregate_opt_t
//		{
//		struct inner_t
//			{
//			std::optional<int> x, y;
//			};
//
//		std::optional<inner_t> a;
//		std::optional<int    > b;
//		std::optional<inner_t> c;
//		};
//	using accessors_helper__aggregate = accessors_helper
//		<
//		accessors_recursive_helper
//			<
//			[](auto& owner) noexcept -> auto& { return owner.a; },
//			accessors_helper
//				<
//				[](auto& instance) noexcept -> auto& { return instance.x; },
//				[](auto& instance) noexcept -> auto& { return instance.y; }
//				>
//			> {},
//		[](auto& instance) noexcept -> auto& { return instance.b; },
//		[](auto& instance) noexcept -> auto& { return instance.c.x; },
//		[](auto& instance) noexcept -> auto& { return instance.c.y; }
//		>;
