// ConsoleApplication1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include <tchar.h>
#include <vector>

class WindowObject {
public:
	LPTSTR winTitle;
	HWND hwnd;
	WindowObject(LPTSTR, HWND);
};

WindowObject::WindowObject(LPTSTR string, HWND hwnd){
	this->winTitle = string;
	this->hwnd = hwnd;
}

typedef std::vector<WindowObject> WindowList;

WindowList windowTitles;

BOOL CALLBACK EnumerateWindowsCallback( HWND hwnd, LPARAM lParam){
	int titleLength = GetWindowTextLength(hwnd);
	if(!titleLength){
		return true;
	}
	titleLength += 1; // For null character

	LPTSTR buff = new TCHAR[titleLength];
	int length = GetWindowText(hwnd, buff, titleLength);
	if(!length){
		delete buff;
		_tprintf(_T("%x failed because %d\n"), hwnd, GetLastError());
		return true;
	}
	WindowObject* myWindowObject = new WindowObject(buff, hwnd);
	windowTitles.push_back(*myWindowObject);
	return true;
}

HWND GetWoWHWND(){

	EnumWindows(&EnumerateWindowsCallback, 0);
	//_tprintf(_T("Printing %d titles\n"), windowTitles.size());
	for(WindowList::iterator iter = windowTitles.begin(); iter != windowTitles.end(); ++iter){
		WindowObject windowObject = *iter;
		LPTSTR windowTitle = windowObject.winTitle;
		if(_tcscmp(windowTitle, _T("World of Warcraft")) == 0){
			return windowObject.hwnd;
		}
	}
	return 0;
}

typedef DWORD (__cdecl *MYPROC)(double, HWND);
typedef double (__cdecl *MYPROC2)(HWND, DWORD);

void myMain(){
	
	HWND hwnd = GetWoWHWND();
	if(!hwnd){
		_tprintf(_T("Failed to find window"));
		return;
	}

	HMODULE potato = LoadLibrary(_T("potato.dll"));
	if(!potato){
		_tprintf(_T("Failed to load module"));
		return;
	}

	MYPROC searchForDouble = (MYPROC)GetProcAddress(potato, "searchForDouble");
	if(!searchForDouble){
		_tprintf(_T("Failed to get function searchForDouble"));
		return;
	}

	MYPROC2 getDoubleAtAddress = (MYPROC2)GetProcAddress(potato, "getDoubleAtAddress");
	if(!getDoubleAtAddress){
		_tprintf(_T("Failed to get function getDoubleAtAddress"));
		return;
	}

	DWORD value = searchForDouble(571565, hwnd);

	_tprintf(_T("Found at %p value %f\n"), value, getDoubleAtAddress(hwnd, value));

	value = searchForDouble(414000 + 804, hwnd);

	_tprintf(_T("Found at %p value %f\n"), value, getDoubleAtAddress(hwnd, value));

	
	_tprintf(_T("Found at %p value %f\n"), value+8, getDoubleAtAddress(hwnd, value+8));
}

int _tmain(int argc, _TCHAR* argv[])
{
	myMain();
	getchar();
	return 0;
}

