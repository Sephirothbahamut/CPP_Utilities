#pragma once

#include "../return_types.h"
#include "../../bezier.h"
#include "../../../../vec4.h"

namespace utils::math::geometry::sdf::details::bezier::_4pt
	{
	struct closest_precalculated_information_t
		{
		float              distance2{utils::math::constants::finf};
		float              t{0.f};
		utils::math::vec2f point;
		utils_gpu_available float distance() const noexcept { return std::sqrt(distance2); } //TODO constexpr when std::sqrt is constexpr
		};

	namespace free_functions
		{
		template <typename T, size_t capacity>
		struct simple_static_vector
			{
			size_t size{0};
			std::array<T, capacity> data{};

			utils_gpu_available constexpr void push_back(T value) noexcept
				{
				assert(size < capacity);
				data[size] = value;
				size++;
				}
			};

		// Simple version
		simple_static_vector<float, 5> solved_quintic(const utils::math::vec2f point, const utils::math::vec2f v1, const utils::math::vec2f v2, const utils::math::vec2f v3, const utils::math::vec2f v4) noexcept;

		// Faster version I gave up trying to understand, it passes by reference solutions to one degree of math function to another, I'm utterly lost.
		closest_precalculated_information_t cubic_bezier_dis(utils::math::vec2f uv, utils::math::vec2f p0, utils::math::vec2f p1, utils::math::vec2f p2, utils::math::vec2f p3) noexcept;
		}

	template<storage::type storage_type, geometry::ends::optional_ab optional_ends>
	struct sdf_proxy
		{
		using shape_t = geometry::shape::generic::bezier<storage_type, 4, optional_ends>;

		sdf_proxy(const shape_t& shape, const vec2f& point) : shape{shape}, point{point} {};
		const shape_t& shape;
		const vec2f point;
		
		template <utils::math::geometry::ends::ab ends>
		utils_gpu_available constexpr closest_precalculated_information_t closest_precalculated_information() const noexcept
			{
			//return free_functions::cubic_bezier_dis(point, shape.vertices[0], shape.vertices[1], shape.vertices[2], shape.vertices[3]);

			closest_precalculated_information_t closest;

			const auto quintic_roots = free_functions::solved_quintic(point, shape.vertices[0], shape.vertices[1], shape.vertices[2], shape.vertices[3]);
			for (size_t i{0}; i < quintic_roots.size; i++)
				{
				const float candidate_t{geometry::ends::clamp_t<ends>(quintic_roots.data[i])};

				const auto  candidate_at_proxy{shape.at(candidate_t)};
				const auto  candidate_point{candidate_at_proxy.point()};
				const float candidate_distance2{utils::math::vec2f::distance2(candidate_point, point)};

				if (candidate_distance2 < closest.distance2)
					{
					closest.distance2 = candidate_distance2;
					closest.t         = candidate_t    ;
					closest.point     = candidate_point;
					}
				}

			return closest;
			}

		template <ends::ab ends>
		utils_gpu_available constexpr float closest_t() const noexcept
			{
			const auto closest{closest_precalculated_information<ends>()};
			return closest.t;
			}
		};
	}


#ifdef utils_implementation
#include "4pt.cpp"
#endif