#pragma once

#include "../../details/base_types.h"
#include "../../../vec2.h"

namespace utils::math::geometry::shape
	{
	namespace generic
		{
		template <storage::type storage_type>
		using point = ::utils::math::vec<utils::storage::storage_type_for<float, storage_type>, 2>;
		}
	namespace concepts
		{
		template <typename T>
		concept point = std::same_as<std::remove_cvref_t<T>, shape::generic::point<std::remove_cvref_t<T>::storage_type>>;
		}

	namespace owner         { using point = shape::generic::point<storage::type::create::owner         ()>; }
	namespace observer      { using point = shape::generic::point<storage::type::create::observer      ()>; }
	namespace const_observer{ using point = shape::generic::point<storage::type::create::const_observer()>; }

	template <concepts::point T, storage::type desired_storage_type>
	struct cast_storage_type<T, desired_storage_type>
		{
		using type = generic::point<desired_storage_type>;
		};

	template <storage::type desired_storage_type>
	utils_gpu_available constexpr auto cast_storage(const concepts::point auto& shape) noexcept
		{
		const typename cast_storage_type<std::remove_cvref_t<decltype(shape)>, desired_storage_type>::type ret{shape};
		return ret;
		}
	template <storage::type desired_storage_type>
	utils_gpu_available constexpr auto cast_storage(concepts::point auto& shape) noexcept
		{
		typename cast_storage_type<std::remove_cvref_t<decltype(shape)>, desired_storage_type>::type ret{shape};
		return ret;
		}
	}