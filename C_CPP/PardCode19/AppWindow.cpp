#include "AppWindow.h"
#include "RenderSystem.h"
#include "InputSystem.h"
#include "TempObj.h"			//임시오브젝트
#include "CameraSystem.h"
#include "FirstPersonCamera.h"
#include "ResourceSystem.h"
#include "ImguiSystem.h"
#include "LightSystem.h"
#include "Light.h"
AppWindow::AppWindow()
{
	std::cout << "Initialize : " << "AppWindow" << " Class" << '\n';
	_InputSystem;
	_ResourceSystem;
	_RenderSystem;
	_ImguiSystem;
	_CameraSystem;
	_LightSystem;
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
	_LightSystem.Init();

	_InputSystem.SetMouseCenter(m_hWnd);
	_CameraSystem.AddCamera(new FirstPersonCamera());
	_CameraSystem.GetCamera(0)->SetPosition({ 100, 200.0f, -1000.0f });
	_RenderSystem.objs.push_back(new TempObj());
	TempObj* obj = _RenderSystem.objs.back();
	obj->m_vScale = Vector3(1000.0f, 1000.0f, 1000.0f);
	obj->m_vRotate = Vector3(0.0f, 180.0f, 0.0f);
	obj->m_vPosition = Vector3(0.0f, 0.0f, 0.0f);
	_CameraSystem.GetCamera(0)->SetTarget(obj->m_vPosition);

	obj->m_hashMeshes.push_back(_RenderSystem.CreateMesh(L"../Assets/Meshes/teapot.obj"));
	obj->m_hashTextures.push_back(_RenderSystem.CreateTexture(L"../Assets/Textures/brick.png", WIC_FLAGS_NONE));

	DirectionalLight* pLight_Directional = new DirectionalLight();
	pLight_Directional->m_mAmbient = Vector4(0.1f, 0.1f, 0.1f, 1.0f);
	pLight_Directional->m_mDiffuse = Vector4(0.8f, 0.3f, 0.2f, 1.0f);
	pLight_Directional->m_mSpecular = Vector4(0.7f, 0.7f, 0.7f, 1.0f);
	auto campos = _CameraSystem.GetCamera(0)->GetPosition();
	auto objpos = obj->m_vPosition;
	pLight_Directional->m_vDirection = (Position(0.0f, 0.0f, 0.0f) - Position(0.0f, 5.0f, -5.0f)).Normalize();
	pLight_Directional->m_fShiness = 100.0f;
	_LightSystem.AddLight(pLight_Directional);
	obj->m_IdxCBs.push_back(_RenderSystem.CreateConstantBuffer(sizeof(CB_DirectionalLight)));

	PointLight* pLight_Point = new PointLight();
	pLight_Point->m_mAmbient = Vector4(0.2f, 0.2f, 0.2f, 1.0f);
	pLight_Point->m_mDiffuse = Vector4(0.0f, 0.0f, 1.0f, 1.0f);
	pLight_Point->m_mSpecular = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	pLight_Point->m_vPosition = Position(-500.0f, 500.0f, 0.0f);
	pLight_Point->m_fShiness = 50.0f;
	pLight_Point->m_fAtt_a0 = 0.10f;
	pLight_Point->m_fAtt_a1 = 0.0f;
	pLight_Point->m_fAtt_a2 = 0.0f;
	pLight_Point->m_fRange = 300000;
	_LightSystem.AddLight(pLight_Point);
	obj->m_IdxCBs.push_back(_RenderSystem.CreateConstantBuffer(sizeof(CB_PointLight)));

	SpotLight* pLight_Spot = new SpotLight();
	pLight_Spot->m_mAmbient = Vector4(0.3f, 0.3f, 0.3f, 1.0f);
	pLight_Spot->m_mDiffuse = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
	pLight_Spot->m_mSpecular = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
	pLight_Spot->m_vDirection = (Position(0.0f, 0.0f, 0.0f) - Position(-250.0f, 500.0f, -5.0f)).Normalize();
	pLight_Spot->m_vPosition = Position(-250.0f, 500.0f, -5.0f);
	pLight_Spot->m_fShiness = 1000.0f;
	pLight_Spot->m_fAtt_a0 = 0.0f;
	pLight_Spot->m_fAtt_a1 = 0.0f;
	pLight_Spot->m_fAtt_a2 = 1.0f;
	pLight_Spot->m_fRange = 2000.0f;
	pLight_Spot->m_fSpot = 95.0f;
	pLight_Spot->m_fCos_OuterCone = cosf(_DEGTORAD(75.0f));
	pLight_Spot->m_fCos_InnerCone = cosf(_DEGTORAD(10.0f));
	_LightSystem.AddLight(pLight_Spot);
	obj->m_IdxCBs.push_back(_RenderSystem.CreateConstantBuffer(sizeof(CB_SpotLight)));

	obj->m_IdxCBs.push_back(_RenderSystem.CreateConstantBuffer(sizeof(CB_WVPITMatrix)));
	obj->m_IdxCBs.push_back(_RenderSystem.CreateConstantBuffer(sizeof(Constant_time)));
	obj->m_IdxCBs.push_back(_RenderSystem.CreateConstantBuffer(sizeof(CB_Campos)));
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
	_LightSystem.Release();
}
