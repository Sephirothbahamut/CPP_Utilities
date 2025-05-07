#pragma once

#include "common.h"
#include "../shape/dynamic.h"

namespace utils::math::geometry::shape::dynamic
	{
	namespace details
		{
		struct base_sdf_proxy
			{
			virtual geometry::shape::point                            closest_point               () const noexcept = 0;
			virtual float                                             minimum_distance            () const noexcept = 0;
			virtual geometry::sdf::side                               side                        () const noexcept = 0;
			virtual geometry::sdf::signed_distance                    signed_distance             () const noexcept = 0;
			virtual geometry::sdf::closest_point_with_distance        closest_with_distance       () const noexcept = 0;
			virtual geometry::sdf::closest_point_with_signed_distance closest_with_signed_distance() const noexcept = 0;
			};
		}

	struct base::sdf_proxy
		{
		geometry::shape::point                            closest_point               () const noexcept { return dynamic_sdf_proxy->closest_point               (); }
		float                                             minimum_distance            () const noexcept { return dynamic_sdf_proxy->minimum_distance            (); }
		geometry::sdf::side                               side                        () const noexcept { return dynamic_sdf_proxy->side                        (); }
		geometry::sdf::signed_distance                    signed_distance             () const noexcept { return dynamic_sdf_proxy->signed_distance             (); }
		geometry::sdf::closest_point_with_distance        closest_with_distance       () const noexcept { return dynamic_sdf_proxy->closest_with_distance       (); }
		geometry::sdf::closest_point_with_signed_distance closest_with_signed_distance() const noexcept { return dynamic_sdf_proxy->closest_with_signed_distance(); }

		utils::polymorphic_value<details::base_sdf_proxy> dynamic_sdf_proxy;
		};

	namespace details
		{
		template <utils::math::geometry::shape::concepts::shape shape_t, storage::type storage_type>
		struct templated_child<shape_t, storage_type>::sdf_proxy
			{
			using shape_t = templated_child<shape_t, storage_type>;
			#include "common.inline.h"
			
			virtual geometry::shape::point                            closest_point               () const noexcept { shape.shape.sdf(point).closest_point               (); }
			virtual float                                             minimum_distance            () const noexcept { shape.shape.sdf(point).minimum_distance            (); }
			virtual geometry::sdf::side                               side                        () const noexcept { shape.shape.sdf(point).side                        (); }
			virtual geometry::sdf::signed_distance                    signed_distance             () const noexcept { shape.shape.sdf(point).signed_distance             (); }
			virtual geometry::sdf::closest_point_with_distance        closest_with_distance       () const noexcept { shape.shape.sdf(point).closest_with_distance       (); }
			virtual geometry::sdf::closest_point_with_signed_distance closest_with_signed_distance() const noexcept { shape.shape.sdf(point).closest_with_signed_distance(); }
			};


		template <utils::math::geometry::shape::concepts::shape shape_t, storage::type storage_type>
		dynamic::base::sdf_proxy templated_child<shape_t, storage_type>::sdf(const vec<float, 2>& point) const noexcept
			{
			return {utils::make_polymorphic_value<typename templated_child<shape_t, storage_type>::sdf_proxy>(*this, point)};
			}
		}
	}
