#include "initializer.h"

#include "../compilation/OS.h"

#ifdef utils_compilation_os_windows
//	{
	#include <Windows.h>
//	}
#elif defined(utils_compilation_os_linux)
//	{
//	}
#endif

namespace utils::console
	{
	initializer::initializer() noexcept
		{
		if constexpr (utils::compilation::os == utils::compilation::os_t::Windows)
			{
			#ifdef utils_compilation_os_windows
			//	{
			SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), ENABLE_VIRTUAL_TERMINAL_INPUT);
			//	}
			#endif
			}
		}
	}