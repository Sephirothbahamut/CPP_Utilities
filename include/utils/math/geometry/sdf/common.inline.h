// I'm fully aware it's bad but it's 100000 times simpler than yet another CRTP hellscape
sdf_proxy(const shape_t& shape, const vec2f& point) : shape{shape}, point{point} {};
const shape_t& shape;
const vec2f point;

utils_gpu_available constexpr geometry::sdf::direction_signed_distance direction_signed_distance() const noexcept
	{
	const auto closest_with_signed_distance_value{closest_with_signed_distance()};
	return geometry::sdf::direction_signed_distance::create(closest_with_signed_distance_value, point);
	}