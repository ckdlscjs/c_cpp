#pragma once
#include "AppWindow.h"
#define _CRTDBG_MAP_ALLOC

//AppWindow* app;
std::unique_ptr<AppWindow> unique_app;
BOOL WINAPI ConsoleHandler(DWORD signal)
{
	if (signal == CTRL_CLOSE_EVENT)
	{
		if (unique_app)
		{
			MessageBoxW(unique_app->GetHwnd(), L"비정상종료, 메모리정리중", NULL, MB_OK);
			unique_app.reset();
		}
		return TRUE;
	}
	return FALSE;
}

int main()
{
	//메모리 릭 체크
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	SetConsoleCtrlHandler(ConsoleHandler, TRUE);

#undef new
	unique_app = std::make_unique<AppWindow>(); // 정상 동작
#define new new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )

	if (unique_app && unique_app->Init())
	{
		while (unique_app && unique_app->IsRun())
		{
			unique_app->BroadCast();
		}
	}

	return 0;
}