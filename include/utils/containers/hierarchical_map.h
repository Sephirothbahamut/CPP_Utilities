#pragma once

#include <tuple>
#include <string>
#include <vector>
#include <variant>
#include <unordered_map>

#include "../template/string.h"
#include "../variadic.h"

namespace utils::containers
	{
	template <typename T, utils::template_wrapper::string name>
	struct key_value
		{
		using value_type = T;
		value_type value;
		};

	template <typename ...KEY_VALUES>
	class hierarchical_map
		{
		using key_values_tuple   = std::tuple  <KEY_VALUES...>;
		using key_values_variant = std::variant<(typename KEY_VALUES::value_type)...>;

		key_values_tuple default_values;
		std::vector<std::unordered_map<std::string, key_values_variant>> stack;

		size_t index_in_tuple(std::string_view key)
			{
			utils::tuple::
				utils::tuple::for_each_in_tuple(default_values, [](const auto& key_value) {});
			}

		};
	}