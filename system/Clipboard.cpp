#include <cstring>
#include "common/Log.hpp"
#include "pattern/Runnable.hpp"

#ifndef WIN32
#include <gtk/gtk.h>
#endif


class ClipboardObserveeHandler : public Runnable
{
#ifdef WIN32
	HWND hwnd;
#endif
public:
#ifdef WIN32
	ClipboardObserveeHandler(): hwnd(0) {}
#endif
	void stop();
	virtual void run();
};


class ClipboardObservee : public Observee
{
public:
	ClipboardObservee() : handler(nullptr) {}
	void start();
	void stop();
private:
	ClipboardObserveeHandler *handler;
};


static ClipboardObservee *clipboardObservee = nullptr;
#ifndef WIN32
static GtkClipboard *cb = nullptr;
#endif


#ifndef WIN32
void gtkInitialize()
{
	static bool isGtkInit = false;

	if (isGtkInit == false)
	{
		::gtk_init(nullptr, nullptr);
		isGtkInit = true;
	}
}
#endif


std::string Clipboard::getText()
{
	std::string text;

#ifdef WIN32
	if (::IsClipboardFormatAvailable(CF_TEXT) == TRUE)
	{
		::OpenClipboard(nullptr);
		HGLOBAL hglb = ::GetClipboardData(CF_TEXT);
		char *ptr = (char *)GlobalLock(hglb);
		text.assign(ptr);
		::GlobalUnlock(hglb);
		::CloseClipboard();
	}
#else
	gtkInitialize();

	if (::cb == nullptr)
		::cb = ::gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);

	if (::gtk_clipboard_wait_is_text_available(::cb) == TRUE)
	{
		char *ptr = ::gtk_clipboard_wait_for_text(::cb);
		text.assign(ptr);
	}
#endif

	return text;
}


void Clipboard::setText(const char *text)
{
	Clipboard::setText(text, (int)::strlen(text));
}


void Clipboard::setText(const std::string &text)
{
	Clipboard::setText(text.data(), (int)text.length());
}


void Clipboard::setText(const char *text, int length)
{
#ifdef WIN32
	::OpenClipboard(nullptr);
	::EmptyClipboard();
	HGLOBAL hMem = ::GlobalAlloc(GMEM_MOVEABLE, (SIZE_T)length);
	HGLOBAL hglb = ::GlobalLock(hMem);
	::memcpy(hglb, text, (size_t)length);
	::GlobalUnlock(hMem);
	::SetClipboardData(CF_TEXT, hMem);
	::CloseClipboard();
#else
	gtkInitialize();

	if (::cb == nullptr)
		::cb = ::gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);

	::gtk_clipboard_set_text(cb, text, (gint)length);
	::gtk_clipboard_store(cb);
#endif
}


void Clipboard::addObserver(Observer *observer)
{
	if (::clipboardObservee == nullptr)
		::clipboardObservee = new ClipboardObservee;

	::clipboardObservee->add(observer);
}


void Clipboard::removeObserver(Observer *observer)
{
	if (::clipboardObservee != nullptr)
		::clipboardObservee->remove(observer);
}


void Clipboard::startObserver()
{
	if (::clipboardObservee != nullptr)
		::clipboardObservee->start();
}


void Clipboard::stopObserver()
{
	if (::clipboardObservee != nullptr)
		::clipboardObservee->stop();
}


void ClipboardObservee::start()
{
	if (this->handler == nullptr)
	{
		this->handler = new ClipboardObserveeHandler;
		this->handler->start();
	}
}


void ClipboardObservee::stop()
{
	if (this->handler != nullptr)
	{
		this->handler->stop();
		delete this->handler;
		this->handler = nullptr;
	}
}


#ifdef WIN32
LRESULT APIENTRY MsgWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static HWND hwndNextViewer;

	switch (uMsg)
	{
		case WM_CREATE:
			hwndNextViewer = SetClipboardViewer(hwnd);
			break;
		case WM_CHANGECBCHAIN:
			if ((HWND) wParam == hwndNextViewer)
				hwndNextViewer = (HWND)lParam;
			else if (hwndNextViewer != nullptr)
				SendMessage(hwndNextViewer, uMsg, wParam, lParam);
			break;
		case WM_DESTROY:
			ChangeClipboardChain(hwnd, hwndNextViewer);
			PostQuitMessage(0);
			break;
		case WM_DRAWCLIPBOARD:
			if (clipboardObservee != nullptr)
				clipboardObservee->notify();
			SendMessage(hwndNextViewer, uMsg, wParam, lParam);
			break;
		default:
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}

	return (LRESULT)0;
}

#else

void gtkOwnerChangeHandle(GtkClipboard *, GdkEvent *, gpointer)
{
	if (::clipboardObservee != nullptr)
		clipboardObservee->notify();
}

#endif


void ClipboardObserveeHandler::stop()
{
#ifdef WIN32
	if (this->hwnd != 0)
	{
		DestroyWindow(this->hwnd);
		this->hwnd = 0;
	}
#else
	::gtk_main_quit();
#endif
}


void ClipboardObserveeHandler::run()
{
#ifdef WIN32
	static const char clipboardCaptureName[] = "captureClass";
	HINSTANCE instance = GetModuleHandle(nullptr);
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = MsgWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = instance;
	wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = clipboardCaptureName;
	wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);

	if (RegisterClassEx(&wc) == 0)
	{
		LOG.d(TRACE, "Window Registration Failed!");
		return;
	}

	this->hwnd = CreateWindowEx(0, clipboardCaptureName,
		"w", 0, 0, 0, 0, 0, HWND_MESSAGE, nullptr, instance, nullptr);

	MSG msg;
	while (GetMessage(&msg, this->hwnd, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
#else
	gtkInitialize();

	if (::cb == nullptr)
		::cb = ::gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);

	::g_signal_connect(::cb, "owner-change", G_CALLBACK(gtkOwnerChangeHandle), NULL);
	::gtk_main();
#endif
}
