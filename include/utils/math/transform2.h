#pragma once

#include "angle.h"
#include "vec.h"

namespace utils::math
	{
	struct transform2
		{
		static transform2 zero() noexcept { return {{}, {}, 1.f}; }

		vec2f translation{0.f, 0.f};
		utils::math::angle::radf rotation{0.f};
		float scaling{1.f};

		transform2  operator+ (const transform2& oth) const noexcept 
			{
			return {translation + oth.translation, rotation + oth.rotation, scaling * oth.scaling}; 
			}
		transform2& operator+=(const transform2& oth)       noexcept { return *this = *this + oth; }
		transform2  operator- ()                      const noexcept
			{
			transform2 ret{ -translation, -rotation, 1.f / scaling};
			ret.translation = ((ret.translation * ret.scaling) + ret.rotation); //TODO not sure
			return ret;
			}
		transform2& operator--()                            noexcept { return *this = -(*this); }

		transform2  operator* (float delta_time) const noexcept { return  {translation * delta_time, rotation * delta_time, scaling}; }
		transform2& operator*=(float delta_time)       noexcept { *this = {translation * delta_time, rotation * delta_time, scaling}; return *this; }

		bool operator==(const transform2& oth) const noexcept { return translation == oth.translation && rotation == oth.rotation && scaling == oth.scaling; }
		bool operator!=(const transform2& oth) const noexcept { return !(*this == oth); }

		transform2  composite(const transform2& oth)  const noexcept { return *this +  oth; }//TODO fix, adding elements is not compose and i'm too tired to fix it now :(
		transform2& compose  (const transform2& oth)        noexcept { return *this += oth; }//TODO fix, adding elements is not compose and i'm too tired to fix it now :(
		transform2  inverse()                         const noexcept { return  -(*this); }
		transform2& invert ()                               noexcept { return --(*this); }
		};
	}

namespace utils::math
	{
	inline transform2 lerp(transform2 a, transform2 b, float t) noexcept
		{
		return
			{
			.translation{utils::math::lerp(a.translation, b.translation, t)},
			.rotation   {utils::math::lerp(a.rotation   , b.rotation   , t)},
			.scaling    {utils::math::lerp(a.scaling    , b.scaling    , t)}
			};
		}
	}
namespace utils::output
	{
	//namespace typeless
	//	{
	//	inline ::std::ostream& operator<<(::std::ostream& os, const utils::math::transform2& container)
	//		{
	//		namespace ucc = utils::console::colour;
	//		return os << ucc::brace << "(" 
	//			<< ucc::type << "translation: " << container.translation << ucc::separ << ", " 
	//			<< ucc::type << "rotation: "    << container.rotation    << ucc::separ << ", "
	//			<< ucc::type << "scaling: "     << ucc::value << container.scaling 
	//			<< ucc::brace << ")";
	//		}
	//	}
	//
	//inline ::std::ostream& operator<<(::std::ostream& os, const utils::math::transform2& container)
	//	{
	//	namespace ucc = utils::console::colour;
	//	os << ucc::type << "transform2";
	//	return utils::output::typeless::operator<<(os, container);
	//	}
	}