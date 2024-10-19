#pragma once

#include "3pt.h"
#include "4pt.h"
#include "other.h"

namespace utils::math::geometry::sdf::details::bezier
	{
	template<storage::type storage_type, size_t extent, geometry::ends::optional_ab optional_ends>
	using proxy_t =
		//*
		std::conditional_t
			<
			extent == 3,
			geometry::sdf::details::bezier::_3pt::sdf_proxy<storage_type, optional_ends>,
			std::conditional_t
				<
				extent == 4,
				geometry::sdf::details::bezier::_4pt::sdf_proxy<storage_type, optional_ends>,
				geometry::sdf::details::bezier::other::sdf_proxy<storage_type, extent, optional_ends>
				>
			>;
		/*/
		geometry::sdf::details::bezier::other::sdf_proxy<storage_type, extent, optional_ends>;
		//*/
	}