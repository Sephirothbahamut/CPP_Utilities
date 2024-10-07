#pragma once

#include <variant>

//TODO evaluate if worth keeping

namespace utils
	{
	// https://stackoverflow.com/questions/63482070/how-can-i-code-something-like-a-switch-for-stdvariant
	
	// helper type for the visitor #4
	template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };

	// Usage:
	// std::visit(overloaded
	// 	{
	// 	[](std::monostate&) {/*..*/},
	// 	[](a&) {/*..*/},
	// 	[](b&) {/*..*/},
	// 	[](c&) {/*..*/}
	// 	}, var);

	namespace concepts
		{
		namespace details
			{
			//https://stackoverflow.com/questions/45892170/how-do-i-check-if-an-stdvariant-can-hold-a-certain-type
			template<typename T, typename VARIANT_T>
			struct is_variant_member;

			template<typename T, typename... ALL_T>
			struct is_variant_member<T, std::variant<ALL_T...>>
				: public std::disjunction<std::is_same<T, ALL_T>...> {};
			}

		template <typename T, typename variant_T>
		concept is_variant_member = details::is_variant_member<T, variant_T>::value;
		}
	}