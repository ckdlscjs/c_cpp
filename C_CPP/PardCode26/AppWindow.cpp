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
#include "TestBlockMacro.h"
#include "Material.h"			//mtl텍스쳐로딩을위한 임시임포트

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

	_InputSystem.SetMouseCenter(m_hWnd);

	//카메라 기본세팅
	_CameraSystem.AddCamera(new FirstPersonCamera());
	_CameraSystem.GetCamera(0)->SetPosition({ 0.0f, 200.0f, -500.0f });
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
	pLight_Point->m_vPosition = Vector3(0.0f, 30.0f, 150.0f);
	pLight_Point->m_fShiness = 100.0f;
	pLight_Point->m_fAtt_a0 = 0.49f;
	pLight_Point->m_fAtt_a1 = 0.5f;
	pLight_Point->m_fAtt_a2 = 0.01f;
	pLight_Point->m_fRange = 500.0f;
	_LightSystem.AddLight(pLight_Point);
	
	SpotLight* pLight_Spot = new SpotLight();
	pLight_Spot->m_mAmbient = Vector4(0.3f, 0.3f, 0.3f, 1.0f);
	pLight_Spot->m_mDiffuse = Vector4(0.5f, 0.5f, 0.5f, 1.0f);
	pLight_Spot->m_mSpecular = Vector4(0.7f, 0.7f, 0.7f, 1.0f);
	pLight_Spot->m_vDirection = (Vector3(0.0f, 0.0f, 0.0f) - Vector3(-250.0f, 500.0f, -5.0f)).Normalize();
	pLight_Spot->m_vPosition = Vector3(-250.0f, 500.0f, -5.0f);
	pLight_Spot->m_fShiness = 100.0f;
	pLight_Spot->m_fAtt_a0 = 0.5f;
	pLight_Spot->m_fAtt_a1 = 0.5f;
	pLight_Spot->m_fAtt_a2 = 0.0f;
	pLight_Spot->m_fRange = 5000.0f;
	pLight_Spot->m_fSpot = 50.0f;
	pLight_Spot->m_fCos_OuterCone = cosf(_DEGTORAD(130.0f));
	pLight_Spot->m_fCos_InnerCone = cosf(_DEGTORAD(10.0f));
	_LightSystem.AddLight(pLight_Spot);

	
