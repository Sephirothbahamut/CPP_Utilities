#pragma once

#include "../../details/base_types.h"
#include "../../../../storage.h"
#include "point.h"


namespace utils::math::geometry::shape
	{
	namespace generic
		{
		template <storage::type storage_type>
		struct utils_oop_empty_bases circle;
		}

	namespace concepts
		{
		template <typename T> concept circle = std::same_as<T, shape::generic::circle<T::storage_type>>;
		}
	
	namespace owner 
		{
		using circle = shape::generic::circle<storage::type::create::owner()>;
		}

	namespace observer
		{
		using circle = shape::generic::circle<storage::type::create::observer()>;
		}

	namespace const_observer
		{
		using circle = shape::generic::circle<storage::type::create::const_observer()>;
		}

	template <concepts::circle T, storage::type desired_storage_type>
	struct cast_storage_type<T, desired_storage_type>
		{
		using type = generic::circle<desired_storage_type>;
		};
	}