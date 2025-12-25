#include "io.h"

#include "../compilation/OS.h"

#include <iostream>
#include <string>

//getch
#ifdef utils_compilation_os_windows
//	{
	#include <conio.h>
//	}
#elif utils_compilation_os_linux
//	{
	#include <unistd.h>
	#include <termios.h>

	char _getch()
		{
		//Do not question, I don't know either. It just works.
		char buf = 0;
		struct termios old = {0};
		fflush(stdout);
		if (tcgetattr(0, &old) < 0)
			perror("tcsetattr()");
		old.c_lflag &= ~ICANON;
		old.c_lflag &= ~ECHO;
		old.c_cc[VMIN] = 1;
		old.c_cc[VTIME] = 0;
		if (tcsetattr(0, TCSANOW, &old) < 0)
			perror("tcsetattr ICANON");
		if (read(0, &buf, 1) < 0)
			perror("read()");
		old.c_lflag |= ICANON;
		old.c_lflag |= ECHO;
		if (tcsetattr(0, TCSADRAIN, &old) < 0)
			perror("tcsetattr ~ICANON");
		return buf;
		}
//	}
#else
//	{
	#error Compilation target OS not recognized.
//	}
#endif

//size
#ifdef utils_compilation_os_windows
//	{
	#include <Windows.h>
//	}
#elif utils_compilation_os_linux
//	{
	#include <sys/ioctl.h>
//	}
#endif

namespace utils::console
	{
	char get_ch() noexcept { return static_cast<char>(_getch()); }

	utils::math::vec2s size() noexcept
		{// https://stackoverflow.com/questions/23369503/get-size-of-terminal-window-rows-columns
		#ifdef utils_compilation_os_windows
		//	{
			CONSOLE_SCREEN_BUFFER_INFO csbi;
			GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
			const auto width {static_cast<size_t>(csbi.srWindow.Right - csbi.srWindow.Left + 1)};
			const auto height{static_cast<size_t>(csbi.srWindow.Bottom - csbi.srWindow.Top + 1)};
			return {width, height};
		//	}
		#elif utils_compilation_os_linux
		//	{
			struct winsize w;
			ioctl(fileno(stdout), TIOCGWINSZ, &w);
			const auto width {static_cast<size_t>(w.ws_col)};
			const auto height{static_cast<size_t>(w.ws_row)};
			return {width, height};
		//	}
		#endif
		}



	namespace string
		{
		std::string clear_line() noexcept { return "\x1b[2K"; }
		std::string cursor_up () noexcept { return "\x1b[1A"; }
		std::string clear_lines(size_t count) noexcept
			{
			assert(count >= 1);
			std::string str;
			str.reserve(4 * count + 4 * (count - 1) + 1);
			str += clear_line();
			for (size_t i{1}; i < count; i++)
				{
				str += cursor_up();
				str += clear_line();
				}
			return str;
			}
		}

	void clear_line (            ) noexcept { std::cout << string::clear_line (); }
	void cursor_up  (            ) noexcept { std::cout << string::cursor_up  (); }
	void clear_lines(size_t count) noexcept { std::cout << string::clear_lines(count); }
	}