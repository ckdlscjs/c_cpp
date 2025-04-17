#include "AppWindow.h"


AppWindow::AppWindow()
{
	std::cout << "AppWindow" << " Class" << " 생성자 호출" << '\n';
}

AppWindow::~AppWindow()
{
	std::cout << "AppWindow" << " Class" << " 소멸자 호출" << '\n';
}

void AppWindow::OnCreate()
{
}

void AppWindow::OnUpdate()
{
}

void AppWindow::OnDestroy()
{
	mIsRun = false;
}
