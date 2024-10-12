#pragma once

#include "../../details/base_types.h"
#include "../../../../storage.h"
#include "ab.h"

namespace utils::math::geometry::shape
	{
	namespace generic
		{
		template <storage::type storage_type>
		struct utils_oop_empty_bases capsule;
		}

	namespace concepts
		{
		template <typename T> concept capsule = std::same_as<T, shape::generic::capsule<T::storage_type>>;
		}
	
	namespace owner 
		{
		using capsule = shape::generic::capsule<storage::type::create::owner()>;
		}

	namespace observer
		{
		using capsule = shape::generic::capsule<storage::type::create::observer()>;
		}

	namespace const_observer
		{
		using capsule = shape::generic::capsule<storage::type::create::const_observer()>;
		}

	template <concepts::capsule T, storage::type desired_storage_type>
	struct cast_storage_type<T, desired_storage_type>
		{
		using type = generic::capsule<desired_storage_type>;
		};

	template <storage::type desired_storage_type>
	utils_gpu_available constexpr auto cast_storage(const concepts::capsule auto& shape) noexcept
		{
		const typename cast_storage_type<std::remove_cvref_t<decltype(shape)>, desired_storage_type>::type ret{shape};
		return ret;
		}
	template <storage::type desired_storage_type>
	utils_gpu_available constexpr auto cast_storage(concepts::capsule auto& shape) noexcept
		{
		typename cast_storage_type<std::remove_cvref_t<decltype(shape)>, desired_storage_type>::type ret{shape};
		return ret;
		}
	}