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
		#ifdef utils_compilation_os_windows
		const auto hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		DWORD mode;
		GetConsoleMode(hConsole, &mode);
		SetConsoleMode(hConsole, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);

		SetConsoleOutputCP(65001); //utf-8 console
		#endif
		}
	}