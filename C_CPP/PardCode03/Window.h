#pragma once
#include "std.h"

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
	void SetHwnd(HWND hWnd);
	HWND GetHwnd() const;
	RECT GetClientWindowRect();
	/*WindowEvent*/
	virtual void OnCreate() = 0;
	virtual void OnUpdate() = 0;
	virtual void OnDestroy() = 0;

protected:
	HWND mHwnd;
	bool mIsRun;
};
//실제정의->cpp
extern Window* g_pWindow;
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);