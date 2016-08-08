#define WIN32_LEAN_AND_MEAN
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#endif

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <string.h>
#include <windows.h>


#define X 123
#define Y 123
#define SCREEN_WIDTH 1366
#define SCREEN_HEIGHT 768


void MouseSetup(INPUT *buffer)
{
    buffer->type = INPUT_MOUSE;
    buffer->mi.dx = (0 * (0xFFFF / SCREEN_WIDTH));
    buffer->mi.dy = (0 * (0xFFFF / SCREEN_HEIGHT));
    buffer->mi.mouseData = 0;
    buffer->mi.dwFlags = MOUSEEVENTF_ABSOLUTE;
    buffer->mi.time = 0;
    buffer->mi.dwExtraInfo = 0;
}


void MouseMoveAbsolute(INPUT *buffer, int x, int y)
{
    buffer->mi.dx = (x * (0xFFFF / SCREEN_WIDTH));
    buffer->mi.dy = (y * (0xFFFF / SCREEN_HEIGHT));
    buffer->mi.dwFlags = (MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE);

    SendInput(1, buffer, sizeof(INPUT));
}


void MouseClick(INPUT *buffer)
{
    buffer->mi.dwFlags = (MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTDOWN);
    SendInput(1, buffer, sizeof(INPUT));

    Sleep(10);

    buffer->mi.dwFlags = (MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTUP);
    SendInput(1, buffer, sizeof(INPUT));
}

void KeyPress()
{
    INPUT inp;
    inp.type = INPUT_KEYBOARD;
    inp.ki.wVk = 0x41;  // 'A'
    inp.ki.wScan = 0;
    inp.ki.dwFlags = 0;
    inp.ki.time = 0;
    inp.ki.dwExtraInfo = 0;

    SendInput(1, &inp, sizeof(INPUT));


    inp.ki.dwFlags = KEYEVENTF_KEYUP;

    SendInput(1, &inp, sizeof(INPUT));
}

int main(int argc, char *argv[])
{
    INPUT buffer[1];

    Sleep(3000);
    MouseSetup(buffer);

    MouseMoveAbsolute(buffer, X, Y);
    MouseClick(buffer);

    Sleep(1000);

    KeyPress();

    Sleep(1000);
    return 0;
}
