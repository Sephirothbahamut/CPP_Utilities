#pragma once

#include "compilation/warnings.h"

utils_disable_warnings_begin
utils_disable_warning_msvc(4141)
#include "third_party/polymorphic_value.h"
utils_disable_warnings_end

namespace utils
	{
	using namespace isocpp_p0201;
	}