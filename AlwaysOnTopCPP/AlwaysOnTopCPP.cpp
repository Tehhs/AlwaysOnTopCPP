// AlwaysOnTopCPP.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <stdio.h>
#include <tchar.h>
#include <Windows.h>
#include <iostream>
#include <thread>
#include <list>  


HHOOK hHook{ NULL };
bool ctrl_down = false;
std::list<HWND> windows;
enum Keys
{	
	CTRL = 162, 
	SPACE = 48
};

HWND focusHandle;

bool isWindowMaintained(HWND window) {
	focusHandle = GetForegroundWindow();

	for (HWND const& h : windows) {
		if (h == window) {
			return true;
		}
	}
	return false; 
}

void removeWindowFromList(HWND hwnd) {
	windows.remove(hwnd);
}

void maintainFocusedWindow() {
	HWND focusHandle = GetForegroundWindow();

	if (isWindowMaintained(focusHandle)) {
		std::cout << "UNDOING Window " << focusHandle << std::endl;
		removeWindowFromList(focusHandle); 
		SetWindowPos(focusHandle, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	}
	else {
		std::cout << "DOING Window " << focusHandle << std::endl;
		SetWindowPos(focusHandle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
		windows.push_back(focusHandle);
	}
	
}


LRESULT CALLBACK keyboard_hook(const int code, const WPARAM wParam, const LPARAM lParam) {
	if (wParam == WM_KEYUP) {
		KBDLLHOOKSTRUCT* kbdStruct = (KBDLLHOOKSTRUCT*)lParam;
		DWORD wVirtKey = kbdStruct->vkCode;
		DWORD wScanCode = kbdStruct->scanCode;

		//std::cout << "this works " << wVirtKey << std::endl;
			
		if (wVirtKey == Keys::CTRL) {
			ctrl_down = false;
		}
	}
	else if (wParam == WM_KEYDOWN) {
		KBDLLHOOKSTRUCT* kbdStruct = (KBDLLHOOKSTRUCT*)lParam;
		DWORD wVirtKey = kbdStruct->vkCode;
		DWORD wScanCode = kbdStruct->scanCode;

		//std::cout << "this works " << wVirtKey << std::endl;

		if (wVirtKey == Keys::CTRL) {
			ctrl_down = true;
		}
		if (wVirtKey == Keys::SPACE && ctrl_down) {
			bool yes = wVirtKey == Keys::SPACE;
			//std::cout << "doing the thing " << Keys::SPACE << " " << wVirtKey << " " << yes << std::endl;

			maintainFocusedWindow();
		}

	}


	return CallNextHookEx(hHook, code, wParam, lParam);
}

int main()
{
	hHook = SetWindowsHookEx(WH_KEYBOARD_LL, keyboard_hook, NULL, 0);
	if (hHook == NULL) {
		std::cout << "Keyboard hook failed!" << std::endl;
	}

	std::cout << "AlwaysOnTop running... Keep this window open for the program to run\n" << std::endl;
	std::cout << "Press CTRL+0 to toggle if a window should stay visible\n" << std::endl;
	std::cout << "This is a test version. Real version wont contain this console window" << std::endl;

	while (GetMessage(NULL, NULL, 0, 0));
	UnhookWindowsHookEx(hHook);
	return 0;
}