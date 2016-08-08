#ifndef _CLIPBOARD_HPP_
#define _CLIPBOARD_HPP_

#include <string>
#include "pattern/Observer.hpp"


class Clipboard
{
public:
	static std::string getText();

	static void setText(const char *text);
	static void setText(const char *text, int length);
	static inline void setText(const std::string &text)
	{ setText(text.data(), (int)text.length()); }

	static void addObserver(Observer *observer);
	static void removeObserver(Observer *observer);
	static void startObserver();
	static void stopObserver();
};

#endif
