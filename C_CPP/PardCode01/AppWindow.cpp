#include "AppWindow.h"


AppWindow::AppWindow()
{
	std::cout << "AppWindow" << " Class" << " ������ ȣ��" << '\n';
}

AppWindow::~AppWindow()
{
	std::cout << "AppWindow" << " Class" << " �Ҹ��� ȣ��" << '\n';
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
