#pragma once

#include <filesystem>

namespace utils::executable
	{
	std::filesystem::path get_path();

	std::filesystem::path get_directory();
	}

#ifdef utils_implementation
#include "executable.cpp"
#endif