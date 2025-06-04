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

	/// <summary>
	/// Example usage of aggregate::apply.
	/// Given an aggregate of optionals, return a non-optional aggregate where fields that were nullopt/nullptr/whatever are replaced with values from a default values aggregate.
	/// </summary>
	/// <typeparam name="aggregate_accessors"></typeparam>
	/// <typeparam name="aggregate_t"></typeparam>
	/// <typeparam name="aggregate_optionals_t">Aggregate containing std::optional<T>, utils::observer_ptr<T>, anything with an "operator bool()" and "const T& operator*()"</typeparam>
	/// <param name="aggregate_of_optionals">The input aggregate from which to take all the "valid" values.</param>
	/// <param name="default_values_aggregate">Values that are invalid from the aggregate of optionals are taken from this aggregate instead.</param>
	/// <returns>A non optional aggregate</returns>
	template <typename aggregate_accessors, typename aggregate_t, typename aggregate_optionals_t>
	aggregate_t apply_defaults_to_optionals(const aggregate_optionals_t& aggregate_of_optionals, const aggregate_t& default_values_aggregate) noexcept
		{
		aggregate_t ret;

		aggregate::apply<accessors_helper>([](const auto& field_opt, const auto& field_default, auto& field_return)
			{
			field_return = field_opt ? (*field_opt) : field_default;
			}, aggregate_of_optionals, default_values_aggregate, ret);

		return ret;
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
