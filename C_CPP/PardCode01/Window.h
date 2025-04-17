#pragma once
#include <Windows.h>
#include <string>
#include <iostream>

class Window
{
public:
	Window();
	virtual ~Window();
public:
	bool Init();
	bool BroadCast();
	bool Release();
	bool IsRun();
	virtual void OnCreate() = 0;
	virtual void OnUpdate() = 0;
	virtual void OnDestroy() = 0;

protected:
	HWND mHwnd;
	bool mIsRun;
};
//실제정의->cpp
extern Window* gWindow;
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);