#pragma once
/*
#include <any>
#include <variant>

#include "all.h"
#include "bounds/all.h"
#include "../../../variant.h"

namespace utils::math::geometry::shapes_group
	{
	enum class shape_id
		{
		aabb,
		ab_finite  ,
		ab_infinite,
		ab_finite_to_infinite,
		ab_infinite_to_finite,
		bezier_3pt_finite,
		bezier_3pt_infinite,
		bezier_3pt_finite_to_infinite,
		bezier_3pt_infinite_to_finite,
		bezier_4pt_finite,
		bezier_4pt_infinite,
		bezier_4pt_finite_to_infinite,
		bezier_4pt_infinite_to_finite,
		circle,
		capsule,
		point,
		polyline_closed,
		polyline_finite,
		polyline_infinite,
		polyline_finite_to_infinite,
		polyline_infinite_to_finite,
		mixed_closed,
		mixed_finite,
		mixed_infinite,
		mixed_finite_to_infinite,
		mixed_infinite_to_finite,
		};

	using variant_t = std::variant
		<
		shape::const_observer::aabb,
		shape::const_observer::ab    <   ends::optional_ab::create::value(ends::ab::create::finite  (           ))>,
		shape::const_observer::ab    <   ends::optional_ab::create::value(ends::ab::create::infinite(           ))>,
		shape::const_observer::ab    <   ends::optional_ab::create::value(ends::ab::create::default_(false, true))>,
		shape::const_observer::ab    <   ends::optional_ab::create::value(ends::ab::create::default_(true, false))>,
		shape::const_observer::bezier<3, ends::optional_ab::create::value(ends::ab::create::finite  (           ))>,
		shape::const_observer::bezier<3, ends::optional_ab::create::value(ends::ab::create::infinite(           ))>,
		shape::const_observer::bezier<3, ends::optional_ab::create::value(ends::ab::create::default_(false, true))>,
		shape::const_observer::bezier<3, ends::optional_ab::create::value(ends::ab::create::default_(true, false))>,
		shape::const_observer::bezier<4, ends::optional_ab::create::value(ends::ab::create::finite  (           ))>,
		shape::const_observer::bezier<4, ends::optional_ab::create::value(ends::ab::create::infinite(           ))>,
		shape::const_observer::bezier<4, ends::optional_ab::create::value(ends::ab::create::default_(false, true))>,
		shape::const_observer::bezier<4, ends::optional_ab::create::value(ends::ab::create::default_(true, false))>,
		shape::const_observer::circle,
		shape::const_observer::point,
		shape::const_observer::polyline<ends::closeable::create::closed(            )>,
		shape::const_observer::polyline<ends::closeable::create::open  (true , true )>,
		shape::const_observer::polyline<ends::closeable::create::open  (false, true )>,
		shape::const_observer::polyline<ends::closeable::create::open  (true , false)>,
		shape::const_observer::polyline<ends::closeable::create::open  (false, false)>,
		shape::const_observer::mixed   <ends::closeable::create::closed(            )>,
		shape::const_observer::mixed   <ends::closeable::create::open  (true , true )>,
		shape::const_observer::mixed   <ends::closeable::create::open  (false, true )>,
		shape::const_observer::mixed   <ends::closeable::create::open  (true , false)>,
		shape::const_observer::mixed   <ends::closeable::create::open  (false, false)>
		>;

	/// <summary> A simpler wrapper to pass a group of shapes as parameters. </summary>
	class observers
		{
		public:
			template <typename shape_t>
			void push_back(const shape_t& shape) noexcept
				{
				using observer_t = geometry::shape::cast_storage_type
					<
					shape_t,
					storage::type::create::const_observer()
					>::type;

				static_assert(utils::concepts::is_variant_member<observer_t, variant_t>, "Shape type is not currently supported in shapes groups.");

				observer_t observer{shape};

				variant_t variant{observer};
				observer_shapes.emplace_back(variant);
				}

			std::vector<variant_t> observer_shapes;

			std::vector<shape::aabb> evaluate_bounding_boxes() const noexcept
				{
				std::vector<shape::aabb> bounding_boxes(observer_shapes.size());

				std::ranges::iota_view shapes_indices(size_t{0}, observer_shapes.size());
				std::for_each(std::execution::par, shapes_indices.begin(), shapes_indices.end(), [&bounding_boxes, this](size_t index)
					{
					const variant_t  & shape_variant{observer_shapes[index]};
					      shape::aabb& bounding_box {bounding_boxes [index]};

					std::visit([&bounding_box](const auto& shape)
						{
						bounding_box = shape.bounding_box();
						}, shape_variant);
					});

				return bounding_boxes;
				}
		};

	/// <summary> Main purpose is compacting data in few buffers for CUDA/Rocm, won't complete until they get the language standard updated and I can actually do some testing. </summary>
	//TODO complete when CUDA and/or rocm support all the language features used in shapes.
	//struct compacted
	//	{
	//	struct shape_info
	//		{
	//		enum class type {};
	//		type type;
	//		size_t vertices_end;
	//		size_t radii_end;
	//		};
	//
	//
	//	std::vector<float             > shapes_info;
	//	std::vector<utils::math::vec2f> vertices;
	//	std::vector<float             > radii;
	//	};
	}*/