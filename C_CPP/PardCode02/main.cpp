#pragma once
#include "AppWindow.h"

int main()
{
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