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
	bool IsRun();
	HWND GetHwnd() const;
	RECT GetClientWindowRect();
	/*WindowEvent*/
	virtual void OnCreate() = 0;
	virtual void OnUpdate() = 0;
	virtual void OnDestroy() = 0;

protected:
	UINT m_iWidth;
	UINT m_iHeight;
	HWND m_hWnd;
	bool m_bIsRun;
};
//실제정의->cpp
extern Window* g_pWindow;
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);