#include "AppWindow.h"
#include "RenderSystem.h"
#include "InputSystem.h"
#include "TempObj.h"			//임시오브젝트
#include "CameraSystem.h"
#include "FirstPersonCamera.h"
#include "ResourceSystem.h"
#include "ImguiSystem.h"


AppWindow::AppWindow()
{
	std::cout << "Initialize : " << "AppWindow" << " Class" << '\n';
	_InputSystem;
	_ResourceSystem;
	_RenderSystem;
	_ImguiSystem;
	_CameraSystem;
}

AppWindow::~AppWindow()
{
	std::cout << "Release : " << "AppWindow" << " Class" << '\n';
}

void AppWindow::OnCreate()
{
	std::cout << "OnCreate" << '\n';
	_InputSystem.Init();
	_ResourceSystem.Init();
	_RenderSystem.Init(m_hWnd, m_iWidth, m_iHeight);
	_ImguiSystem.Init(m_hWnd, _RenderSystem.GetD3DDevice(), _RenderSystem.GetD3DDeviceContext());
	_CameraSystem.Init();

	_InputSystem.SetMouseCenter(m_hWnd);
	_CameraSystem.AddCamera(new FirstPersonCamera());
	_RenderSystem.objs.push_back(new TempObj());
	TempObj* obj = _RenderSystem.objs.back();

	obj->m_IdxMHs.push_back(_RenderSystem.CreateMesh(L"../Assets/Meshes/teapot.obj"));
	obj->m_IdxTX = _RenderSystem.CreateTexture(L"../Assets/Textures/brick.png", Samplers::WRAP_LINEAR);
	Constant_wvp cc0;
	obj->m_IdxCBs.push_back(_RenderSystem.CreateConstantBuffer(&cc0, sizeof(Constant_wvp)));
	Constant_time cc1;
	obj->m_IdxCBs.push_back(_RenderSystem.CreateConstantBuffer(&cc1, sizeof(Constant_time)));
	obj->m_vPosition = Vector3(-200, -200.0f, 100.0f);
	obj->m_vScale = Vector3(200.0f, 200.0f, 200.0f);
	_CameraSystem.GetCamera(0)->SetPosition({ 100, -200, -1000.0f });
	_CameraSystem.GetCamera(0)->SetTarget(obj->m_vPosition);
}

void AppWindow::OnUpdate()
{
	std::cout << "OnUpdate" << '\n';
	_InputSystem.Frame();
	_ImguiSystem.Frame();
	_CameraSystem.Frame(0.033f);
	_RenderSystem.Frame();

	_RenderSystem.PreRender();
	_RenderSystem.Render();
	_ImguiSystem.Render();
	_RenderSystem.PostRender();
	std::cout << '\n';
}

void AppWindow::OnDestroy()
{
	std::cout << "OnDestroy" << '\n';
	m_bIsRun = false;
	_ResourceSystem.Release();
	_CameraSystem.Release();
	_RenderSystem.Release();
	_InputSystem.Release();
	_ImguiSystem.Release();
}
