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
	GraphicEngine::Get().Init(g_pWindow);
}

void AppWindow::OnUpdate()
{
}

void AppWindow::OnDestroy()
{
	GraphicEngine::Get().Release();
	m_bIsRun = false;
}
