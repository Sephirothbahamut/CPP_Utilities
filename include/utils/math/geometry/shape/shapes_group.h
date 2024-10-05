#pragma once

#include <any>
#include <variant>

#include "all.h"

namespace utils::math::geometry::shapes::group
	{
	using variant_t = std::variant
		<
		shape::const_observer::aabb,
		shape::const_observer::ab    <   ends::optional_ab::create::value(ends::ab::create::finite  (           ))>,
		shape::const_observer::ab    <   ends::optional_ab::create::value(ends::ab::create::infinite(           ))>,
		shape::const_observer::ab    <   ends::optional_ab::create::value(ends::ab::create::default_(false, true))>,
		shape::const_observer::ab    <   ends::optional_ab::create::value(ends::ab::create::default_(true, false))>,
		shape::const_observer::bezier<   ends::optional_ab::create::value(ends::ab::create::finite  (           ))>,
		shape::const_observer::bezier<   ends::optional_ab::create::value(ends::ab::create::infinite(           ))>,
		shape::const_observer::bezier<   ends::optional_ab::create::value(ends::ab::create::default_(false, true))>,
		shape::const_observer::bezier<   ends::optional_ab::create::value(ends::ab::create::default_(true, false))>,
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
		shape::const_observer::mixed   <ends::closeable::create::open  (false, false)>,
		>;

	/// <summary> A simpler wrapper to pass a group of shapes as parameters. </summary>
	class view
		{
		public:
			template <shape::concepts::shape T>
			void push_back(const auto& T) noexcept
				{
				T::const_observer
				}

			std::vector<variant_t> observer_shapes;
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
	}