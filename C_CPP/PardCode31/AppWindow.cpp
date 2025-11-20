#include "AppWindow.h"
#include "RenderSystem.h"
#include "InputSystem.h"
#include "CameraSystem.h"
#include "FirstPersonCamera.h"
#include "ResourceSystem.h"
#include "ImguiSystem.h"
#include "LightSystem.h"
#include "TimerSystem.h"
#include "CollisionSystem.h"
#include "TestBlockMacro.h"
#include "Material.h"			//mtl텍스쳐로딩을위한 임시임포트
#include "GeometryGenerator.h"

#include "ECSSystem.h"
#include "BehaviorSystem.h"

AppWindow::AppWindow()
{
	std::cout << "Initialize : " << "AppWindow" << " Class" << '\n';
	_TimerSystem;
	_InputSystem;
	_ResourceSystem;
	_CameraSystem;
	_CollisionSystem;
	_ECSSystem;
	_RenderSystem;
	_ImguiSystem;
	_LightSystem;
	_BehaviorSystem;
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
	_ECSSystem.Init();
	_BehaviorSystem.Init();

	// 1. 난수 시드(seed)를 설정합니다.
		// std::random_device는 하드웨어 기반의 비결정적 난수를 제공하여
		// 매번 다른 시드를 얻을 수 있습니다.
	std::random_device rd;

	// 2. mt19937 난수 생성 엔진을 초기화합니다.
	// rd()를 시드로 사용하여 매번 다른 난수 시퀀스를 생성하도록 합니다.
	std::mt19937 gen(rd());

	// 3. 난수 분포(distribution)를 정의합니다.
	// 여기서는 1부터 100까지의 균등한 정수 난수를 생성하도록 설정합니다.
	std::uniform_int_distribution<int> dis(-10, 10);

	_InputSystem.SetMouseCenter(m_hWnd);
	//InputSystem Listner(event driven)
	/*_InputSystem.AddListner(E_InputEvent::KEY_DOWN, 
		[](const InputEvent& v) -> void 
		{
			
		}
	);*/

	//Initialize Cameras
	{
		//카메라 기본세팅
		ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Input, C_Transform, C_Behavior, C_Camera, C_Projection>();
		_CameraSystem.lookup_maincam = _ECSSystem.CreateEntity<C_Input, C_Transform, C_Behavior, C_Camera, C_Projection>();
		std::bitset<256> vkmask;
		vkmask['W'] = true;
		vkmask['A'] = true;
		vkmask['S'] = true;
		vkmask['D'] = true;
		vkmask['Q'] = true;
		vkmask['E'] = true;
		vkmask[255] = true;
		_ECSSystem.AddComponent<C_Input>(key, { vkmask });
		Vector3 pos(0.0f, 50.0f, -50.0f);
		Vector3 dir = (Vector3(0.0f, 0.0f, 0.0f) - pos).Normalize();
		_ECSSystem.AddComponent<C_Transform>(key, { {1.0f, 1.0f, 1.0f}, Quarternion(dir), pos});

		std::array<unsigned char, E_Behavior::COUNT> behavior;
		behavior[E_Behavior::MOVE_FORWARD] = 'W';
		behavior[E_Behavior::MOVE_LEFT] = 'A';
		behavior[E_Behavior::MOVE_BACKWARD] = 'S';
		behavior[E_Behavior::MOVE_RIGHT] = 'D';
		behavior[E_Behavior::MOVE_UP] = 'Q';
		behavior[E_Behavior::MOVE_DOWN] = 'E';
		_ECSSystem.AddComponent<C_Behavior>(key, { behavior });
		C_Camera camera;
		camera.fScreenWidth = m_iWidth;
		camera.fScreenHeight = m_iHeight;
		camera.fFov = 75.0f;
		camera.fNear = 0.1f;
		camera.fFar = 10000.0f;
		_ECSSystem.AddComponent<C_Camera>(key, std::move(camera));
	}

	//Light Initialize
	{
		//Directional Light
		{
			ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Transform, C_Light, T_Light_Directional>();
			_LightSystem.lookup_directional = _ECSSystem.CreateEntity<C_Transform, C_Light, T_Light_Directional>();

			Vector3 dir = Vector3(0.0f, -1.0f, 1.0f).Normalize();
			_ECSSystem.AddComponent<C_Transform>(key, { {}, Quarternion(dir), {} });
			_ECSSystem.AddComponent<C_Light>(key, { Vector4(0.3f, 0.3f, 0.3f, 1.0f) , Vector4(0.7f, 0.7f, 0.7f, 1.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), 100.0f });
		}
		
		//Point Light
		{
			ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Transform, C_Light, C_Light_Attenuation, T_Light_Point>();
			_LightSystem.lookup_point = _ECSSystem.CreateEntity<C_Transform, C_Light, C_Light_Attenuation, T_Light_Point>();

			_ECSSystem.AddComponent<C_Transform>(key, { {}, {}, Vector3(10.0f, 10.0f, 0.0f) });
			_ECSSystem.AddComponent<C_Light>(key, { Vector4(0.2f, 0.5f, 0.2f, 1.0f) , Vector4(0.7f, 0.7f, 0.7f, 1.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), 100.0f });
			_ECSSystem.AddComponent<C_Light_Attenuation>(key, { 0.49f, 0.5f, 0.01f, 500.0f });
		}
		
		//Spot Light
		{
			ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Transform, C_Light, C_Light_Attenuation, C_Light_Spot, T_Light_Spot>();
			_LightSystem.lookup_spot = _ECSSystem.CreateEntity<C_Transform, C_Light, C_Light_Attenuation, C_Light_Spot, T_Light_Spot>();

			_ECSSystem.AddComponent<C_Transform>(key, { {}, (Vector3(0.0f, 0.0f, 0.0f) - Vector3(-250.0f, 500.0f, -5.0f)).Normalize(), Vector3(-250.0f, 500.0f, -5.0f) });
			_ECSSystem.AddComponent<C_Light>(key, { Vector4(0.2f, 0.2f, 0.2f, 1.0f) , Vector4(0.7f, 0.7f, 0.7f, 1.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), 100.0f });
			_ECSSystem.AddComponent<C_Light_Attenuation>(key, { 0.5f, 0.5f, 0.00f, 5000.0f });
			_ECSSystem.AddComponent<C_Light_Spot>(key, { 50.0f, cosf(_DEGTORAD(130.0f)), cosf(_DEGTORAD(10.0f)) });
		}
	}

	//Initialize RenderAsset
	{
		size_t hash_mesh = _RenderSystem.CreateMesh<Vertex_PTN>(L"../Assets/Meshes/cube.obj");
		const std::unordered_set<size_t>& hash_CLs = _RenderSystem.CreateColliders<Vertex_PTN>(hash_mesh, E_Collider::AABB);
		size_t hash_material = _RenderSystem.CreateMaterial<Vertex_PTN>(L"Mat_rand1", L"VS_PTN.hlsl", L"PS_PTN.hlsl");
		std::vector<TX_HASH> tx_hashs;
		tx_hashs.push_back({ E_Texture::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/butter6.webp", WIC_FLAGS_NONE) });
		_RenderSystem.Material_SetTextures(hash_material, tx_hashs);
		std::vector<Mesh_Material> mesh_mats;
		mesh_mats.push_back({ hash_mesh, hash_material });
		size_t hash_ra = _RenderSystem.CreateRenderAsset(L"ra0", mesh_mats);
		size_t hash_ca = _RenderSystem.CreateColliderAsset(L"ca0", hash_CLs);

		//ECS Initialize(test, 251111)
		ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Transform, C_Input, C_Behavior, C_Render, C_Collider>();

		//rand obj
		{
			for (int i = 0; i < 30; i++)
			{
				size_t lookup = _ECSSystem.CreateEntity<C_Transform, C_Input, C_Behavior, C_Render, C_Collider>();
				std::bitset<256> vkmask;
				vkmask[VK_UP] = true;
				//vkmask[VK_LEFT] = true;
				vkmask[VK_DOWN] = true;
				//vkmask[VK_RIGHT] = true;
				_ECSSystem.AddComponent<C_Input>(key, { vkmask });

				_ECSSystem.AddComponent<C_Transform>(key, { {5.0f, 5.0f, 5.0f}, Quarternion(dis(gen) * 10.0f, dis(gen) * 10.0f, dis(gen) * 10.0f), {dis(gen) * 10.0f, dis(gen) * 10.0f, dis(gen) * 10.0f}});

				std::array<unsigned char, E_Behavior::COUNT> behavior;
				behavior[E_Behavior::MOVE_FORWARD] = VK_UP;
				behavior[E_Behavior::MOVE_LEFT] = VK_LEFT;
				behavior[E_Behavior::MOVE_BACKWARD] = VK_DOWN;
				behavior[E_Behavior::MOVE_RIGHT] = VK_RIGHT;
				_ECSSystem.AddComponent<C_Behavior>(key, { behavior });

				_ECSSystem.AddComponent<C_Render>(key, { true, hash_ra });

				_ECSSystem.AddComponent<C_Collider>(key, { hash_ca });
			}
		}
	}

	//Initialize RenderAsset
	{
		size_t hash_mesh = _RenderSystem.CreateMesh<Vertex_PTN>(L"../Assets/Meshes/sphere.obj");
		const std::unordered_set<size_t>& hash_CLs = _RenderSystem.CreateColliders<Vertex_PTN>(hash_mesh, E_Collider::AABB);
		size_t hash_material = _RenderSystem.CreateMaterial<Vertex_PTN>(L"Mat_rand1", L"VS_PTN.hlsl", L"PS_PTN.hlsl");
		std::vector<TX_HASH> tx_hashs;
		tx_hashs.push_back({ E_Texture::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/butter6.webp", WIC_FLAGS_NONE) });
		_RenderSystem.Material_SetTextures(hash_material, tx_hashs);
		std::vector<Mesh_Material> mesh_mats;
		mesh_mats.push_back({ hash_mesh, hash_material });
		size_t hash_ra = _RenderSystem.CreateRenderAsset(L"ra1", mesh_mats);
		size_t hash_ca = _RenderSystem.CreateColliderAsset(L"ca1", hash_CLs);

		//ECS Initialize(test, 251111)
		ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Transform, C_Input, C_Behavior, C_Render, C_Collider>();

		//rand obj
		{
			for (int i = 0; i < 50; i++)
			{
				size_t lookup = _ECSSystem.CreateEntity<C_Transform, C_Input, C_Behavior, C_Render, C_Collider>();
				std::bitset<256> vkmask;
				//vkmask[VK_UP] = true;
				vkmask[VK_LEFT] = true;
				//vkmask[VK_DOWN] = true;
				vkmask[VK_RIGHT] = true;
				_ECSSystem.AddComponent<C_Input>(key, { vkmask });

				_ECSSystem.AddComponent<C_Transform>(key, { {5.0f, 5.0f, 5.0f}, Quarternion(dis(gen) * 10.0f, dis(gen) * 10.0f, dis(gen) * 10.0f), {dis(gen) * 10.0f, dis(gen) * 10.0f, dis(gen) * 10.0f} });

				std::array<unsigned char, E_Behavior::COUNT> behavior;
				behavior[E_Behavior::MOVE_FORWARD] = VK_UP;
				behavior[E_Behavior::MOVE_LEFT] = VK_LEFT;
				behavior[E_Behavior::MOVE_BACKWARD] = VK_DOWN;
				behavior[E_Behavior::MOVE_RIGHT] = VK_RIGHT;
				_ECSSystem.AddComponent<C_Behavior>(key, { behavior });

				_ECSSystem.AddComponent<C_Render>(key, { true, hash_ra });

				_ECSSystem.AddComponent<C_Collider>(key, { hash_ca });
			}
		}
	}

	//Initialize RenderAsset
	{
		size_t hash_mesh = _RenderSystem.CreateMesh<Vertex_PTN>(L"../Assets/Meshes/nene.obj");
		const std::unordered_set<size_t>& hash_CLs = _RenderSystem.CreateColliders<Vertex_PTN>(hash_mesh, E_Collider::SPHERE);
		std::vector<std::wstring> VSs, PSs;
		VSs.push_back(L"VS_PTN.hlsl"); VSs.push_back(L"VS_PTN.hlsl"); VSs.push_back(L"VS_PTN.hlsl");
		PSs.push_back(L"PS_PTN.hlsl"); PSs.push_back(L"PS_PTN.hlsl"); PSs.push_back(L"PS_PTN.hlsl");
		std::vector<size_t> hash_materals = _RenderSystem.CreateMaterials<Vertex_PTN>(L"../Assets/Meshes/nene.mtl", VSs, PSs);
		std::vector<TX_HASH> tx_hashs[3];
		tx_hashs[0].push_back({ E_Texture::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/face.dds", WIC_FLAGS_NONE) });
		_RenderSystem.Material_SetTextures(hash_materals[0], tx_hashs[0]);
		tx_hashs[1].push_back({ E_Texture::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/body.dds", WIC_FLAGS_NONE) });
		_RenderSystem.Material_SetTextures(hash_materals[1], tx_hashs[1]);
		tx_hashs[2].push_back({ E_Texture::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/hair.dds", WIC_FLAGS_NONE) });
		_RenderSystem.Material_SetTextures(hash_materals[2], tx_hashs[2]);

		std::vector<Mesh_Material> mesh_mats;
		mesh_mats.push_back({ hash_mesh, hash_materals[0]});
		mesh_mats.push_back({ hash_mesh, hash_materals[1] });
		mesh_mats.push_back({ hash_mesh, hash_materals[2] });
		size_t hash_ra = _RenderSystem.CreateRenderAsset(L"ra2", mesh_mats);
		size_t hash_ca = _RenderSystem.CreateColliderAsset(L"ca2", hash_CLs);

		//ECS Initialize(test, 251111)
		ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Transform, C_Behavior, C_Render, C_Collider>();

		size_t lookup = _ECSSystem.CreateEntity<C_Transform, C_Behavior, C_Render, C_Collider>();
		std::bitset<256> vkmask;
		vkmask[VK_UP] = true;
		//vkmask[VK_LEFT] = true;
		vkmask[VK_DOWN] = true;
		//vkmask[VK_RIGHT] = true;
		//_ECSSystem.AddComponent<C_Input>(key, { vkmask });

		_ECSSystem.AddComponent<C_Transform>(key, { {5.0f, 5.0f, 5.0f}, Quarternion(0.0f, 0.0f, 0.0f), {0.0f, 0.0f, 0.0f} });

		std::array<unsigned char, E_Behavior::COUNT> behavior;
		behavior[E_Behavior::MOVE_FORWARD] = VK_UP;
		behavior[E_Behavior::MOVE_LEFT] = VK_LEFT;
		behavior[E_Behavior::MOVE_BACKWARD] = VK_DOWN;
		behavior[E_Behavior::MOVE_RIGHT] = VK_RIGHT;
		_ECSSystem.AddComponent<C_Behavior>(key, { behavior });

		_ECSSystem.AddComponent<C_Render>(key, { true, hash_ra });

		_ECSSystem.AddComponent<C_Collider>(key, { hash_ca });
	}

	//Initialize RTV, DSV
	{
		//Init TargetView geometry
		std::vector<std::vector<Vector3>> points;
		std::vector<Vertex_PT> vertices;
		std::vector<UINT> indices;
		GeometryGenerate_Plane(points, vertices, indices);
		size_t hash_mesh = _RenderSystem.CreateMeshFromGeometry<Vertex_PT>(L"Plane", std::move(points), std::move(vertices), std::move(indices));
		{
			/*size_t hash_material = _RenderSystem.CreateMaterial<Vertex_PT>(L"Mat_srv", L"VS_PT.hlsl", L"PS_Distortion.hlsl");
			std::vector<TX_HASH> tx_hashs;
			tx_hashs.push_back({ E_Texture::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/butter7.png", WIC_FLAGS_IGNORE_SRGB) });
			_RenderSystem.Material_SetTextures(hash_material, tx_hashs);
			std::vector<Mesh_Material> mesh_mats;
			mesh_mats.push_back({ hash_mesh, hash_material });
			size_t hash_ra = _RenderSystem.CreateRenderAsset(L"ra_rtv", mesh_mats);

			ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Transform, C_Render>();
			size_t lookup = _ECSSystem.CreateEntity<C_Transform, C_Render>();

			_ECSSystem.AddComponent<C_Transform>(key, { {m_iWidth, m_iHeight, 1.0f}, {Vector3(0.0f, 0.0f, 0.0f)}, {0.0f, 0.0f, 0.0f} });
			_ECSSystem.AddComponent<C_Render>(key, { true, hash_ra });*/
		}

		{
			/*size_t hash_material = _RenderSystem.CreateMaterial<Vertex_PT>(L"Mat_depth", L"VS_PT.hlsl", L"PS_DepthDebug.hlsl");
			std::vector<TX_HASH> tx_hash;
			tx_hash.push_back({ E_Texture::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/butter5.webp", WIC_FLAGS_IGNORE_SRGB) });
			_RenderSystem.Material_SetTextures(hash_material, tx_hash);

			_RenderSystem.ortho_objs.push_back(new TempObj());
			TempObj* obj = _RenderSystem.ortho_objs.back();
			obj->m_vScale = Vector3(300, 300, 1.0f);
			obj->m_vPosition = Vector3(100.0f, 0.0f, 1.0f);
			obj->m_Mesh_Material.push_back({ hash_mesh , hash_material });*/
		}
	}
#ifdef _ECS
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

	//gizmo
	{
		std::vector<std::vector<Vector3>> points;
		std::vector<Vertex_PC> vertices;
		std::vector<UINT> indices;
		GeometryGenerate_Gizmo(points, vertices, indices);
		size_t hash_mesh = _RenderSystem.CreateMeshFromGeometry<Vertex_PC>(L"Gizmo", std::move(points), std::move(vertices), std::move(indices), E_Collider::AABB);
		{
			size_t hash_material = _RenderSystem.CreateMaterial<Vertex_PC>(L"Mat_Gizmo", L"VS_PC.hlsl", L"PS_PC.hlsl");
			_RenderSystem.Gizmo = new TempObj();
			TempObj* obj = _RenderSystem.Gizmo;
			obj->m_vScale = Vector3(100.0f, 100.0f, 100.0f);
			obj->m_vPosition = Vector3(0.0f, 0.0f, 0.0f);
			obj->m_Mesh_Material.push_back({ hash_mesh , hash_material });
		}
	}

#ifndef _TESTBLOCK
	{
		//skysphere
		{
			size_t hash_mesh = _RenderSystem.CreateMesh<Vertex_PTN>(L"../Assets/Meshes/sphere.obj", E_Collider::SPHERE);
			size_t hash_material = _RenderSystem.CreateMaterial<Vertex_PTN>(L"Mat_SkySphere", L"VS_PTN.hlsl", L"PS_.hlsl");
			std::vector<TX_HASH> hash_tx_hash;
			hash_tx_hash.push_back({ E_Texture::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/butter8.png", WIC_FLAGS_NONE) });
			_RenderSystem.Material_SetTextures(hash_material, hash_tx_hash);

			TempObj* obj = new TempObj();
			obj->m_vScale = Vector3(_CameraSystem.GetCamera(0)->GetFarZ() * 0.9f, _CameraSystem.GetCamera(0)->GetFarZ() * 0.9f, _CameraSystem.GetCamera(0)->GetFarZ() * 0.9f);
			obj->m_Mesh_Material.push_back({ hash_mesh , hash_material });
			_RenderSystem.SkyObj = obj;
		}

		////rand obj
		//{
		//	for (int i = 0; i < 50; i++)
		//	{
		//		_RenderSystem.objs.push_back(new TempObj());
		//		TempObj* obj = _RenderSystem.objs.back();
		//		obj->m_vScale = Vector3(300.0f, 300.0f, 300.0f);
		//		obj->m_vRotate = Vector3(0.0f, 0.0f, 0.0f);
		//		obj->m_vPosition = Vector3(dis(gen) * 30.0f, dis(gen) * 30.0f, dis(gen) * 30.0f);

		//		size_t hash_mesh_rand = _RenderSystem.CreateMesh<Vertex_PTN>(i % 3 ? L"../Assets/Meshes/sphere.obj" : L"../Assets/Meshes/cube.obj", i % 3 ? E_Collider::SPHERE : E_Collider::AABB);
		//		size_t hash_meterial_rand = _RenderSystem.CreateMaterial<Vertex_PTN>(i % 3 ? L"Mat_Rand0" : L"Mat_Rand1", L"VS_PTN.hlsl", L"PS_PTN.hlsl");
		//		std::vector<TX_HASH> txs_objRand;
		//		txs_objRand.push_back({ E_Texture::Diffuse, _RenderSystem.CreateTexture(i % 2 ? L"../Assets/Textures/butter.dds" : L"../Assets/Textures/butter8.png", WIC_FLAGS_NONE) });
		//		_RenderSystem.Material_SetTextures(hash_meterial_rand, txs_objRand);
		//		obj->m_Mesh_Material.push_back({ hash_mesh_rand , hash_meterial_rand });
		//	}
		//}

		//PardCode22(earth)
		{
			size_t hash_mesh = _RenderSystem.CreateMesh<Vertex_PTN>(L"../Assets/Meshes/sphere.obj", E_Collider::SPHERE);
			size_t mash_material = _RenderSystem.CreateMaterial<Vertex_PTN>(L"Mat_Earth", L"VS_PTN.hlsl", L"PS_TextureFlow.hlsl");
			std::vector<TX_HASH> hash_tx_hash;
			hash_tx_hash.push_back({ E_Texture::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/PardCode22/earth_color.jpg", WIC_FLAGS_NONE) });
			hash_tx_hash.push_back({ E_Texture::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/PardCode22/earth_night.jpg", WIC_FLAGS_NONE) });
			hash_tx_hash.push_back({ E_Texture::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/PardCode22/earth_clouds.jpg", WIC_FLAGS_NONE) });
			hash_tx_hash.push_back({ E_Texture::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/PardCode22/earth_spec.jpg", WIC_FLAGS_NONE) });
			_RenderSystem.Material_SetTextures(mash_material, hash_tx_hash);

			_RenderSystem.objs.push_back(new TempObj());
			TempObj* obj = _RenderSystem.objs.back();
			obj->m_vScale = Vector3(100.0f, 100.0f, 100.0f);
			obj->m_vRotate = Vector3(0.0f, 0.0f, 0.0f);
			obj->m_vPosition = Vector3(0.0f, 0.0f, 0.0f);
			obj->m_Mesh_Material.push_back({ hash_mesh , mash_material });
		}
#ifdef _REFLECT
		//ReflectPlane
		{
			std::vector<std::vector<Vector3>> points;
			std::vector<Vertex_PTN> vertices;
			std::vector<UINT> indices;
			GeometryGenerate_Plane(points, vertices, indices);
			size_t hash_mesh = _RenderSystem.CreateMeshFromGeometry<Vertex_PTN>(L"Plane_Reflect", std::move(points), std::move(vertices), std::move(indices), E_Collider::AABB);
			{
				size_t hash_material = _RenderSystem.CreateMaterial<Vertex_PTN>(L"Mat_Reflect", L"VS_PTN.hlsl", L"PS_PTN_Transparent.hlsl");
				std::vector<TX_HASH> tx_hash;
				tx_hash.push_back({ E_Texture::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/blue01.dds", DDS_FLAGS_NONE) });
				_RenderSystem.Material_SetTextures(hash_material, tx_hash);

				_RenderSystem.ReflectPlane = new TempObj();
				TempObj* obj = _RenderSystem.ReflectPlane;
				obj->m_vScale = Vector3(300.0f, 300.0f, 300.0f);
				obj->m_vRotate = Vector3(90.0f, 0.0f, 0.0f);
				obj->m_vPosition = Vector3(0.0f, -150.0f, 0.0f);
				obj->m_Mesh_Material.push_back({ hash_mesh , hash_material });
			}
		}
#endif // _REFLECT


		////PardCode25(house)
		//{
		//	size_t hash_mesh = _RenderSystem.CreateMesh<Vertex_PTN>(L"../Assets/Meshes/house.obj", E_Collider::AABB);
		//	std::vector<std::wstring> VSs, PSs;
		//	VSs.push_back(L"VS_PTN.hlsl"); VSs.push_back(L"VS_PTN.hlsl"); VSs.push_back(L"VS_PTN.hlsl"); VSs.push_back(L"VS_PTN.hlsl");
		//	PSs.push_back(L"PS_PTN.hlsl"); PSs.push_back(L"PS_PTN.hlsl"); PSs.push_back(L"PS_PTN.hlsl"); PSs.push_back(L"PS_PTN.hlsl");
		//	std::vector<size_t> hash_materials = _RenderSystem.CreateMaterials<Vertex_PTN>(L"../Assets/Meshes/house.mtl", VSs, PSs);
		//	std::vector<TX_HASH> hash_tx_hash[4];
		//	hash_tx_hash[0].push_back({E_Texture::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/house_barrel.jpg", WIC_FLAGS_NONE)});
		//	_RenderSystem.Material_SetTextures(hash_materials[0], hash_tx_hash[0]);
		//	hash_tx_hash[1].push_back({E_Texture::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/house_brick.jpg", WIC_FLAGS_NONE)});
		//	_RenderSystem.Material_SetTextures(hash_materials[1], hash_tx_hash[1]);
		//	hash_tx_hash[2].push_back({E_Texture::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/house_windows.jpg", WIC_FLAGS_NONE)});
		//	_RenderSystem.Material_SetTextures(hash_materials[2], hash_tx_hash[2]);
		//	hash_tx_hash[3].push_back({E_Texture::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/house_wood.jpg", WIC_FLAGS_NONE)});
		//	_RenderSystem.Material_SetTextures(hash_materials[3], hash_tx_hash[3]);

		//	_RenderSystem.objs.push_back(new TempObj());
		//	TempObj* obj = _RenderSystem.objs.back();
		//	obj->m_vScale = Vector3(300.0f, 300.0f, 300.0f);
		//	obj->m_vRotate = Vector3(0.0f, 0.0f, 0.0f);
		//	obj->m_vPosition = Vector3(0.0f, 0.0f, 0.0f);
		//	obj->m_Mesh_Material.push_back({ hash_mesh , hash_materials[0] });
		//	obj->m_Mesh_Material.push_back({ hash_mesh , hash_materials[1] });
		//	obj->m_Mesh_Material.push_back({ hash_mesh , hash_materials[2] });
		//	obj->m_Mesh_Material.push_back({ hash_mesh , hash_materials[3] });
		//}

		////nene
		//{
		//	size_t hash_mesh = _RenderSystem.CreateMesh<Vertex_PTN>(L"../Assets/Meshes/nene.obj", E_Collider::SPHERE);
		//	std::vector<std::wstring> VSs, PSs;
		//	VSs.push_back(L"VS_PTN.hlsl"); VSs.push_back(L"VS_PTN.hlsl"); VSs.push_back(L"VS_PTN.hlsl");
		//	PSs.push_back(L"PS_PTN.hlsl"); PSs.push_back(L"PS_PTN.hlsl"); PSs.push_back(L"PS_PTN.hlsl");
		//	std::vector<size_t> hash_materals = _RenderSystem.CreateMaterials<Vertex_PTN>(L"../Assets/Meshes/nene.mtl", VSs, PSs);
		//	std::vector<TX_HASH> txs_nene[3];
		//	txs_nene[0].push_back({E_Texture::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/face.dds", WIC_FLAGS_NONE)});
		//	_RenderSystem.Material_SetTextures(hash_materals[0], txs_nene[0]);
		//	txs_nene[1].push_back({E_Texture::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/body.dds", WIC_FLAGS_NONE)});
		//	_RenderSystem.Material_SetTextures(hash_materals[1], txs_nene[1]);
		//	txs_nene[2].push_back({E_Texture::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/hair.dds", WIC_FLAGS_NONE)});
		//	_RenderSystem.Material_SetTextures(hash_materals[2], txs_nene[2]);

		//	_RenderSystem.objs.push_back(new TempObj());
		//	TempObj* obj = _RenderSystem.objs.back();
		//	obj->m_vScale = Vector3(10.0f, 10.0f, 10.0f);
		//	obj->m_vRotate = Vector3(0.0f, 0.0f, 0.0f);
		//	obj->m_vPosition = Vector3(0.0f, 0.0f, 0.0f);
		//	obj->m_Mesh_Material.push_back({ hash_mesh , hash_materals[0] });
		//	obj->m_Mesh_Material.push_back({ hash_mesh , hash_materals[1] });
		//	obj->m_Mesh_Material.push_back({ hash_mesh , hash_materals[2] });
		//}

		////girlobj
		//{
		//	size_t hash_mesh = _RenderSystem.CreateMesh<Vertex_PTN>(L"../Assets/Meshes/girl.obj", E_Collider::SPHERE);
		//	std::vector<std::wstring> VSs, PSs;
		//	VSs.push_back(L"VS_PTN.hlsl"); VSs.push_back(L"VS_PTN.hlsl"); VSs.push_back(L"VS_PTN.hlsl");
		//	VSs.push_back(L"VS_PTN.hlsl"); VSs.push_back(L"VS_PTN.hlsl"); VSs.push_back(L"VS_PTN.hlsl"); VSs.push_back(L"VS_PTN.hlsl");
		//	PSs.push_back(L"PS_PTN.hlsl"); PSs.push_back(L"PS_PTN.hlsl"); PSs.push_back(L"PS_PTN.hlsl");
		//	PSs.push_back(L"PS_PTN.hlsl"); PSs.push_back(L"PS_PTN.hlsl"); PSs.push_back(L"PS_PTN.hlsl"); PSs.push_back(L"PS_PTN.hlsl");
		//	std::vector<size_t> hash_materials = _RenderSystem.CreateMaterials<Vertex_PTN>(L"../Assets/Meshes/girl.mtl", VSs, PSs);

		//	std::vector<TX_HASH> hash_tx_hash[7];
		//	auto pMat = _ResourceSystem.GetResource<Material>(hash_materials[0]);
		//	hash_tx_hash[0].push_back({ E_Texture::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/" + _tomw(pMat->GetTexturesPaths()[0][0]), WIC_FLAGS_NONE) });
		//	//txs_bot[0].push_back({ E_Texture::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/tEXTURE/bot color.jpg", WIC_FLAGS_NONE) });
		//	_RenderSystem.Material_SetTextures(hash_materials[0], hash_tx_hash[0]);
		//	
		//	pMat = _ResourceSystem.GetResource<Material>(hash_materials[1]);
		//	hash_tx_hash[1].push_back({ E_Texture::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/" + _tomw(pMat->GetTexturesPaths()[0][0]), WIC_FLAGS_NONE) });
		//	//txs_bot[1].push_back({ E_Texture::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/tEXTURE/BOdy Skin Base Color.png", WIC_FLAGS_NONE) });
		//	_RenderSystem.Material_SetTextures(hash_materials[1], hash_tx_hash[1]);
		//	
		//	pMat = _ResourceSystem.GetResource<Material>(hash_materials[2]);
		//	hash_tx_hash[2].push_back({ E_Texture::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/" + _tomw(pMat->GetTexturesPaths()[0][0]) , WIC_FLAGS_NONE) });
		//	//hash_tx_hash[2].push_back({ E_Texture::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/tEXTURE/FACE Base Color alpha.png", WIC_FLAGS_NONE) });
		//	_RenderSystem.Material_SetTextures(hash_materials[2], hash_tx_hash[2]);
		//	
		//	hash_tx_hash[3].push_back({ E_Texture::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/tEXTURE/COLORS.jpg", WIC_FLAGS_NONE) });
		//	_RenderSystem.Material_SetTextures(hash_materials[3], hash_tx_hash[3]);

		//	hash_tx_hash[4].push_back({ E_Texture::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/tEXTURE/BOdy Skin Base Color.png", WIC_FLAGS_NONE) });
		//	_RenderSystem.Material_SetTextures(hash_materials[4], hash_tx_hash[4]);

		//	hash_tx_hash[5].push_back({ E_Texture::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/tEXTURE/COLORS.jpg", WIC_FLAGS_NONE) });
		//	_RenderSystem.Material_SetTextures(hash_materials[5], hash_tx_hash[5]);

		//	hash_tx_hash[6].push_back({ E_Texture::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/tEXTURE/top color.png", WIC_FLAGS_NONE) });
		//	_RenderSystem.Material_SetTextures(hash_materials[6], hash_tx_hash[6]);

		//	_RenderSystem.objs.push_back(new TempObj());
		//	TempObj* obj = _RenderSystem.objs.back();
		//	obj->m_vScale = Vector3(500.0f, 500.0f, 500.0f);
		//	obj->m_vRotate = Vector3(0.0f, 0.0f, 0.0f);
		//	obj->m_vPosition = Vector3(1500.0f, 600.0f, 0.0f);
		//	obj->m_Mesh_Material.push_back({ hash_mesh , hash_materials[0] });
		//	obj->m_Mesh_Material.push_back({ hash_mesh , hash_materials[1] });
		//	obj->m_Mesh_Material.push_back({ hash_mesh , hash_materials[2] });
		//	obj->m_Mesh_Material.push_back({ hash_mesh , hash_materials[3] });
		//	obj->m_Mesh_Material.push_back({ hash_mesh , hash_materials[4] });
		//	obj->m_Mesh_Material.push_back({ hash_mesh , hash_materials[5] });
		//	obj->m_Mesh_Material.push_back({ hash_mesh , hash_materials[6] });
		//}

		////BlendCheck
		//{
		//	size_t hash_mesh = _RenderSystem.CreateMesh<Vertex_PTN>(L"../Assets/Meshes/Cube.obj", E_Collider::AABB);
		//	size_t hash_material = _RenderSystem.CreateMaterial<Vertex_PTN>(L"../Assets/Meshes/house.mtl", L"VS_PTN.hlsl", L"PS_PTN.hlsl");
		//	std::vector<TX_HASH> hash_tx_hash;
		//	hash_tx_hash.push_back({E_Texture::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/wireFence.dds", DDS_FLAGS_NONE)});
		//	_RenderSystem.Material_SetTextures(hash_material, hash_tx_hash);

		//	_RenderSystem.objs.push_back(new TempObj());
		//	TempObj* obj = _RenderSystem.objs.back();
		//	obj->m_vScale = Vector3(300.0f, 300.0f, 300.0f);
		//	obj->m_vRotate = Vector3(0.0f, 0.0f, 0.0f);
		//	obj->m_vPosition = Vector3(0.0f, 0.0f, 0.0f);
		//	obj->m_Mesh_Material.push_back({ hash_mesh , hash_material});
		//}
	}
#endif // DEBUG

#ifdef _TESTBLOCK
	{
		//SkySphere
		{
			size_t hash_mesh_skysphere = _RenderSystem.CreateMesh<Vertex_PTN>(L"../Assets/Meshes/sphere.obj", E_Collider::SPHERE);
			size_t hash_material_skysphere = _RenderSystem.CreateMaterial<Vertex_PTN>(L"Mat_SkySphere", L"VS_PTN.hlsl", L"PS_.hlsl");
			std::vector<TX_HASH> txs_skysphere;
			txs_skysphere.push_back({ E_Texture::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/stars_map.jpg", WIC_FLAGS_IGNORE_SRGB) });
			_RenderSystem.Material_SetTextures(hash_material_skysphere, txs_skysphere);

			TempObj* obj = new TempObj();
			obj->m_vScale = Vector3(_CameraSystem.GetCamera(0)->GetFarZ() * 0.9f, _CameraSystem.GetCamera(0)->GetFarZ() * 0.9f, _CameraSystem.GetCamera(0)->GetFarZ() * 0.9f);
			obj->m_Mesh_Material.push_back({ hash_mesh_skysphere , hash_material_skysphere });
			_RenderSystem.SkyObj = obj;
		}

		////NormalMap
		//{
		//	size_t hash_mesh = _RenderSystem.CreateMesh<Vertex_PTNTB>(L"../Assets/Meshes/sphere.obj", E_Collider::SPHERE);
		//	size_t hash_material = _RenderSystem.CreateMaterial<Vertex_PTNTB>(L"Mat_NormalMapping", L"VS_PTNTB.hlsl", L"PS_PTNTB.hlsl");
		//	std::vector<TX_HASH> tx_hash;
		//	tx_hash.push_back({ E_Texture::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/brick_d.jpg", WIC_FLAGS_IGNORE_SRGB) });
		//	tx_hash.push_back({ E_Texture::Normal, _RenderSystem.CreateTexture(L"../Assets/Textures/brick_n.jpg", WIC_FLAGS_IGNORE_SRGB) });
		//	_RenderSystem.Material_SetTextures(hash_material, tx_hash);

		//	_RenderSystem.objs.push_back(new TempObj());
		//	TempObj* obj = _RenderSystem.objs.back();
		//	obj->m_vScale = Vector3(300, 300, 300);
		//	obj->m_Mesh_Material.push_back({ hash_mesh , hash_material });
		//}

		////rand obj
		//{
		//	
		//	size_t hash_mesh_cube = _RenderSystem.CreateMesh<Vertex_PTN>(L"../Assets/Meshes/cube.obj", E_Collider::AABB);
		//	size_t hash_meterial_cube = _RenderSystem.CreateMaterial<Vertex_PTN>(L"Mat_rand0", L"VS_PTN.hlsl", L"PS_PTN.hlsl");
		//	std::vector<TX_HASH> txs_cube;
		//	txs_cube.push_back({ E_Texture::Diffuse, _RenderSystem.CreateTexture( L"../Assets/Textures/butter6.webp", WIC_FLAGS_NONE) });
		//	_RenderSystem.Material_SetTextures(hash_meterial_cube, txs_cube);

		//	size_t hash_mesh_sphere = _RenderSystem.CreateMesh<Vertex_PTN>(L"../Assets/Meshes/sphere.obj", E_Collider::SPHERE);
		//	size_t hash_meterial_sphere = _RenderSystem.CreateMaterial<Vertex_PTN>(L"Mat_rand1", L"VS_PTN.hlsl", L"PS_PTN.hlsl");
		//	std::vector<TX_HASH> txs_sphere;
		//	txs_sphere.push_back({ E_Texture::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/butter.dds", DDS_FLAGS_NONE) });
		//	_RenderSystem.Material_SetTextures(hash_meterial_sphere, txs_sphere);

		//	for (int i = 0; i < 50; i++)
		//	{
		//		_RenderSystem.objs.push_back(new TempObj());
		//		TempObj* obj = _RenderSystem.objs.back();
		//		obj->m_vScale = Vector3(300.0f, 300.0f, 300.0f);
		//		obj->m_vRotate = Vector3(0.0f, 0.0f, 0.0f);
		//		obj->m_vPosition = Vector3(dis(gen) * 30.0f, dis(gen) * 30.0f, dis(gen) * 30.0f);

		//		size_t hash_mesh = i % 3 ? hash_mesh_sphere : hash_mesh_cube;
		//		size_t hash_meterial = i % 3 ? hash_meterial_sphere : hash_meterial_cube;
		//		obj->m_Mesh_Material.push_back({ hash_mesh , hash_meterial });
		//	}
		//}

		/*{
			std::vector<std::vector<Vector3>> points;
			std::vector<Vertex_PTN> vertices;
			std::vector<UINT> indices;
			GeometryGenerate_Plane(points, vertices, indices);
			size_t hash_mesh = _RenderSystem.CreateMeshFromGeometry<Vertex_PTN>(L"Plane", std::move(points), std::move(vertices), std::move(indices), E_Collider::AABB);
			size_t hash_material = _RenderSystem.CreateMaterial<Vertex_PTN>(L"Mat_UI2", L"VS_PTN.hlsl", L"PS_.hlsl");
			std::vector<TX_HASH> tx_hash;
			tx_hash.push_back({ E_Texture::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/butter6.webp", WIC_FLAGS_IGNORE_SRGB) });
			_RenderSystem.Material_SetTextures(hash_material, tx_hash);

			_RenderSystem.ortho_objs.push_back(new TempObj());
			TempObj* obj = _RenderSystem.ortho_objs.back();
			obj->m_vScale = Vector3(200, 200, 1.0f);
			obj->m_vPosition = Vector3(-150.0f, 50.0f, 0.5f);
			obj->m_Mesh_Material.push_back({ hash_mesh , hash_material });
		}*/
#ifdef _SPACESHIP
		//SpaceShip
		{
			size_t hash_mesh = _RenderSystem.CreateMesh<Vertex_PTN>(L"../Assets/Meshes/spaceship.obj", E_Collider::SPHERE);
			std::vector<std::wstring> VSs, PSs;
			VSs.push_back(L"VS_PTN.hlsl"); PSs.push_back(L"PS_PTN.hlsl");

			std::vector<size_t> hash_mats = _RenderSystem.CreateMaterials<Vertex_PTN>(L"../Assets/Meshes/spaceship.mtl", VSs, PSs);
			std::vector<TX_HASH> TX_HASHS[1];
			TX_HASHS[0].push_back({ E_Texture::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/stars_spaceship.jpg", WIC_FLAGS_IGNORE_SRGB) });
			_RenderSystem.Material_SetTextures(hash_mats[0], TX_HASHS[0]);

			_RenderSystem.SpaceShip = new TempObj();
			TempObj* obj = _RenderSystem.SpaceShip;
			obj->m_vScale = Vector3(25.0f, 25.0f, 25.0f);
			obj->m_vRotate = Vector3(0.0f, -90.0f, 0.0f);
			obj->m_Mesh_Material.push_back({ hash_mesh , hash_mats[0] });
		}

		//Asteroid
		{
			size_t hash_mesh = _RenderSystem.CreateMesh<Vertex_PTN>(L"../Assets/Meshes/asteroid.obj", E_Collider::SPHERE);
			std::vector<std::wstring> VSs, PSs;
			VSs.push_back(L"VS_PTN.hlsl"); PSs.push_back(L"PS_PTN.hlsl");

			std::vector<size_t> hash_mats = _RenderSystem.CreateMaterials<Vertex_PTN>(L"../Assets/Meshes/asteroid.mtl", VSs, PSs);
			std::vector<TX_HASH> TX_HASHS[1];
			TX_HASHS[0].push_back({ E_Texture::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/stars_asteroid.jpg", WIC_FLAGS_IGNORE_SRGB) });
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
#endif // _SPACESHIP

	}
#endif _TESTBLOCK
	{
		{
			std::vector<std::vector<Vector3>> points;
			std::vector<Vertex_PT> vertices;
			std::vector<UINT> indices;
			GeometryGenerate_Plane(points, vertices, indices);
			size_t hash_mesh = _RenderSystem.CreateMeshFromGeometry<Vertex_PT>(L"Plane", std::move(points), std::move(vertices), std::move(indices), E_Collider::AABB);
			{
				size_t hash_material = _RenderSystem.CreateMaterial<Vertex_PT>(L"Mat_srv", L"VS_PT.hlsl", L"PS_Distortion.hlsl");
				std::vector<TX_HASH> tx_hash;
				tx_hash.push_back({ E_Texture::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/butter7.png", WIC_FLAGS_IGNORE_SRGB) });
				_RenderSystem.Material_SetTextures(hash_material, tx_hash);

				_RenderSystem.ortho_objs.push_back(new TempObj());
				TempObj* obj = _RenderSystem.ortho_objs.back();
				obj->m_vScale = Vector3(m_iWidth, m_iHeight, 1.0f);
				obj->m_vPosition = Vector3(0.0f, 0.0f, 1.0f);
				obj->m_Mesh_Material.push_back({ hash_mesh , hash_material });
			}

			{
				size_t hash_material = _RenderSystem.CreateMaterial<Vertex_PT>(L"Mat_depth", L"VS_PT.hlsl", L"PS_DepthDebug.hlsl");
				std::vector<TX_HASH> tx_hash;
				tx_hash.push_back({ E_Texture::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/butter5.webp", WIC_FLAGS_IGNORE_SRGB) });
				_RenderSystem.Material_SetTextures(hash_material, tx_hash);

				_RenderSystem.ortho_objs.push_back(new TempObj());
				TempObj* obj = _RenderSystem.ortho_objs.back();
				obj->m_vScale = Vector3(300, 300, 1.0f);
				obj->m_vPosition = Vector3(100.0f, 0.0f, 1.0f);
				obj->m_Mesh_Material.push_back({ hash_mesh , hash_material });
			}
		}
	}
#endif // _ECS
}

void AppWindow::OnUpdate()
{
	std::cout << "OnUpdate" << '\n';
	_TimerSystem.Frame();
	float deltaTime = _TimerSystem.GetDeltaTime();
	float elpasedTime = _TimerSystem.GetElapsedTime();
	float FPS = _TimerSystem.GetFps();
	std::cout << "ElapsedTime : " << elpasedTime << '\n';
	std::cout << "FPS : " << FPS << '\n';

	//FrameIntent
	{
		_InputSystem.Frame();
		_BehaviorSystem.Frame(deltaTime);
		_CameraSystem.Frame(deltaTime);
		_ImguiSystem.Frame(deltaTime);
		_CollisionSystem.Frame(deltaTime);
		_RenderSystem.Frame(deltaTime, elpasedTime);
	}
	
	//RenderIntent
	{
		_RenderSystem.PreRender(deltaTime, elpasedTime);
		_RenderSystem.Render(deltaTime, elpasedTime);
		_ImguiSystem.Render();
		_RenderSystem.PostRender();
	}
	
	std::cout << '\n';
}

void AppWindow::OnDestroy()
{
	std::cout << "OnDestroy" << '\n';
	m_bIsRun = false;
}
