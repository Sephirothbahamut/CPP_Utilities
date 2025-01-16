#pragma once

#include "../../details/base_types.h"
#include "../../../../storage.h"
#include "../../../../template/optional.h"
#include "point.h"

namespace utils::math::geometry::shape
	{
	namespace generic
		{
		template <storage::type STORAGE_TYPE, geometry::ends::optional_ab OPTIONAL_ENDS>
		struct utils_oop_empty_bases ab;

		template <storage::type storage_type, ends::ab ends>
		using ab_ends_aware = ab<storage_type, geometry::ends::optional_ab::create::value(ends)>;
		
		template <storage::type storage_type>
		using line = ab_ends_aware<storage_type, ends::ab::create::infinite()>;
		template <storage::type storage_type>
		using ray = ab_ends_aware<storage_type, ends::ab::create::default_(true, false)>;
		template <storage::type storage_type>
		using reverse_ray = ab_ends_aware<storage_type, ends::ab::create::default_(false, true)>;
		template <storage::type storage_type>
		using segment = ab_ends_aware<storage_type, ends::ab::create::finite()>;
		}

	namespace concepts
		{
		template <typename T> concept ab            = concepts::shape<T> && std::derived_from<T, shape::generic::ab<T::storage_type, T::optional_ends>>;
		template <typename T> concept ab_ends_aware = ab<T> && T::optional_ends.has_value();
		template <typename T> concept line          = ab_ends_aware<T> && std::same_as<T, shape::generic::line       <T::storage_type>>;
		template <typename T> concept ray           = ab_ends_aware<T> && std::same_as<T, shape::generic::ray        <T::storage_type>>;
		template <typename T> concept reverse_ray   = ab_ends_aware<T> && std::same_as<T, shape::generic::reverse_ray<T::storage_type>>;
		template <typename T> concept segment       = ab_ends_aware<T> && std::same_as<T, shape::generic::segment    <T::storage_type>>;
		}
	
	namespace owner 
		{
		template <geometry::ends::optional_ab optional_ends = geometry::ends::optional_ab::create::empty()>
		using ab          = shape::generic::ab         <storage::type::create::owner(), optional_ends>;
		using line        = shape::generic::line       <storage::type::create::owner()>;
		using ray         = shape::generic::ray        <storage::type::create::owner()>;
		using reverse_ray = shape::generic::reverse_ray<storage::type::create::owner()>;
		using segment     = shape::generic::segment    <storage::type::create::owner()>;
		}

	namespace observer
		{
		template <geometry::ends::optional_ab optional_ends = geometry::ends::optional_ab::create::empty()>
		using ab          = shape::generic::ab         <storage::type::create::observer(), optional_ends>;
		using line        = shape::generic::line       <storage::type::create::observer()>;
		using ray         = shape::generic::ray        <storage::type::create::observer()>;
		using reverse_ray = shape::generic::reverse_ray<storage::type::create::observer()>;
		using segment     = shape::generic::segment    <storage::type::create::observer()>;
		}

	namespace const_observer
		{
		template <geometry::ends::optional_ab optional_ends = geometry::ends::optional_ab::create::empty()>
		using ab          = shape::generic::ab         <storage::type::create::const_observer(), optional_ends>;
		using line        = shape::generic::line       <storage::type::create::const_observer()>;
		using ray         = shape::generic::ray        <storage::type::create::const_observer()>;
		using reverse_ray = shape::generic::reverse_ray<storage::type::create::const_observer()>;
		using segment     = shape::generic::segment    <storage::type::create::const_observer()>;
		}
	}