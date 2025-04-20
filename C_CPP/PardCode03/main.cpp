#pragma once
#include "AppWindow.h"
#define _CRTDBG_MAP_ALLOC
#ifdef _DEBUG
#define new new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#endif


int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	AppWindow app;
	if (app.Init())
	{
		while (app.IsRun())
		{
			app.BroadCast();
		}
	}
	return 0;
}