#include "AppWindow.h"
#include "GraphicEngine.h"

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
	GraphicEngine::Get().Init();
}

void AppWindow::OnUpdate()
{
}

void AppWindow::OnDestroy()
{
	GraphicEngine::Get().Release();
	mIsRun = false;
}
