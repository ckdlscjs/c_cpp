#pragma once
#include "AppWindow.h"
#define _CRTDBG_MAP_ALLOC

AppWindow* app;

BOOL WINAPI ConsoleHandler(DWORD signal)
{
	if (signal == CTRL_CLOSE_EVENT)
	{
		if (app)
		{
			MessageBoxW(app->GetHwnd(), L"비정상종료, 메모리정리중", NULL, MB_OK);
			delete app;
			app = nullptr;
		}
		return TRUE;
	}
	return FALSE;
}

int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	SetConsoleCtrlHandler(ConsoleHandler, TRUE);

#undef new
	app = new AppWindow(); // 정상 동작
#define new new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )

	if (app->Init())
	{
		while (app->IsRun())
		{
			app->BroadCast();
		}
	}

	delete app;
	app = nullptr;

	return 0;
}