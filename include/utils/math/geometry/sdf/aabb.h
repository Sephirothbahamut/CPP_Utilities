#pragma once

#include "common.h"
#include "../shape/aabb.h"

namespace utils::math
	{
	template <typename T>
	struct rect<T>::sdf_proxy
		{
		using shape_t = rect<T>;

		sdf_proxy(const shape_t& shape, const vec2f& point) : shape{shape}, point{point} {};
		const shape_t& shape;
		const vec2f point;

		//TODO understand why this breaks intellisense

		utils_gpu_available constexpr vec2f closest_point() const noexcept
		#ifndef __INTELLISENSE__
			{
			if (point.x() <= shape.ll())
				{
				if (point.y() <= shape.up()) { return shape.ul(); }
				if (point.y() >= shape.dw()) { return shape.dl(); }
				return {shape.ll(), point.y()};
				}
			else if (point.x() >= shape.rr())
				{
				if (point.y() <= shape.up()) { return shape.ur(); }
				if (point.y() >= shape.dw()) { return shape.dr(); }
				return {shape.rr(), point.y()};
				}
			else if (point.y() <= shape.up()) 
				{
				return {point.x(), shape.up()};
				}
			else if (point.y() >= shape.dw())
				{
				return {point.x(), shape.dw()};
				}
		
			const float distance_ll{std::abs(point.x() - shape.ll())};
			const float distance_rr{std::abs(point.x() - shape.rr())};
			const float distance_up{std::abs(point.y() - shape.up())};
			const float distance_dw{std::abs(point.y() - shape.dw())};
			const float min_distance_horizontal{std::min(distance_ll, distance_rr)};
			const float min_distance_vertical  {std::min(distance_up, distance_dw)};
		
			if (min_distance_horizontal < min_distance_vertical)
				{
				return {distance_ll < distance_rr ? shape.ll() : shape.rr(), point.y()};
				}
			else
				{
				return {point.x(), distance_up < distance_dw ? shape.up() : shape.dw()};
				}
			}
		#else
			;
		#endif
		
		utils_gpu_available constexpr float minimum_distance() const noexcept
			{
			return signed_distance().absolute();
			}
		
		utils_gpu_available constexpr geometry::sdf::side side() const noexcept
		#ifndef __INTELLISENSE__
			{
			if (point.x() > shape.ll() && point.x() < shape.rr() && point.y() > shape.up() && point.y() < shape.dw()) { return geometry::sdf::side::create::inside (); }
			if (point.x() < shape.ll() || point.x() > shape.rr() || point.y() < shape.up() || point.y() > shape.dw()) { return geometry::sdf::side::create::outside(); }
			return geometry::sdf::side::create::coincident();
			}
		#else
			;
		#endif
		
		utils_gpu_available constexpr geometry::sdf::signed_distance signed_distance() const noexcept
		#ifndef __INTELLISENSE__
			{
			const vec2f point_from_centre_ur_quadrant{utils::math::abs(point - shape.centre())};
			const vec2f corner_from_centre{shape.ur() - shape.centre()};
			const vec2f distances{point_from_centre_ur_quadrant - corner_from_centre};
			return {utils::math::max(distances, {0.f}).get_length() + utils::math::min(utils::math::max(distances.x(), distances.y()), 0.f)};
			}
		#else
			;
		#endif
		
		utils_gpu_available constexpr geometry::sdf::closest_point_with_distance closest_with_distance() const noexcept
		#ifndef __INTELLISENSE__
			{
			const auto closest{closest_point()};
			return {closest, minimum_distance()};
			}
		#else
			;
		#endif
		
		utils_gpu_available constexpr geometry::sdf::closest_point_with_signed_distance closest_with_signed_distance() const noexcept
		#ifndef __INTELLISENSE__
			{
			//TODO test, I had it return positive both inside and outside
			const auto closest{closest_point()};
			return {closest, minimum_distance() * side()};
			}
		#else
			;
		#endif

		utils_gpu_available constexpr geometry::sdf::direction_signed_distance direction_signed_distance() const noexcept
		#ifndef __INTELLISENSE__
			{
			const vec2f point_from_centre{shape.centre() - point};
			const vec2f w{utils::math::abs(point_from_centre) - (shape.size() / 2.f)};//utils::math::abs(point) - (b);
			const vec2f s
				{
				(point_from_centre.x() < 0.f) ? -1.f : 1.f,
				(point_from_centre.y() < 0.f) ? -1.f : 1.f
				};
			
			const float g{max(w.x(), w.y())};
			const vec2f q{max(w, 0.f)};
			const float l{q.get_length()};

			const float distance{(g > 0.f) ? l : g};
			const vec2f direction{s * ((g > 0.f) ? q / l : -((w.x() > w.y()) ? vec2f{1.f, 0.f} : vec2f{0.f, 1.f}))};
			return {distance, direction};
			}
		#else
			;
		#endif
		};
	}

namespace utils::math
	{
	template <typename T>
	utils_gpu_available constexpr rect<T>::sdf_proxy rect<T>::sdf(const vec2f& point) const noexcept 
		requires(std::same_as<value_type, float>)
		{
		return {*this, point};
		}
	}