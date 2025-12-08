#pragma once

#include "../../details/base_types.h"
#include "../../../../storage.h"


namespace utils::math::geometry::shape
	{
	namespace generic
		{
		template <storage::type storage_type>
		struct utils_oop_empty_bases arc;
		}

	namespace concepts
		{
		template <typename T> 
		concept arc = concepts::shape<T> && std::same_as<T, shape::generic::arc<T::storage_type>>;
		}
	
	namespace owner 
		{
		using arc = shape::generic::arc<storage::type::create::owner()>;
		}

	namespace observer
		{
		using arc = shape::generic::arc<storage::type::create::observer()>;
		}

	namespace const_observer
		{
		using arc = shape::generic::arc<storage::type::create::const_observer()>;
		}
	}