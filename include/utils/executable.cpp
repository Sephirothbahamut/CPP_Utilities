#include "executable.h"

#include "compilation/OS.h"

#include <array>

#ifdef utils_compilation_os_windows
//	{
	#define WINDOWS_LEAN_AND_MEAN
	#define NOMINMAX
	#include <Windows.h>
//  }
#endif

namespace utils::executable
	{
	std::filesystem::path get_path()
		{
		#ifdef utils_compilation_os_windows
		//	{
			std::array<TCHAR, MAX_PATH> dest;

			DWORD length{GetModuleFileName(NULL, dest.data(), MAX_PATH)};
			dest[length] = '\0';
			return {dest.data()};
		//	}
		#else
			#error "Unimplemented"
		#endif
		}

	std::filesystem::path get_directory()
		{
		std::filesystem::path ret{get_path()};
		ret.remove_filename();
		return ret;
		}
	}