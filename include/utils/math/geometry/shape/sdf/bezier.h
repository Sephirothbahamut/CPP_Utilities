#pragma once
#include "return_types.h"
#include "point.h"
#include "../bezier.h"

namespace utils::math::geometry::sdf::details::bezier
	{
	namespace _3pt
		{
		template <ends::ab ends>
		utils_gpu_available constexpr float closest_t(const utils::math::vec2f& point, const shape::concepts::bezier auto& shape) noexcept;
		}
	namespace _4pt
		{
		template <ends::ab ends>
		utils_gpu_available constexpr float closest_t(const utils::math::vec2f& point, const shape::concepts::bezier auto& shape) noexcept;
		}
	namespace other
		{
		template <ends::ab ends>
		utils_gpu_available constexpr float closest_t(const utils::math::vec2f& point, const shape::concepts::bezier auto& shape) noexcept;
		}
	}

namespace utils::math::geometry::shape::generic
	{
	template<storage::type storage_type, size_t extent, geometry::ends::optional_ab optional_ends>
	struct bezier<storage_type, extent, optional_ends>::sdf_proxy
		{
		using shape_t = bezier<storage_type, extent, optional_ends>;
		#include "common.inline.h"

		template <ends::ab ends>
		utils_gpu_available constexpr float closest_t() const noexcept
			requires(extent != std::dynamic_extent)
			{
			if constexpr (extent == 3)
				{
				return utils::math::geometry::sdf::details::bezier::_3pt::closest_t<ends>(point, shape);
				}
			else if constexpr (extent == 4)
				{
				return utils::math::geometry::sdf::details::bezier::_4pt::closest_t<ends>(point, shape);
				}
			else if constexpr (extent != 3 && extent != 4)
				{
				return utils::math::geometry::sdf::details::bezier::other::closest_t<ends>(point, shape);
				}
			}

		template <ends::ab ends>
		utils_gpu_available constexpr float closest_t() const noexcept
			requires(extent == std::dynamic_extent)
			{
			if (shape.vertices.size() == 3)
				{
				return utils::math::geometry::sdf::details::bezier::_3pt::closest_t<ends>(point, shape);
				}
			else if (shape.vertices.size() == 4)
				{
				return utils::math::geometry::sdf::details::bezier::_4pt::closest_t<ends>(point, shape);
				}
			else if (shape.vertices.size() != 3 && shape.vertices.size() != 4)
				{
				return utils::math::geometry::sdf::details::bezier::other::closest_t<ends>(point, shape);
				}
			}

		template <ends::ab ends>
		utils_gpu_available constexpr float closest_t() const noexcept
			{
			return this->proxy_t::closest_t<ends>();
			}

		utils_gpu_available constexpr float closest_t() const noexcept
			requires(optional_ends.has_value())
			{
			return closest_t<optional_ends.value()>();
			}

		template <ends::ab ends>
		utils_gpu_available constexpr auto closest_proxy()const noexcept
			{
			const float t{closest_t<ends>()};
			const auto proxy{shape.at(t)};
			return proxy;
			}

		template <ends::ab ends>
		utils_gpu_available constexpr shape::point closest_point() const noexcept
			{
			const auto proxy{closest_proxy<ends>()};
			return proxy.point();
			}

		template <ends::ab ends>
		utils_gpu_available constexpr float minimum_distance()const noexcept
			{
			const auto proxy{closest_proxy<ends>()};
			const float distance{proxy.point().sdf(point).minimum_distance()};
			return distance;
			}

		template <ends::ab ends>
		utils_gpu_available constexpr geometry::sdf::closest_point_with_distance closest_with_distance()const noexcept
			{
			const auto proxy{closest_proxy<ends>()};
			const float distance{proxy.point().sdf(point).minimum_distance()};
			return {proxy.point(), distance};
			}

		template <ends::ab ends>
		utils_gpu_available constexpr geometry::sdf::side side()const noexcept
			{
			const auto proxy{closest_proxy<ends>()};
			const auto left{proxy.normal()};
			const auto a_to_point{point - proxy.point()};
			//TODO not sure why needs inverting. Normal is left which is outside and should lead to positive result already :shrugs:
			const geometry::sdf::side ret{-utils::math::vec2f::dot(left, a_to_point)}; 
			return ret;
			}

		template <ends::ab ends>
		utils_gpu_available constexpr geometry::sdf::signed_distance signed_distance()const noexcept
			{
			const auto distance{minimum_distance<ends>()};
			const auto side{this->side<ends>()};
			return {distance * side.value()};
			}

		template <ends::ab ends>
		utils_gpu_available constexpr geometry::sdf::closest_point_with_signed_distance closest_with_signed_distance()const noexcept
			{
			//if (shape.vertices.size() == 4)
			//	{//special case for segmented workaround to my proper math ignorance
			//	const float t_mid{closest_t<ends>()};
			//	const float t_a{std::max(0.f, t_mid - shape_stepping_t / 2.f)};
			//	const float t_b{std::min(1.f, t_mid + shape_stepping_t / 2.f)};
			//	const shape::segment edge{shape.at(t_a).point(), shape.at(t_b).point()};
			//	return closest_with_signed_distance(edge, point);
			//	}
			const auto proxy{closest_proxy<ends>()};
			const auto closest{proxy.point()};
			const float distance{minimum_distance<ends>()};

			const auto left{proxy.normal()};
			const auto a_to_point{point - closest};
			const geometry::sdf::side side{-utils::math::vec2f::dot(left, a_to_point)};

			const geometry::sdf::signed_distance signed_distance{distance * side};
			const geometry::sdf::closest_point_with_signed_distance ret{closest, signed_distance};
			return ret;
			}

		utils_gpu_available constexpr auto closest_proxy() const noexcept
			requires(optional_ends.has_value())
			{
			return closest_proxy<optional_ends.value()>();
			}

		utils_gpu_available constexpr shape::point closest_point() const noexcept
			requires(optional_ends.has_value())
			{
			return closest_point<optional_ends.value()>();
			}

		utils_gpu_available constexpr float minimum_distance() const noexcept
			requires(optional_ends.has_value())
			{
			return minimum_distance<optional_ends.value()>();
			}

		utils_gpu_available constexpr geometry::sdf::closest_point_with_distance closest_with_distance() const noexcept
			requires(optional_ends.has_value())
			{
			return closest_with_distance<optional_ends.value()>();
			}

		utils_gpu_available constexpr geometry::sdf::side side() const noexcept
			requires(optional_ends.has_value())
			{
			return side<optional_ends.value()>();
			}

		utils_gpu_available constexpr geometry::sdf::signed_distance signed_distance() const noexcept
			requires(optional_ends.has_value())
			{
			return signed_distance<optional_ends.value()>();
			}

		utils_gpu_available constexpr geometry::sdf::closest_point_with_signed_distance closest_with_signed_distance() const noexcept
			requires(optional_ends.has_value())
			{
			return closest_with_signed_distance<optional_ends.value()>();
			}
		};
	}

namespace utils::math::geometry::shape::generic
	{
	template<storage::type storage_type, size_t extent, geometry::ends::optional_ab optional_ends>
	bezier<storage_type, extent, optional_ends>::sdf_proxy bezier<storage_type, extent, optional_ends>::sdf(const shape::point& point) const noexcept
		{
		return {*this, point};
		}
	}

#include "bezier/all.h"