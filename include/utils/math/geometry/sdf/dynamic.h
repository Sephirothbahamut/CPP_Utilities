#pragma once

#include "common.h"
#include "../shape/dynamic.h"

namespace utils::math::geometry::shape::dynamic
	{
	namespace details
		{
		struct base_sdf_proxy
			{
			base_sdf_proxy(const vec2f& point) : point{point} {}
			const vec2f point;

			geometry::sdf::direction_signed_distance direction_signed_distance() const noexcept
				{
				const auto closest_with_signed_distance_value{closest_with_signed_distance()};
				return geometry::sdf::direction_signed_distance::create(closest_with_signed_distance_value, point);
				}

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
		geometry::sdf::direction_signed_distance          direction_signed_distance   () const noexcept { return dynamic_sdf_proxy->direction_signed_distance   (); }
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
		template <utils::math::geometry::shape::concepts::shape shape_t>
		struct templated_child<shape_t>::sdf_proxy : details::base_sdf_proxy
			{
			sdf_proxy(const shape_t& shape, const vec2f& point) : shape{shape}, details::base_sdf_proxy{point} {};
			const shape_t& shape;

			virtual geometry::shape::point                            closest_point               () const noexcept { return shape.sdf(point).closest_point               (); }
			virtual float                                             minimum_distance            () const noexcept { return shape.sdf(point).minimum_distance            (); }
			virtual geometry::sdf::side                               side                        () const noexcept { return shape.sdf(point).side                        (); }
			virtual geometry::sdf::signed_distance                    signed_distance             () const noexcept { return shape.sdf(point).signed_distance             (); }
			virtual geometry::sdf::closest_point_with_distance        closest_with_distance       () const noexcept { return shape.sdf(point).closest_with_distance       (); }
			virtual geometry::sdf::closest_point_with_signed_distance closest_with_signed_distance() const noexcept { return shape.sdf(point).closest_with_signed_distance(); }
			};


		template <utils::math::geometry::shape::concepts::shape shape_t>
		dynamic::base::sdf_proxy templated_child<shape_t>::sdf(const vec<float, 2>& point) const noexcept
			{
			return {utils::make_polymorphic_value<details::base_sdf_proxy, typename templated_child<shape_t>::sdf_proxy>(this->shape, point)};
			}
		}
	}
