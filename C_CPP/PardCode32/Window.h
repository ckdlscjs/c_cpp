#pragma once
#include "CommonHeader.h"

class Window
{
public:
	Window();
	virtual ~Window();
public:
	bool Init(UINT width = 1024, UINT height = 768);
	bool BroadCast();
	bool IsRun();
	HWND GetHwnd() const;
	RECT GetClientWindowRect();
	/*WindowEvent*/
	virtual void OnCreate() = 0;
	virtual void OnUpdate() = 0;
	virtual void OnDestroy() = 0;

protected:
	UINT g_iWidth;
	UINT g_iHeight;
	HWND m_hWnd;
	bool m_bIsRun;
};
//실제정의->cpp
extern Window* g_pWindow;
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);