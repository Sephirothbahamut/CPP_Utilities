#include "../shape/bezier.h"

namespace utils::math::geometry::shape::generic
	{
	template<storage::type storage_type, size_t extent, geometry::ends::optional_ab optional_ends>
	struct bezier<storage_type, extent, optional_ends>::sdf_proxy
		{
		using shape_t = bezier<storage_type, extent, optional_ends>;

		sdf_proxy(const shape_t& shape, const shape::point& point) : shape{shape}, point{point} {};
		const shape_t& shape;
		const shape::point point;


		};
	}

namespace utils::math::geometry::shape::generic
	{
	template<storage::type storage_type, size_t extent, geometry::ends::optional_ab optional_ends>
	bezier<storage_type, extent, optional_ends>::sdf_proxy bezier<storage_type, extent, optional_ends>::sdf(const concepts::point auto& point) const noexcept
		{
		return {*this, point};
		}
	}