#ifndef _TESTBLOCK
	//textblock
	{
		//SkySphere
		TempObj* SkySphere = new TempObj();
		SkySphere->m_vScale = Vector3(_CameraSystem.GetCamera(0)->GetFarZ() * 0.9f, _CameraSystem.GetCamera(0)->GetFarZ() * 0.9f, _CameraSystem.GetCamera(0)->GetFarZ() * 0.9f);

		size_t hash_material_skysphere = _RenderSystem.CreateMaterial(L"Mat_SkySphere", L"VertexShaderPTN.hlsl", L"PSSkySphere.hlsl");
		std::vector<TX_HASH> txs_skysphere;
		txs_skysphere.push_back({ E_Textures::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/butter8.png", WIC_FLAGS_NONE) });
		_RenderSystem.Material_SetTextures(hash_material_skysphere, txs_skysphere);
		size_t hash_mesh_skysphere = _RenderSystem.CreateMesh(L"../Assets/Meshes/sphere.obj", E_Colliders::SPHERE);
		SkySphere->m_Mesh_Material.push_back({ hash_mesh_skysphere , hash_material_skysphere });
		_RenderSystem.SkyObj = SkySphere;

		
		/*for (int i = 0; i < 50; i++)
		{
			_RenderSystem.objs.push_back(new TempObj());
			TempObj* obj = _RenderSystem.objs.back();
			obj->m_vScale = Vector3(300.0f, 300.0f, 300.0f);
			obj->m_vRotate = Vector3(0.0f, 0.0f, 0.0f);
			obj->m_vPosition = Vector3(dis(gen) * 30.0f, dis(gen) * 30.0f, dis(gen) * 30.0f);

			size_t hash_mesh_rand = _RenderSystem.CreateMesh(i % 3 ? L"../Assets/Meshes/sphere.obj" : L"../Assets/Meshes/cube.obj", i % 3 ? E_Colliders::SPHERE : E_Colliders::AABB);
			size_t hash_meterial_rand = _RenderSystem.CreateMaterial(i % 3 ? L"Mat_Rand0" : L"Mat_Rand1", L"VertexShaderPTN.hlsl", L"PixelShaderPTN.hlsl");
			std::vector<TX_HASH> txs_objRand;
			txs_objRand.push_back({ E_Textures::Diffuse, _RenderSystem.CreateTexture(i % 2 ? L"../Assets/Textures/butter.dds" : L"../Assets/Textures/butter8.png", WIC_FLAGS_NONE) });
			_RenderSystem.Material_SetTextures(hash_meterial_rand, txs_objRand);
			obj->m_Mesh_Material.push_back({ hash_mesh_rand , hash_meterial_rand });
		}*/

		//Pard22
		_RenderSystem.objs.push_back(new TempObj());
		TempObj* obj = _RenderSystem.objs.back();
		obj->m_vScale = Vector3(300.0f, 300.0f, 300.0f);
		obj->m_vRotate = Vector3(0.0f, 0.0f, 0.0f);
		obj->m_vPosition = Vector3(700, 600.0f, -200);

		size_t hash_mesh_obj0 = _RenderSystem.CreateMesh(L"../Assets/Meshes/sphere.obj", E_Colliders::SPHERE);
		size_t hash_material_0 = _RenderSystem.CreateMaterial(L"Mat_Earth", L"VertexShaderPTN.hlsl", L"PS_TextureFlow.hlsl");
		std::vector<TX_HASH> txs_obj0;
		txs_obj0.push_back({ E_Textures::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/PardCode22/earth_color.jpg", WIC_FLAGS_NONE)});
		txs_obj0.push_back({ E_Textures::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/PardCode22/earth_night.jpg", WIC_FLAGS_NONE) });
		txs_obj0.push_back({ E_Textures::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/PardCode22/earth_clouds.jpg", WIC_FLAGS_NONE) });
		txs_obj0.push_back({ E_Textures::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/PardCode22/earth_spec.jpg", WIC_FLAGS_NONE) });
		_RenderSystem.Material_SetTextures(hash_material_0, txs_obj0);
		obj->m_Mesh_Material.push_back({ hash_mesh_obj0 , hash_material_0 });

		//Pard25
		_RenderSystem.objs.push_back(new TempObj());
		obj = _RenderSystem.objs.back();
		obj->m_vScale = Vector3(300.0f, 300.0f, 300.0f);
		obj->m_vRotate = Vector3(0.0f, 0.0f, 0.0f);
		obj->m_vPosition = Vector3(0.0f, 0.0f, 0.0f);

		size_t hash_mesh_obj1 = _RenderSystem.CreateMesh(L"../Assets/Meshes/house.obj");
		std::vector<std::wstring> VSs, PSs;
		VSs.push_back(L"VertexShaderPTN.hlsl"); VSs.push_back(L"VertexShaderPTN.hlsl"); VSs.push_back(L"VertexShaderPTN.hlsl"); VSs.push_back(L"VertexShaderPTN.hlsl");
		PSs.push_back(L"PixelShaderPTN.hlsl"); PSs.push_back(L"PixelShaderPTN.hlsl"); PSs.push_back(L"PixelShaderPTN.hlsl"); PSs.push_back(L"PixelShaderPTN.hlsl");
		std::vector<size_t> house_materials = _RenderSystem.CreateMaterials(L"../Assets/Meshes/house.mtl", VSs, PSs);
		std::vector<TX_HASH> txs_obj1_0;
		txs_obj1_0.push_back({ E_Textures::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/house_barrel.jpg", WIC_FLAGS_NONE) });
		_RenderSystem.Material_SetTextures(house_materials[0], txs_obj1_0);
		obj->m_Mesh_Material.push_back({ hash_mesh_obj1 , house_materials[0]});
		std::vector<TX_HASH> txs_obj1_1;
		txs_obj1_1.push_back({ E_Textures::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/house_brick.jpg", WIC_FLAGS_NONE) });
		_RenderSystem.Material_SetTextures(house_materials[1], txs_obj1_1);
		obj->m_Mesh_Material.push_back({ hash_mesh_obj1 , house_materials[1] });
		std::vector<TX_HASH> txs_obj1_2;
		txs_obj1_2.push_back({ E_Textures::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/house_windows.jpg", WIC_FLAGS_NONE) });
		_RenderSystem.Material_SetTextures(house_materials[2], txs_obj1_2);
		obj->m_Mesh_Material.push_back({ hash_mesh_obj1 , house_materials[2] });
		std::vector<TX_HASH> txs_obj1_3;
		txs_obj1_3.push_back({ E_Textures::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/house_wood.jpg", WIC_FLAGS_NONE) });
		_RenderSystem.Material_SetTextures(house_materials[3], txs_obj1_3);
		obj->m_Mesh_Material.push_back({ hash_mesh_obj1 , house_materials[3] });

		//nene
		_RenderSystem.objs.push_back(new TempObj());
		obj = _RenderSystem.objs.back();
		obj->m_vScale = Vector3(50.0f, 50.0f, 50.0f);
		obj->m_vRotate = Vector3(0.0f, 0.0f, 0.0f);
		obj->m_vPosition = Vector3(0.0f, 600.0f, 0.0f);

		size_t hash_mesh_nene = _RenderSystem.CreateMesh(L"../Assets/Meshes/nene.obj", E_Colliders::SPHERE);
		std::vector<std::wstring> neneVSs, nenePSs;
		neneVSs.push_back(L"VertexShaderPTN.hlsl"); neneVSs.push_back(L"VertexShaderPTN.hlsl"); neneVSs.push_back(L"VertexShaderPTN.hlsl");
		nenePSs.push_back(L"PixelShaderPTN.hlsl"); nenePSs.push_back(L"PixelShaderPTN.hlsl"); nenePSs.push_back(L"PixelShaderPTN.hlsl");
		std::vector<size_t> hash_materials_nene = _RenderSystem.CreateMaterials(L"../Assets/Meshes/nene.mtl", neneVSs, nenePSs);
		std::vector<TX_HASH> txs_nene_face;
		txs_nene_face.push_back({ E_Textures::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/face.dds", WIC_FLAGS_NONE)});
		_RenderSystem.Material_SetTextures(hash_materials_nene[0], txs_nene_face);
		obj->m_Mesh_Material.push_back({ hash_mesh_nene , hash_materials_nene[0] });
		std::vector<TX_HASH> txs_nene_body;
		txs_nene_body.push_back({ E_Textures::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/body.dds", WIC_FLAGS_NONE) });
		_RenderSystem.Material_SetTextures(hash_materials_nene[1], txs_nene_body);
		obj->m_Mesh_Material.push_back({ hash_mesh_nene , hash_materials_nene[1] });
		std::vector<TX_HASH> txs_nene_hair;
		txs_nene_hair.push_back({ E_Textures::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/hair.dds", WIC_FLAGS_NONE) });
		_RenderSystem.Material_SetTextures(hash_materials_nene[2], txs_nene_hair);
		obj->m_Mesh_Material.push_back({ hash_mesh_nene , hash_materials_nene[2] });

		//girltest
		_RenderSystem.objs.push_back(new TempObj());
		obj = _RenderSystem.objs.back();
		obj->m_vScale = Vector3(500.0f, 500.0f, 500.0f);
		obj->m_vRotate = Vector3(0.0f, 0.0f, 0.0f);
		obj->m_vPosition = Vector3(1500.0f, 600.0f, 0.0f);

		size_t hash_girl = _RenderSystem.CreateMesh(L"../Assets/Meshes/girl.obj", E_Colliders::SPHERE);
		std::vector<std::wstring> girlVSs, girlPSs;
		girlVSs.push_back(L"VertexShaderPTN.hlsl"); girlVSs.push_back(L"VertexShaderPTN.hlsl"); girlVSs.push_back(L"VertexShaderPTN.hlsl");
		girlVSs.push_back(L"VertexShaderPTN.hlsl"); girlVSs.push_back(L"VertexShaderPTN.hlsl"); girlVSs.push_back(L"VertexShaderPTN.hlsl"); girlVSs.push_back(L"VertexShaderPTN.hlsl");
		girlPSs.push_back(L"PixelShaderPTN.hlsl"); girlPSs.push_back(L"PixelShaderPTN.hlsl"); girlPSs.push_back(L"PixelShaderPTN.hlsl");
		girlPSs.push_back(L"PixelShaderPTN.hlsl"); girlPSs.push_back(L"PixelShaderPTN.hlsl"); girlPSs.push_back(L"PixelShaderPTN.hlsl"); girlPSs.push_back(L"PixelShaderPTN.hlsl");
		std::vector<size_t> hash_materials_girl = _RenderSystem.CreateMaterials(L"../Assets/Meshes/girl.mtl", girlVSs, girlPSs);

		auto pMat = _ResourceSystem.GetResource<Material>(hash_materials_girl[0]);
		std::vector<TX_HASH> txs_bot[7];
		txs_bot[0].push_back({ E_Textures::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/" + _tomw(pMat->GetTexturesPaths()[0][0]), WIC_FLAGS_NONE)});
		//txs_bot[0].push_back({ E_Textures::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/tEXTURE/bot color.jpg", WIC_FLAGS_NONE) });
		_RenderSystem.Material_SetTextures(hash_materials_girl[0], txs_bot[0]);
		obj->m_Mesh_Material.push_back({ hash_girl , hash_materials_girl[0] });

		pMat = _ResourceSystem.GetResource<Material>(hash_materials_girl[1]);
		txs_bot[1].push_back({ E_Textures::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/" +_tomw(pMat->GetTexturesPaths()[0][0]), WIC_FLAGS_NONE) });
		//txs_bot[1].push_back({ E_Textures::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/tEXTURE/BOdy Skin Base Color.png", WIC_FLAGS_NONE) });
		_RenderSystem.Material_SetTextures(hash_materials_girl[1], txs_bot[1]);
		obj->m_Mesh_Material.push_back({ hash_girl , hash_materials_girl[1] });

		pMat = _ResourceSystem.GetResource<Material>(hash_materials_girl[2]);
		//txs_bot[2].push_back({ E_Textures::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/" + _tomw(pMat->GetTexturesPaths()[0][0]) , WIC_FLAGS_NONE) });
		txs_bot[2].push_back({ E_Textures::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/tEXTURE/FACE Base Color alpha.png", WIC_FLAGS_NONE) });
		_RenderSystem.Material_SetTextures(hash_materials_girl[2], txs_bot[2]);
		obj->m_Mesh_Material.push_back({ hash_girl , hash_materials_girl[2] });

		txs_bot[3].push_back({ E_Textures::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/tEXTURE/COLORS.jpg", WIC_FLAGS_NONE) });
		_RenderSystem.Material_SetTextures(hash_materials_girl[3], txs_bot[3]);
		obj->m_Mesh_Material.push_back({ hash_girl , hash_materials_girl[3] });

		txs_bot[4].push_back({ E_Textures::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/tEXTURE/BOdy Skin Base Color.png", WIC_FLAGS_NONE) });
		_RenderSystem.Material_SetTextures(hash_materials_girl[4], txs_bot[4]);
		obj->m_Mesh_Material.push_back({ hash_girl , hash_materials_girl[4] });

		txs_bot[5].push_back({ E_Textures::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/tEXTURE/COLORS.jpg", WIC_FLAGS_NONE) });
		_RenderSystem.Material_SetTextures(hash_materials_girl[5], txs_bot[5]);
		obj->m_Mesh_Material.push_back({ hash_girl , hash_materials_girl[5] });

		txs_bot[6].push_back({ E_Textures::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/tEXTURE/top color.png", WIC_FLAGS_NONE) });
		_RenderSystem.Material_SetTextures(hash_materials_girl[6], txs_bot[6]);
		obj->m_Mesh_Material.push_back({ hash_girl , hash_materials_girl[6] });
	}
#endif // DEBUG

#ifdef _TESTBLOCK
	{
		//SkySphere
		{
			size_t hash_mesh_skysphere = _RenderSystem.CreateMesh(L"../Assets/Meshes/sphere.obj", E_Colliders::SPHERE);
			size_t hash_material_skysphere = _RenderSystem.CreateMaterial(L"Mat_SkySphere", L"VertexShaderPTN.hlsl", L"PSSkySphere.hlsl");
			std::vector<TX_HASH> txs_skysphere;
			txs_skysphere.push_back({ E_Textures::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/stars_map.jpg", WIC_FLAGS_IGNORE_SRGB) });
			_RenderSystem.Material_SetTextures(hash_material_skysphere, txs_skysphere);

			TempObj* obj = new TempObj();
			obj->m_vScale = Vector3(_CameraSystem.GetCamera(0)->GetFarZ() * 0.9f, _CameraSystem.GetCamera(0)->GetFarZ() * 0.9f, _CameraSystem.GetCamera(0)->GetFarZ() * 0.9f);
			obj->m_Mesh_Material.push_back({ hash_mesh_skysphere , hash_material_skysphere });
			_RenderSystem.SkyObj = obj;
		}

		//SpaceShip
		{
			size_t hash_mesh = _RenderSystem.CreateMesh(L"../Assets/Meshes/spaceship.obj", E_Colliders::SPHERE);
			std::vector<std::wstring> VSs, PSs;
			VSs.push_back(L"VertexShaderPTN.hlsl"); PSs.push_back(L"PixelShaderPTN.hlsl");

			std::vector<size_t> hash_mats = _RenderSystem.CreateMaterials(L"../Assets/Meshes/spaceship.mtl", VSs, PSs);
			std::vector<TX_HASH> TX_HASHS[1];
			TX_HASHS[0].push_back({ E_Textures::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/stars_spaceship.jpg", WIC_FLAGS_IGNORE_SRGB) });
			_RenderSystem.Material_SetTextures(hash_mats[0], TX_HASHS[0]);

			_RenderSystem.objs.push_back(new TempObj());
			TempObj* obj = _RenderSystem.objs.back();
			obj->m_vScale = Vector3(25.0f, 25.0f, 25.0f);
			obj->m_Mesh_Material.push_back({ hash_mesh , hash_mats[0]});
		}
		
		//Asteroid
		{
			size_t hash_mesh = _RenderSystem.CreateMesh(L"../Assets/Meshes/asteroid.obj", E_Colliders::SPHERE);
			std::vector<std::wstring> VSs, PSs;
			VSs.push_back(L"VertexShaderPTN.hlsl"); PSs.push_back(L"PixelShaderPTN.hlsl");

			std::vector<size_t> hash_mats = _RenderSystem.CreateMaterials(L"../Assets/Meshes/asteroid.mtl", VSs, PSs);
			std::vector<TX_HASH> TX_HASHS[1];
			TX_HASHS[0].push_back({ E_Textures::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/stars_asteroid.jpg", WIC_FLAGS_IGNORE_SRGB) });
			_RenderSystem.Material_SetTextures(hash_mats[0], TX_HASHS[0]);

			for (int i = 0; i < 300; i++)
			{
				_RenderSystem.objs.push_back(new TempObj());
				TempObj* obj = _RenderSystem.objs.back();
				obj->m_vScale = Vector3(30.0f, 30.0f, 30.0f);
				obj->m_vRotate = Vector3(dis(gen), dis(gen), dis(gen));
				obj->m_vPosition = Vector3(dis(gen) * 30, dis(gen) * 30, dis(gen) * 30);
				obj->m_Mesh_Material.push_back({ hash_mesh , hash_mats[0] });
			}
		}
	}
#endif _TESTBLOCK
}

void AppWindow::OnUpdate()
{
	std::cout << "OnUpdate" << '\n';
	_TimerSystem.Frame();
	std::cout << "ElapsedTime : " << _TimerSystem.GetElapsedTime() << '\n';
	std::cout << "FPS : " << _TimerSystem.GetFps() << '\n';
	_InputSystem.Frame();
	_CameraSystem.Frame(_TimerSystem.GetDeltaTime());
	_ImguiSystem.Frame();
	_RenderSystem.Frame(_TimerSystem.GetDeltaTime());
	_CollisionSystem.Frame(_TimerSystem.GetDeltaTime());

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
