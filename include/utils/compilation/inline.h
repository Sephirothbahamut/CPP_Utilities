#pragma once

#include "compiler.h"

#if defined(utils_compiler_msvc)
	#define utils_force_inline __forceinline inline
#elif defined(utils_compiler_gcc) || defined(utils_compiler_clang)
	#define utils_force_inline [[gnu::always_inline]]
#endif