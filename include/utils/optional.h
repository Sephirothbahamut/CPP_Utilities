#pragma once

#include <optional>

#include "memory.h"

namespace utils
	{
	template <typename T>
	observer_ptr<T> optional_to_observer_ptr(std::optional<T>& optional)
		{
		if (optional)
			{
			return &(*optional);
			}
		else
			{
			return nullptr;
			}
		}

	template <typename T>
	observer_ptr<const T> optional_to_observer_ptr(const std::optional<T>& optional)
		{
		if (optional)
			{
			return &(*optional);
			}
		else
			{
			return nullptr;
			}
		}

	template <typename T>
	std::optional<T> observer_ptr_to_optional(observer_ptr<T> ptr)
		{
		if (ptr)
			{
			return *ptr;
			}
		else
			{
			return std::nullopt;
			}
		}
	}