#pragma once

#include <concepts>

namespace utils::oop
	{
	template <typename T, typename owner_t, typename value_type>
	concept proxy_setter = requires(T t, owner_t owner, const value_type & value)
		{
			{ (owner.*t)(value) };//->std::same_as<T>;
		};
	template <typename T, typename owner_t, typename value_type>
	concept proxy_getter = requires(T t, owner_t owner)
		{
			{ (owner.*t)() } -> std::same_as<value_type>;
		};

	template <typename owner_t, bool is_const, typename proxy_value_type, auto setter, auto getter>
		requires proxy_setter<decltype(setter), owner_t, proxy_value_type> && proxy_getter<decltype(getter), owner_t, proxy_value_type>
	struct proxy
		{
		//Can't declare template paramters as friends, no private constructor :_(
		//friend class owner_t;

		using self_t = proxy<owner_t, is_const, proxy_value_type, setter, getter>;
		using owner_const_aware_ref_t = std::conditional_t<is_const, const owner_t&, owner_t&>;

		operator proxy_value_type() const noexcept { return value(); }
		proxy_value_type operator->() const noexcept { return value(); }
		proxy_value_type value() const noexcept 
			{
			return (owner_const_aware_ref.*getter)(); 
			}
		self_t operator=(const proxy_value_type& new_value) noexcept requires(!is_const) 
			{
			(owner_const_aware_ref.*setter)(new_value); 
			return {*this};
			}

		//private:
			proxy(owner_const_aware_ref_t owner_const_aware_ref) : owner_const_aware_ref{owner_const_aware_ref} {}
		private:
			owner_const_aware_ref_t owner_const_aware_ref;
		};
	}