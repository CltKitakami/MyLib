#ifndef _CONSOLE_HPP_
#define _CONSOLE_HPP_

#include <cstdlib>
#include <ostream>
#include <string>

#ifndef _WIN32
#include <unistd.h>
#include <termios.h>
#endif


namespace Console
{
	// http://misc.flogisoft.com/bash/tip_colors_and_formatting
	enum Code
	{
		FG_DEFAULT = 39,
		FG_BLACK = 30,
		FG_RED,
		FG_GREEN,
		FG_YELLOW,
		FG_BLUE,
		FG_MAGENTA,
		FG_CYAN,
		FG_LIGHT_GRAY,
		FG_DARK_GRAY = 90,
		FG_LIGHT_RED,
		FG_LIGHT_GREEN,
		FG_LIGHT_YELLOW,
		FG_LIGHT_BLUE,
		FG_LIGHT_MAGENTA,
		FG_LIGHT_CYAN,
		FG_WHITE,

		BG_DEFAULT = 49,
		BG_BLACK = 40,
		BG_RED,
		BG_GREEN,
		BG_YELLOW,
		BG_BLUE,
		BG_MAGENTA,
		BG_CYAN,
		BG_LIGHT_GRAY,
		BG_DARK_GRAY = 100,
		BG_LIGHT_RED,
		BG_LIGHT_GREEN,
		BG_LIGHT_YELLOW,
		BG_LIGHT_BLUE,
		BG_LIGHT_MAGENTA,
		BG_LIGHT_CYAN,
		BG_WHITE
	};

#ifdef __linux__
	class Color
	{
		std::string codeStr;

	public:
		Color(Code code) : codeStr("\033[")
			{ codeStr += std::to_string((unsigned int)code); codeStr += 'm'; }

		const std::string & value() const { return codeStr; }

		friend std::ostream & operator << (std::ostream& os, const Color &color)
		{ return os << color.codeStr; }
	};
#endif

	static void write(const std::string &str) { std::cout << str; }
	static void writeLine(const std::string &str) { std::cout << str << std::endl; }

	static int waitKey()
	{
#ifdef _WIN32
		char c = 0;
		std::cin >> c;
		return (int)c;
#else
		int c;
		struct termios savedState, newState;

		if (-1 == tcgetattr(STDIN_FILENO, &savedState))
			return EOF;

		newState = savedState;
		newState.c_lflag &= (unsigned)~(ECHO | ICANON);
		newState.c_cc[VMIN] = 1;

		if (-1 == tcsetattr(STDIN_FILENO, TCSANOW, &newState))
			return EOF;

		c = getchar();

		if (-1 == tcsetattr(STDIN_FILENO, TCSANOW, &savedState))
			return EOF;

		return c;
#endif
	}

	static int block(int key = 254)
	{
		int value = 0;

		while (value != key)
			value = Console::waitKey();

		return key;
	}
}

#endif
