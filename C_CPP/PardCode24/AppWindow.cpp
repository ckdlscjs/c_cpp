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
#include "TimerSystem.h"
#include "CollisionSystem.h"

AppWindow::AppWindow()
{
	std::cout << "Initialize : " << "AppWindow" << " Class" << '\n';
	_TimerSystem;
	_InputSystem;
	_ResourceSystem;
	_CameraSystem;
	_CollisionSystem;
	_RenderSystem;
	_ImguiSystem;
	_LightSystem;
}

AppWindow::~AppWindow()
{
	std::cout << "Release : " << "AppWindow" << " Class" << '\n';
}

void AppWindow::OnCreate()
{
	std::cout << "OnCreate" << '\n';
	_TimerSystem.Init();
	_InputSystem.Init();
	_ResourceSystem.Init();
	_CollisionSystem.Init();
	_RenderSystem.Init(m_hWnd, m_iWidth, m_iHeight);
	_ImguiSystem.Init(m_hWnd, _RenderSystem.GetD3DDevice(), _RenderSystem.GetD3DDeviceContext());
	_CameraSystem.Init();
	_LightSystem.Init();

	_InputSystem.SetMouseCenter(m_hWnd);

	//카메라 기본세팅
	_CameraSystem.AddCamera(new FirstPersonCamera());
	_CameraSystem.GetCamera(0)->SetPosition({ 100, 200.0f, -1000.0f });
	_CameraSystem.GetCamera(0)->SetAsepectRatio((float)m_iWidth / (float)m_iHeight);
	_CameraSystem.GetCamera(0)->SetFOV(75.0f);
	_CameraSystem.GetCamera(0)->SetClipPlanes(0.1f, 50000.0f);
	_CameraSystem.GetCamera(0)->SetTarget(Vector3(0, 0, 0));

	//라이팅 기본세팅
	DirectionalLight* pLight_Directional = new DirectionalLight();
	pLight_Directional->m_mAmbient = Vector4(0.3f, 0.3f, 0.3f, 1.0f);
	pLight_Directional->m_mDiffuse = Vector4(0.7f, 0.7f, 0.7f, 1.0f);
	pLight_Directional->m_mSpecular = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	pLight_Directional->m_vDirection = (Vector3(0.0f, 0.0f, 0.0f) - _CameraSystem.GetCamera(0)->GetPosition()).Normalize();
	pLight_Directional->m_fShiness = 100.0f;
	_LightSystem.AddLight(pLight_Directional);

	PointLight* pLight_Point = new PointLight();
	pLight_Point->m_mAmbient = Vector4(0.2f, 0.2f, 0.2f, 1.0f);
	pLight_Point->m_mDiffuse = Vector4(0.7f, 0.7f, 0.7f, 1.0f);
	pLight_Point->m_mSpecular = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	pLight_Point->m_vPosition = Vector3(0.0f, 400.0f, 400.0f);
	pLight_Point->m_fShiness = 100.0f;
	pLight_Point->m_fAtt_a0 = 0.0f;
	pLight_Point->m_fAtt_a1 = 0.5f;
	pLight_Point->m_fAtt_a2 = 0.5f;
	pLight_Point->m_fRange = 1000.0f;
	_LightSystem.AddLight(pLight_Point);
	
	SpotLight* pLight_Spot = new SpotLight();
	pLight_Spot->m_mAmbient = Vector4(0.3f, 0.3f, 0.3f, 1.0f);
	pLight_Spot->m_mDiffuse = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
	pLight_Spot->m_mSpecular = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
	pLight_Spot->m_vDirection = (Vector3(0.0f, 0.0f, 0.0f) - Vector3(-250.0f, 500.0f, -5.0f)).Normalize();
	pLight_Spot->m_vPosition = Vector3(-250.0f, 500.0f, -5.0f);
	pLight_Spot->m_fShiness = 100.0f;
	pLight_Spot->m_fAtt_a0 = 0.0f;
	pLight_Spot->m_fAtt_a1 = 0.0f;
	pLight_Spot->m_fAtt_a2 = 1.0f;
	pLight_Spot->m_fRange = 5000.0f;
	pLight_Spot->m_fSpot = 95.0f;
	pLight_Spot->m_fCos_OuterCone = cosf(_DEGTORAD(130.0f));
	pLight_Spot->m_fCos_InnerCone = cosf(_DEGTORAD(10.0f));
	_LightSystem.AddLight(pLight_Spot);
	
	//SkySphere
	TempObj* SkySphere = new TempObj();
	SkySphere->m_vScale = Vector3(_CameraSystem.GetCamera(0)->GetFarZ()*0.9f, _CameraSystem.GetCamera(0)->GetFarZ()*0.9f, _CameraSystem.GetCamera(0)->GetFarZ()*0.9f);

	size_t hash_material_skysphere = _RenderSystem.CreateMaterial(L"Mat_SkySphere", L"VertexShaderPTN.hlsl", L"PSSkySphere.hlsl");
	std::vector<pTX_HASH> txs_skysphere;
	txs_skysphere.push_back({E_Textures::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/butter4.webp", WIC_FLAGS_NONE)});
	_RenderSystem.Material_SetTextures(hash_material_skysphere, &txs_skysphere);
	size_t hash_mesh_skysphere = _RenderSystem.CreateMesh(L"../Assets/Meshes/sphere.obj");
	SkySphere->m_Mesh_Material.push_back({ hash_mesh_skysphere , hash_material_skysphere });
	_RenderSystem.SkyObj = SkySphere;

	// 1. 난수 시드(seed)를 설정합니다.
	// std::random_device는 하드웨어 기반의 비결정적 난수를 제공하여
	// 매번 다른 시드를 얻을 수 있습니다.
	std::random_device rd;

	// 2. mt19937 난수 생성 엔진을 초기화합니다.
	// rd()를 시드로 사용하여 매번 다른 난수 시퀀스를 생성하도록 합니다.
	std::mt19937 gen(rd());

	// 3. 난수 분포(distribution)를 정의합니다.
	// 여기서는 1부터 100까지의 균등한 정수 난수를 생성하도록 설정합니다.
	std::uniform_int_distribution<int> dis(-100, 100);

	/*for (int i = 0; i < 50; i++)
	{
		_RenderSystem.objs.push_back(new TempObj());
		TempObj* obj = _RenderSystem.objs.back();
		obj->m_vScale = Vector3(300.0f, 300.0f, 300.0f);
		obj->m_vRotate = Vector3(0.0f, 180.0f, 0.0f);
		obj->m_vPosition = Vector3(dis(gen)*30.0f, dis(gen)*30.0f, dis(gen)*30.0f);

		obj->m_hashMeshes.push_back(_RenderSystem.CreateMesh(i % 3 ? L"../Assets/Meshes/sphere.obj" : L"../Assets/Meshes/cube.obj", Colliders::SPHERE));
		obj->m_hashTextures.push_back(_RenderSystem.CreateTexture(i % 2 ? L"../Assets/Textures/butter.dds" : L"../Assets/Textures/butter3.webp", WIC_FLAGS_NONE));
		obj->m_IdxVS = hash_vs_0;
		obj->m_IdxPS = hash_ps_1;

		obj->m_IdxCBs.push_back(hash_cb_dl);
		obj->m_IdxCBs.push_back(hash_cb_pl);
		obj->m_IdxCBs.push_back(hash_cb_sl);

		obj->m_IdxCBs.push_back(hash_cb_wvpitmat);
		obj->m_IdxCBs.push_back(hash_cb_cbtime);
		obj->m_IdxCBs.push_back(hash_cb_campos);
	}*/
	
	//Pard22
	_RenderSystem.objs.push_back(new TempObj());
	TempObj* obj = _RenderSystem.objs.back();
	obj->m_vScale = Vector3(300.0f, 300.0f, 300.0f);
	obj->m_vRotate = Vector3(0.0f, 0.0f, 0.0f);
	obj->m_vPosition = Vector3(0.0f, 600.0f, 0.0f);

	size_t hash_mesh_obj0 = _RenderSystem.CreateMesh(L"../Assets/Meshes/sphere.obj", E_Colliders::SPHERE);
	size_t hash_material_0 = _RenderSystem.CreateMaterial(L"Mat_Earth", L"VertexShaderPTN.hlsl", L"PS_TextureFlow.hlsl");
	std::vector<pTX_HASH> txs_obj0;
	txs_obj0.push_back({ E_Textures::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/PardCode22/earth_color.jpg", WIC_FLAGS_NONE)});
	txs_obj0.push_back({ E_Textures::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/PardCode22/earth_night.jpg", WIC_FLAGS_NONE) });
	txs_obj0.push_back({ E_Textures::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/PardCode22/earth_clouds.jpg", WIC_FLAGS_NONE) });
	txs_obj0.push_back({ E_Textures::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/PardCode22/earth_spec.jpg", WIC_FLAGS_NONE) });
	_RenderSystem.Material_SetTextures(hash_material_0, &txs_obj0);
	obj->m_Mesh_Material.push_back({ hash_mesh_obj0 , hash_material_0 });

	//Pard23
	_RenderSystem.objs.push_back(new TempObj());
	obj = _RenderSystem.objs.back();
	obj->m_vScale = Vector3(300.0f, 300.0f, 300.0f);
	obj->m_vRotate = Vector3(0.0f, 0.0f, 0.0f);
	obj->m_vPosition = Vector3(0.0f, 0.0f, 0.0f);

	size_t hash_mesh_obj1 = _RenderSystem.CreateMesh(L"../Assets/Meshes/scene.obj");
	size_t hash_material_1 = _RenderSystem.CreateMaterial(L"Mat_PointLight", L"VertexShaderPTN.hlsl", L"PS_RotatePointLight.hlsl");
	std::vector<pTX_HASH> txs_obj1;
	txs_obj1.push_back({ E_Textures::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/wall.jpg", WIC_FLAGS_NONE) });
	_RenderSystem.Material_SetTextures(hash_material_1, &txs_obj1);
	obj->m_Mesh_Material.push_back({ hash_mesh_obj1 , hash_material_1 });
}

void AppWindow::OnUpdate()
{
	std::cout << "OnUpdate" << '\n';
	_TimerSystem.Frame();
	std::cout << "ElapsedTime : " << _TimerSystem.GetElapsedTime() << '\n';
	std::cout << "FPS : " << _TimerSystem.GetFps() << '\n';
	_InputSystem.Frame();
	_CameraSystem.Frame(_TimerSystem.GetDeltaTime());
	_CollisionSystem.Frame(_TimerSystem.GetDeltaTime());
	_ImguiSystem.Frame();
	_RenderSystem.Frame(_TimerSystem.GetDeltaTime());

	_RenderSystem.PreRender();
	_RenderSystem.Render(_TimerSystem.GetElapsedTime());
	_ImguiSystem.Render();
	_RenderSystem.PostRender();
	std::cout << '\n';
}

void AppWindow::OnDestroy()
{
	std::cout << "OnDestroy" << '\n';
	m_bIsRun = false;
	_CollisionSystem.Release();
	_ResourceSystem.Release();
	_CameraSystem.Release();
	_RenderSystem.Release();
	_InputSystem.Release();
	_ImguiSystem.Release();
	_LightSystem.Release();
}
