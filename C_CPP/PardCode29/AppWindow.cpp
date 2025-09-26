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
#include "GeometryGenerator.h"

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
	_CameraSystem.GetCamera(0)->SetScreenWidth(m_iWidth);
	_CameraSystem.GetCamera(0)->SetScreenHeight(m_iHeight);
	_CameraSystem.GetCamera(0)->SetFOV(75.0f);
	_CameraSystem.GetCamera(0)->SetClipPlanes(0.1f, 10000.0f);
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
	{
		//skysphere
		{
			size_t hash_mesh = _RenderSystem.CreateMesh<Vertex_PTN>(L"../Assets/Meshes/sphere.obj", E_Colliders::SPHERE);
			size_t hash_material = _RenderSystem.CreateMaterial<Vertex_PTN>(L"Mat_SkySphere", L"VS_PTN.hlsl", L"PS_.hlsl");
			std::vector<TX_HASH> hash_tx_hash;
			hash_tx_hash.push_back({ E_Textures::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/butter8.png", WIC_FLAGS_NONE) });
			_RenderSystem.Material_SetTextures(hash_material, hash_tx_hash);

			TempObj* obj = new TempObj();
			obj->m_vScale = Vector3(_CameraSystem.GetCamera(0)->GetFarZ() * 0.9f, _CameraSystem.GetCamera(0)->GetFarZ() * 0.9f, _CameraSystem.GetCamera(0)->GetFarZ() * 0.9f);
			obj->m_Mesh_Material.push_back({ hash_mesh , hash_material });
			_RenderSystem.SkyObj = obj;
		}
		
		//rand obj
		{
			for (int i = 0; i < 50; i++)
			{
				_RenderSystem.objs.push_back(new TempObj());
				TempObj* obj = _RenderSystem.objs.back();
				obj->m_vScale = Vector3(300.0f, 300.0f, 300.0f);
				obj->m_vRotate = Vector3(0.0f, 0.0f, 0.0f);
				obj->m_vPosition = Vector3(dis(gen) * 30.0f, dis(gen) * 30.0f, dis(gen) * 30.0f);

				size_t hash_mesh_rand = _RenderSystem.CreateMesh<Vertex_PTN>(i % 3 ? L"../Assets/Meshes/sphere.obj" : L"../Assets/Meshes/cube.obj", i % 3 ? E_Colliders::SPHERE : E_Colliders::AABB);
				size_t hash_meterial_rand = _RenderSystem.CreateMaterial<Vertex_PTN>(i % 3 ? L"Mat_Rand0" : L"Mat_Rand1", L"VS_PTN.hlsl", L"PS_PTN.hlsl");
				std::vector<TX_HASH> txs_objRand;
				txs_objRand.push_back({ E_Textures::Diffuse, _RenderSystem.CreateTexture(i % 2 ? L"../Assets/Textures/butter.dds" : L"../Assets/Textures/butter8.png", WIC_FLAGS_NONE) });
				_RenderSystem.Material_SetTextures(hash_meterial_rand, txs_objRand);
				obj->m_Mesh_Material.push_back({ hash_mesh_rand , hash_meterial_rand });
			}
		}
		
		//PardCode22(earth)
		{
			size_t hash_mesh = _RenderSystem.CreateMesh<Vertex_PTN>(L"../Assets/Meshes/sphere.obj", E_Colliders::SPHERE);
			size_t mash_material = _RenderSystem.CreateMaterial<Vertex_PTN>(L"Mat_Earth", L"VS_PTN.hlsl", L"PS_TextureFlow.hlsl");
			std::vector<TX_HASH> hash_tx_hash;
			hash_tx_hash.push_back({ E_Textures::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/PardCode22/earth_color.jpg", WIC_FLAGS_NONE) });
			hash_tx_hash.push_back({ E_Textures::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/PardCode22/earth_night.jpg", WIC_FLAGS_NONE) });
			hash_tx_hash.push_back({ E_Textures::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/PardCode22/earth_clouds.jpg", WIC_FLAGS_NONE) });
			hash_tx_hash.push_back({ E_Textures::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/PardCode22/earth_spec.jpg", WIC_FLAGS_NONE) });
			_RenderSystem.Material_SetTextures(mash_material, hash_tx_hash);

			_RenderSystem.objs.push_back(new TempObj());
			TempObj* obj = _RenderSystem.objs.back();
			obj->m_vScale = Vector3(300.0f, 300.0f, 300.0f);
			obj->m_vRotate = Vector3(0.0f, 0.0f, 0.0f);
			obj->m_vPosition = Vector3(700, 600.0f, -200);
			obj->m_Mesh_Material.push_back({ hash_mesh , mash_material });
		}

		//PardCode25(house)
		{
			size_t hash_mesh = _RenderSystem.CreateMesh<Vertex_PTN>(L"../Assets/Meshes/house.obj", E_Colliders::AABB);
			std::vector<std::wstring> VSs, PSs;
			VSs.push_back(L"VS_PTN.hlsl"); VSs.push_back(L"VS_PTN.hlsl"); VSs.push_back(L"VS_PTN.hlsl"); VSs.push_back(L"VS_PTN.hlsl");
			PSs.push_back(L"PS_PTN.hlsl"); PSs.push_back(L"PS_PTN.hlsl"); PSs.push_back(L"PS_PTN.hlsl"); PSs.push_back(L"PS_PTN.hlsl");
			std::vector<size_t> hash_materials = _RenderSystem.CreateMaterials<Vertex_PTN>(L"../Assets/Meshes/house.mtl", VSs, PSs);
			std::vector<TX_HASH> hash_tx_hash[4];
			hash_tx_hash[0].push_back({E_Textures::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/house_barrel.jpg", WIC_FLAGS_NONE)});
			_RenderSystem.Material_SetTextures(hash_materials[0], hash_tx_hash[0]);
			hash_tx_hash[1].push_back({E_Textures::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/house_brick.jpg", WIC_FLAGS_NONE)});
			_RenderSystem.Material_SetTextures(hash_materials[1], hash_tx_hash[1]);
			hash_tx_hash[2].push_back({E_Textures::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/house_windows.jpg", WIC_FLAGS_NONE)});
			_RenderSystem.Material_SetTextures(hash_materials[2], hash_tx_hash[2]);
			hash_tx_hash[3].push_back({E_Textures::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/house_wood.jpg", WIC_FLAGS_NONE)});
			_RenderSystem.Material_SetTextures(hash_materials[3], hash_tx_hash[3]);

			_RenderSystem.objs.push_back(new TempObj());
			TempObj* obj = _RenderSystem.objs.back();
			obj->m_vScale = Vector3(300.0f, 300.0f, 300.0f);
			obj->m_vRotate = Vector3(0.0f, 0.0f, 0.0f);
			obj->m_vPosition = Vector3(0.0f, 0.0f, 0.0f);
			obj->m_Mesh_Material.push_back({ hash_mesh , hash_materials[0] });
			obj->m_Mesh_Material.push_back({ hash_mesh , hash_materials[1] });
			obj->m_Mesh_Material.push_back({ hash_mesh , hash_materials[2] });
			obj->m_Mesh_Material.push_back({ hash_mesh , hash_materials[3] });
		}
		
		//nene
		{
			size_t hash_mesh = _RenderSystem.CreateMesh<Vertex_PTN>(L"../Assets/Meshes/nene.obj", E_Colliders::SPHERE);
			std::vector<std::wstring> VSs, PSs;
			VSs.push_back(L"VS_PTN.hlsl"); VSs.push_back(L"VS_PTN.hlsl"); VSs.push_back(L"VS_PTN.hlsl");
			PSs.push_back(L"PS_PTN.hlsl"); PSs.push_back(L"PS_PTN.hlsl"); PSs.push_back(L"PS_PTN.hlsl");
			std::vector<size_t> hash_materals = _RenderSystem.CreateMaterials<Vertex_PTN>(L"../Assets/Meshes/nene.mtl", VSs, PSs);
			std::vector<TX_HASH> txs_nene[3];
			txs_nene[0].push_back({E_Textures::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/face.dds", WIC_FLAGS_NONE)});
			_RenderSystem.Material_SetTextures(hash_materals[0], txs_nene[0]);
			txs_nene[1].push_back({E_Textures::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/body.dds", WIC_FLAGS_NONE)});
			_RenderSystem.Material_SetTextures(hash_materals[1], txs_nene[1]);
			txs_nene[2].push_back({E_Textures::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/hair.dds", WIC_FLAGS_NONE)});
			_RenderSystem.Material_SetTextures(hash_materals[2], txs_nene[2]);

			_RenderSystem.objs.push_back(new TempObj());
			TempObj* obj = _RenderSystem.objs.back();
			obj->m_vScale = Vector3(50.0f, 50.0f, 50.0f);
			obj->m_vRotate = Vector3(0.0f, 0.0f, 0.0f);
			obj->m_vPosition = Vector3(0.0f, 600.0f, 0.0f);
			obj->m_Mesh_Material.push_back({ hash_mesh , hash_materals[0] });
			obj->m_Mesh_Material.push_back({ hash_mesh , hash_materals[1] });
			obj->m_Mesh_Material.push_back({ hash_mesh , hash_materals[2] });
		}
		
		//girlobj
		{
			size_t hash_mesh = _RenderSystem.CreateMesh<Vertex_PTN>(L"../Assets/Meshes/girl.obj", E_Colliders::SPHERE);
			std::vector<std::wstring> VSs, PSs;
			VSs.push_back(L"VS_PTN.hlsl"); VSs.push_back(L"VS_PTN.hlsl"); VSs.push_back(L"VS_PTN.hlsl");
			VSs.push_back(L"VS_PTN.hlsl"); VSs.push_back(L"VS_PTN.hlsl"); VSs.push_back(L"VS_PTN.hlsl"); VSs.push_back(L"VS_PTN.hlsl");
			PSs.push_back(L"PS_PTN.hlsl"); PSs.push_back(L"PS_PTN.hlsl"); PSs.push_back(L"PS_PTN.hlsl");
			PSs.push_back(L"PS_PTN.hlsl"); PSs.push_back(L"PS_PTN.hlsl"); PSs.push_back(L"PS_PTN.hlsl"); PSs.push_back(L"PS_PTN.hlsl");
			std::vector<size_t> hash_materials = _RenderSystem.CreateMaterials<Vertex_PTN>(L"../Assets/Meshes/girl.mtl", VSs, PSs);

			std::vector<TX_HASH> hash_tx_hash[7];
			auto pMat = _ResourceSystem.GetResource<Material>(hash_materials[0]);
			hash_tx_hash[0].push_back({ E_Textures::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/" + _tomw(pMat->GetTexturesPaths()[0][0]), WIC_FLAGS_NONE) });
			//txs_bot[0].push_back({ E_Textures::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/tEXTURE/bot color.jpg", WIC_FLAGS_NONE) });
			_RenderSystem.Material_SetTextures(hash_materials[0], hash_tx_hash[0]);
			
			pMat = _ResourceSystem.GetResource<Material>(hash_materials[1]);
			hash_tx_hash[1].push_back({ E_Textures::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/" + _tomw(pMat->GetTexturesPaths()[0][0]), WIC_FLAGS_NONE) });
			//txs_bot[1].push_back({ E_Textures::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/tEXTURE/BOdy Skin Base Color.png", WIC_FLAGS_NONE) });
			_RenderSystem.Material_SetTextures(hash_materials[1], hash_tx_hash[1]);
			
			pMat = _ResourceSystem.GetResource<Material>(hash_materials[2]);
			hash_tx_hash[2].push_back({ E_Textures::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/" + _tomw(pMat->GetTexturesPaths()[0][0]) , WIC_FLAGS_NONE) });
			//hash_tx_hash[2].push_back({ E_Textures::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/tEXTURE/FACE Base Color alpha.png", WIC_FLAGS_NONE) });
			_RenderSystem.Material_SetTextures(hash_materials[2], hash_tx_hash[2]);
			
			hash_tx_hash[3].push_back({ E_Textures::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/tEXTURE/COLORS.jpg", WIC_FLAGS_NONE) });
			_RenderSystem.Material_SetTextures(hash_materials[3], hash_tx_hash[3]);
	
			hash_tx_hash[4].push_back({ E_Textures::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/tEXTURE/BOdy Skin Base Color.png", WIC_FLAGS_NONE) });
			_RenderSystem.Material_SetTextures(hash_materials[4], hash_tx_hash[4]);

			hash_tx_hash[5].push_back({ E_Textures::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/tEXTURE/COLORS.jpg", WIC_FLAGS_NONE) });
			_RenderSystem.Material_SetTextures(hash_materials[5], hash_tx_hash[5]);

			hash_tx_hash[6].push_back({ E_Textures::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/tEXTURE/top color.png", WIC_FLAGS_NONE) });
			_RenderSystem.Material_SetTextures(hash_materials[6], hash_tx_hash[6]);
	
			_RenderSystem.objs.push_back(new TempObj());
			TempObj* obj = _RenderSystem.objs.back();
			obj->m_vScale = Vector3(500.0f, 500.0f, 500.0f);
			obj->m_vRotate = Vector3(0.0f, 0.0f, 0.0f);
			obj->m_vPosition = Vector3(1500.0f, 600.0f, 0.0f);
			obj->m_Mesh_Material.push_back({ hash_mesh , hash_materials[0] });
			obj->m_Mesh_Material.push_back({ hash_mesh , hash_materials[1] });
			obj->m_Mesh_Material.push_back({ hash_mesh , hash_materials[2] });
			obj->m_Mesh_Material.push_back({ hash_mesh , hash_materials[3] });
			obj->m_Mesh_Material.push_back({ hash_mesh , hash_materials[4] });
			obj->m_Mesh_Material.push_back({ hash_mesh , hash_materials[5] });
			obj->m_Mesh_Material.push_back({ hash_mesh , hash_materials[6] });
		}
	}
#endif // DEBUG

#ifdef _TESTBLOCK
	{
		//SkySphere
		{
			size_t hash_mesh_skysphere = _RenderSystem.CreateMesh<Vertex_PTN>(L"../Assets/Meshes/cube.obj", E_Colliders::AABB);
			size_t hash_material_skysphere = _RenderSystem.CreateMaterial<Vertex_PTN>(L"Mat_SkySphere", L"VS_PTN.hlsl", L"PS_.hlsl");
			std::vector<TX_HASH> txs_skysphere;
			txs_skysphere.push_back({ E_Textures::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/stars_map.jpg", WIC_FLAGS_IGNORE_SRGB) });
			_RenderSystem.Material_SetTextures(hash_material_skysphere, txs_skysphere);

			TempObj* obj = new TempObj();
			obj->m_vScale = Vector3(_CameraSystem.GetCamera(0)->GetFarZ() * 0.9f, _CameraSystem.GetCamera(0)->GetFarZ() * 0.9f, _CameraSystem.GetCamera(0)->GetFarZ() * 0.9f);
			obj->m_Mesh_Material.push_back({ hash_mesh_skysphere , hash_material_skysphere });
			_RenderSystem.SkyObj = obj;
		}

		//normalmap
		{
			size_t hash_mesh = _RenderSystem.CreateMesh<Vertex_PTNTB>(L"../Assets/Meshes/sphere.obj", E_Colliders::SPHERE);
			size_t hash_material = _RenderSystem.CreateMaterial<Vertex_PTNTB>(L"Mat_NormalMapping", L"VS_PTNTB.hlsl", L"PS_PTNTB.hlsl");
			std::vector<TX_HASH> tx_hash;
			tx_hash.push_back({ E_Textures::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/brick_d.jpg", WIC_FLAGS_IGNORE_SRGB) });
			tx_hash.push_back({ E_Textures::Normal, _RenderSystem.CreateTexture(L"../Assets/Textures/brick_n.jpg", WIC_FLAGS_IGNORE_SRGB) });
			_RenderSystem.Material_SetTextures(hash_material, tx_hash);

			_RenderSystem.objs.push_back(new TempObj());
			TempObj* obj = _RenderSystem.objs.back();
			obj->m_vScale = Vector3(300, 300, 300);
			obj->m_Mesh_Material.push_back({ hash_mesh , hash_material });
		}
		//rand obj
		{
			for (int i = 0; i < 50; i++)
			{
				_RenderSystem.objs.push_back(new TempObj());
				TempObj* obj = _RenderSystem.objs.back();
				obj->m_vScale = Vector3(300.0f, 300.0f, 300.0f);
				obj->m_vRotate = Vector3(0.0f, 0.0f, 0.0f);
				obj->m_vPosition = Vector3(dis(gen) * 30.0f, dis(gen) * 30.0f, dis(gen) * 30.0f);

				size_t hash_mesh_rand = _RenderSystem.CreateMesh<Vertex_PTN>(i % 3 ? L"../Assets/Meshes/sphere.obj" : L"../Assets/Meshes/cube.obj", i % 3 ? E_Colliders::SPHERE : E_Colliders::AABB);
				size_t hash_meterial_rand = _RenderSystem.CreateMaterial<Vertex_PTN>(i % 3 ? L"Mat_Rand0" : L"Mat_Rand1", L"VS_PTN.hlsl", L"PS_PTN.hlsl");
				std::vector<TX_HASH> txs_objRand;
				txs_objRand.push_back({ E_Textures::Diffuse, _RenderSystem.CreateTexture(i % 2 ? L"../Assets/Textures/butter.dds" : L"../Assets/Textures/butter8.png", WIC_FLAGS_NONE) });
				_RenderSystem.Material_SetTextures(hash_meterial_rand, txs_objRand);
				obj->m_Mesh_Material.push_back({ hash_mesh_rand , hash_meterial_rand });
			}
		}

		{
			std::vector<std::vector<Vector3>> points;
			std::vector<Vertex_PTN> vertices;
			std::vector<UINT> indices;
			GeometryGenerate_Plane(points, vertices, indices);
			size_t hash_mesh = _RenderSystem.CreateMeshFromGeometry<Vertex_PTN>(L"Plane", std::move(points), std::move(vertices), std::move(indices), E_Colliders::AABB);
			{
				size_t hash_material = _RenderSystem.CreateMaterial<Vertex_PTN>(L"Mat_srv", L"VS_PTN.hlsl", L"PS_.hlsl");
				std::vector<TX_HASH> tx_hash;
				tx_hash.push_back({ E_Textures::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/butter5.webp", WIC_FLAGS_IGNORE_SRGB) });
				_RenderSystem.Material_SetTextures(hash_material, tx_hash);

				_RenderSystem.ortho_objs.push_back(new TempObj());
				TempObj* obj = _RenderSystem.ortho_objs.back();
				obj->m_vScale = Vector3(m_iWidth, m_iHeight, 1.0f);
				obj->m_vPosition = Vector3(0.0f, 0.0f, 1.0f);
				obj->m_Mesh_Material.push_back({ hash_mesh , hash_material });
			}

			{
				size_t hash_material = _RenderSystem.CreateMaterial<Vertex_PTN>(L"Mat_depth", L"VS_PTN.hlsl", L"PS_DepthDebug.hlsl");
				std::vector<TX_HASH> tx_hash;
				tx_hash.push_back({ E_Textures::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/butter5.webp", WIC_FLAGS_IGNORE_SRGB) });
				_RenderSystem.Material_SetTextures(hash_material, tx_hash);

				_RenderSystem.ortho_objs.push_back(new TempObj());
				TempObj* obj = _RenderSystem.ortho_objs.back();
				obj->m_vScale = Vector3(300, 300, 1.0f);
				obj->m_vPosition = Vector3(100.0f, 0.0f, 1.0f);
				obj->m_Mesh_Material.push_back({ hash_mesh , hash_material });
			}
		}
		
		/*{
			std::vector<std::vector<Vector3>> points;
			std::vector<Vertex_PTN> vertices;
			std::vector<UINT> indices;
			GeometryGenerate_Plane(points, vertices, indices);
			size_t hash_mesh = _RenderSystem.CreateMeshFromGeometry<Vertex_PTN>(L"Plane", std::move(points), std::move(vertices), std::move(indices), E_Colliders::AABB);
			size_t hash_material = _RenderSystem.CreateMaterial<Vertex_PTN>(L"Mat_UI2", L"VS_PTN.hlsl", L"PS_.hlsl");
			std::vector<TX_HASH> tx_hash;
			tx_hash.push_back({ E_Textures::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/butter6.webp", WIC_FLAGS_IGNORE_SRGB) });
			_RenderSystem.Material_SetTextures(hash_material, tx_hash);

			_RenderSystem.ortho_objs.push_back(new TempObj());
			TempObj* obj = _RenderSystem.ortho_objs.back();
			obj->m_vScale = Vector3(200, 200, 1.0f);
			obj->m_vPosition = Vector3(-150.0f, 50.0f, 0.5f);
			obj->m_Mesh_Material.push_back({ hash_mesh , hash_material });
		}*/

		////SpaceShip
		//{
		//	size_t hash_mesh = _RenderSystem.CreateMesh<Vertex_PTN>(L"../Assets/Meshes/spaceship.obj", E_Colliders::SPHERE);
		//	std::vector<std::wstring> VSs, PSs;
		//	VSs.push_back(L"VS_PTN.hlsl"); PSs.push_back(L"PS_PTN.hlsl");

		//	std::vector<size_t> hash_mats = _RenderSystem.CreateMaterials<Vertex_PTN>(L"../Assets/Meshes/spaceship.mtl", VSs, PSs);
		//	std::vector<TX_HASH> TX_HASHS[1];
		//	TX_HASHS[0].push_back({ E_Textures::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/stars_spaceship.jpg", WIC_FLAGS_IGNORE_SRGB) });
		//	_RenderSystem.Material_SetTextures(hash_mats[0], TX_HASHS[0]);

		//	_RenderSystem.objs.push_back(new TempObj());
		//	TempObj* obj = _RenderSystem.objs.back();
		//	obj->m_vScale = Vector3(25.0f, 25.0f, 25.0f);
		//	obj->m_Mesh_Material.push_back({ hash_mesh , hash_mats[0]});
		//}
		//
		////Asteroid
		//{
		//	size_t hash_mesh = _RenderSystem.CreateMesh<Vertex_PTN>(L"../Assets/Meshes/asteroid.obj", E_Colliders::SPHERE);
		//	std::vector<std::wstring> VSs, PSs;
		//	VSs.push_back(L"VS_PTN.hlsl"); PSs.push_back(L"PS_PTN.hlsl");

		//	std::vector<size_t> hash_mats = _RenderSystem.CreateMaterials<Vertex_PTN>(L"../Assets/Meshes/asteroid.mtl", VSs, PSs);
		//	std::vector<TX_HASH> TX_HASHS[1];
		//	TX_HASHS[0].push_back({ E_Textures::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/stars_asteroid.jpg", WIC_FLAGS_IGNORE_SRGB) });
		//	_RenderSystem.Material_SetTextures(hash_mats[0], TX_HASHS[0]);

		//	for (int i = 0; i < 300; i++)
		//	{
		//		_RenderSystem.objs.push_back(new TempObj());
		//		TempObj* obj = _RenderSystem.objs.back();
		//		obj->m_vScale = Vector3(30.0f, 30.0f, 30.0f);
		//		obj->m_vRotate = Vector3(dis(gen), dis(gen), dis(gen));
		//		obj->m_vPosition = Vector3(dis(gen) * 30, dis(gen) * 30, dis(gen) * 30);
		//		obj->m_Mesh_Material.push_back({ hash_mesh , hash_mats[0] });
		//	}
		//}
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
