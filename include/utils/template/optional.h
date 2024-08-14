#pragma once

#include <tuple>
#include <algorithm>

#include "../concepts.h"
#include "../oop/disable_move_copy.h"

namespace utils
	{
	namespace template_wrapper
		{
		template <typename value_t>
		struct optional
			{
			struct create : ::utils::oop::non_constructible
				{
				static consteval auto value(value_t value) noexcept { return optional<value_t>{._has_value{true}, ._value{value}}; }
				static consteval auto empty(             ) noexcept { return optional<value_t>{._has_value{false}}; }
				};

			bool _has_value;
			value_t _value;

			consteval bool has_value() const noexcept { return _has_value; }
			consteval operator bool() const noexcept { return has_value(); }
			consteval value_t value() const noexcept { return _value; }
			consteval value_t* operator->() const noexcept { return &_value; }

			consteval bool operator==(const optional<value_t>& other) const noexcept
				{
				if (!has_value() || !other.has_value()) { return false; }
				return value() == other.value();
				};
			};
		}
	}