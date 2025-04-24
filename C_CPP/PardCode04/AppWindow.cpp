#include "AppWindow.h"
#include "GraphicEngine.h"
#include "SwapChain.h"
#include "DeviceContext.h"
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
	std::cout << "OnCreate" << '\n';
	GraphicEngine::Get().Init(g_pWindow);
}

void AppWindow::OnUpdate()
{
	std::cout << "OnUpdate" << '\n';
	GraphicEngine::Get().m_pCDeviceContext->ClearRenderTargetColor(GraphicEngine::Get().m_pCSwapChain, 0.5, 0.6, 0.7, 1.0);
	GraphicEngine::Get().m_pCSwapChain->Present(false);
}

void AppWindow::OnDestroy()
{
	std::cout << "OnDestroy" << '\n';
	//GraphicEngine::Get().Release();
	m_bIsRun = false;
}
