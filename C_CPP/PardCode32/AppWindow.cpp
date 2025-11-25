#include "AppWindow.h"
#include "RenderSystem.h"
#include "InputSystem.h"
#include "CameraSystem.h"
#include "ResourceSystem.h"
#include "ImguiSystem.h"
#include "LightSystem.h"
#include "TimerSystem.h"
#include "CollisionSystem.h"
#include "Material.h"			//mtl텍스쳐로딩을위한 임시임포트
#include "GeometryGenerator.h"
#include "ECSSystem.h"
#include "BehaviorSystem.h"

#include "TestBlockMacro.h"

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
		ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Input, C_Transform, C_Behavior, C_Camera, C_Projection, C_Orthographic, T_Camera_Ortho_LT>();
		_CameraSystem.lookup_maincam = _ECSSystem.CreateEntity<C_Input, C_Transform, C_Behavior, C_Camera, C_Projection, C_Orthographic, T_Camera_Ortho_LT>();
		std::bitset<256> vkmask;
		vkmask['W'] = true;
		vkmask['A'] = true;
		vkmask['S'] = true;
		vkmask['D'] = true;
		vkmask['Q'] = true;
		vkmask['E'] = true;
		vkmask[VK_MOUSE_MOVE] = true;
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
		//SkySphere
		{
			size_t hash_mesh = _RenderSystem.CreateMesh<Vertex_PTN>(L"../Assets/Meshes/sphere.obj");
			size_t hash_material = _RenderSystem.CreateMaterial<Vertex_PTN>(L"Mat_SkySphere", L"VS_PTN.hlsl", L"PS_.hlsl");
			std::vector<TX_HASH> tx_hashs;
			tx_hashs.push_back({ E_Texture::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/stars_map.jpg", WIC_FLAGS_IGNORE_SRGB) });
			_RenderSystem.Material_SetTextures(hash_material, tx_hashs);
			std::vector<Mesh_Material> mesh_mats;
			mesh_mats.push_back({ hash_mesh, hash_material });
			size_t hash_ra = _RenderSystem.CreateRenderAsset(L"ra_sky", mesh_mats);

			//ECS Initialize(test, 251111)
			ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Transform, C_Input, C_Behavior, C_Render, T_Render_Sky>();
			size_t lookup = _ECSSystem.CreateEntity<C_Transform, C_Input, C_Behavior, C_Render, T_Render_Sky>();
			std::bitset<256> vkmask;
			vkmask['W'] = true;
			vkmask['A'] = true;
			vkmask['S'] = true;
			vkmask['D'] = true;
			vkmask['Q'] = true;
			vkmask['E'] = true;
			_ECSSystem.AddComponent<C_Input>(key, { vkmask });
			size_t lookup_maincam = _CameraSystem.lookup_maincam;
			const auto& c_cam_main = _ECSSystem.GetComponent<C_Camera>(lookup_maincam);
			_ECSSystem.AddComponent<C_Transform>(key, { {c_cam_main.fFar * 0.9f, c_cam_main.fFar * 0.9f, c_cam_main.fFar * 0.9f},  {}, {} });

			std::array<unsigned char, E_Behavior::COUNT> behavior;
			behavior[E_Behavior::MOVE_FORWARD] = 'W';
			behavior[E_Behavior::MOVE_LEFT] = 'A';
			behavior[E_Behavior::MOVE_BACKWARD] = 'S';
			behavior[E_Behavior::MOVE_RIGHT] = 'D';
			behavior[E_Behavior::MOVE_UP] = 'Q';
			behavior[E_Behavior::MOVE_DOWN] = 'E';
			_ECSSystem.AddComponent<C_Behavior>(key, { behavior });

			_ECSSystem.AddComponent<C_Render>(key, { true, hash_ra });
		}

		//rand obj
		{
			size_t hash_mesh = _RenderSystem.CreateMesh<Vertex_PTN>(L"../Assets/Meshes/cube.obj");
			size_t hash_material = _RenderSystem.CreateMaterial<Vertex_PTN>(L"Mat_rand0", L"VS_PTN.hlsl", L"PS_PTN.hlsl");
			std::vector<TX_HASH> tx_hashs;
			tx_hashs.push_back({ E_Texture::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/butter6.webp", WIC_FLAGS_NONE) });
			_RenderSystem.Material_SetTextures(hash_material, tx_hashs);
			std::vector<Mesh_Material> mesh_mats;
			mesh_mats.push_back({ hash_mesh, hash_material });
			size_t hash_ra = _RenderSystem.CreateRenderAsset(L"ra0", mesh_mats);

			const std::unordered_set<size_t>& hash_CLs = _RenderSystem.CreateColliders<Vertex_PTN>(hash_mesh, E_Collider::AABB);
			size_t hash_ca = _RenderSystem.CreateColliderAsset(L"ca0", hash_CLs);

			ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Transform, C_Input, C_Behavior, C_Render, C_Collider, T_Render_Geometry>();

			{
				for (int i = 0; i < 500; i++)
				{
					size_t lookup = _ECSSystem.CreateEntity<C_Transform, C_Input, C_Behavior, C_Render, C_Collider, T_Render_Geometry>();
					std::bitset<256> vkmask;
					vkmask[VK_UP] = true;
					//vkmask[VK_LEFT] = true;
					vkmask[VK_DOWN] = true;
					//vkmask[VK_RIGHT] = true;
					_ECSSystem.AddComponent<C_Input>(key, { vkmask });

					_ECSSystem.AddComponent<C_Transform>(key, { {5.0f, 5.0f, 5.0f},  Quarternion(dis(gen) * 10.0f, dis(gen) * 10.0f, dis(gen) * 10.0f), {(float)lookup, (float)lookup, (float)lookup}});

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
		
		{
			size_t hash_mesh = _RenderSystem.CreateMesh<Vertex_PTN>(L"../Assets/Meshes/sphere.obj");
			size_t hash_material = _RenderSystem.CreateMaterial<Vertex_PTN>(L"Mat_rand1", L"VS_PTN.hlsl", L"PS_PTN.hlsl");
			std::vector<TX_HASH> tx_hashs;
			tx_hashs.push_back({ E_Texture::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/butter5.webp", WIC_FLAGS_NONE) });
			_RenderSystem.Material_SetTextures(hash_material, tx_hashs);
			std::vector<Mesh_Material> mesh_mats;
			mesh_mats.push_back({ hash_mesh, hash_material });
			size_t hash_ra = _RenderSystem.CreateRenderAsset(L"ra1", mesh_mats);

			const std::unordered_set<size_t>& hash_CLs = _RenderSystem.CreateColliders<Vertex_PTN>(hash_mesh, E_Collider::SPHERE);
			size_t hash_ca = _RenderSystem.CreateColliderAsset(L"ca1", hash_CLs);

			ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Transform, C_Input, C_Behavior, C_Render, C_Collider, T_Render_Geometry>();

			{
				for (int i = 0; i < 500; i++)
				{
					size_t lookup = _ECSSystem.CreateEntity<C_Transform, C_Input, C_Behavior, C_Render, C_Collider, T_Render_Geometry>();
					std::bitset<256> vkmask;
					//vkmask[VK_UP] = true;
					vkmask[VK_LEFT] = true;
					//vkmask[VK_DOWN] = true;
					vkmask[VK_RIGHT] = true;
					_ECSSystem.AddComponent<C_Input>(key, { vkmask });

					_ECSSystem.AddComponent<C_Transform>(key, { {5.0f, 5.0f, 5.0f}, Quarternion(dis(gen) * 10.0f, dis(gen) * 10.0f, dis(gen) * 10.0f), {(float)lookup,(float)lookup,(float)lookup} });

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

		//Normal Map
		{
			size_t hash_mesh = _RenderSystem.CreateMesh<Vertex_PTNTB>(L"../Assets/Meshes/sphere.obj");
			size_t hash_material = _RenderSystem.CreateMaterial<Vertex_PTNTB>(L"Mat_NormalMapping", L"VS_PTNTB.hlsl", L"PS_PTNTB.hlsl");
			std::vector<TX_HASH> tx_hashs;
			tx_hashs.push_back({ E_Texture::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/brick_d.jpg", WIC_FLAGS_IGNORE_SRGB) });
			tx_hashs.push_back({ E_Texture::Normal, _RenderSystem.CreateTexture(L"../Assets/Textures/brick_n.jpg", WIC_FLAGS_IGNORE_SRGB) });
			_RenderSystem.Material_SetTextures(hash_material, tx_hashs);
			std::vector<Mesh_Material> mesh_mats;
			mesh_mats.push_back({ hash_mesh, hash_material });
			size_t hash_ra = _RenderSystem.CreateRenderAsset(L"ra_normal", mesh_mats);

			const std::unordered_set<size_t>& hash_CLs = _RenderSystem.CreateColliders<Vertex_PTNTB>(hash_mesh, E_Collider::SPHERE);
			size_t hash_ca = _RenderSystem.CreateColliderAsset(L"ca_normal", hash_CLs);

			//ECS Initialize(test, 251111)
			ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Transform, C_Render, C_Collider, T_Render_Geometry>();
			size_t lookup = _ECSSystem.CreateEntity<C_Transform, C_Render, C_Collider, T_Render_Geometry>();

			_ECSSystem.AddComponent<C_Transform>(key, { {5.0f, 5.0f, 5.0f}, {}, {100.0f, 0.0f, 0.0f} });

			_ECSSystem.AddComponent<C_Render>(key, { true, hash_ra });

			_ECSSystem.AddComponent<C_Collider>(key, { hash_ca });
		}

		//nene
		{
			size_t hash_mesh = _RenderSystem.CreateMesh<Vertex_PTN>(L"../Assets/Meshes/nene.obj");
			
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

			const std::unordered_set<size_t>& hash_CLs = _RenderSystem.CreateColliders<Vertex_PTN>(hash_mesh, E_Collider::SPHERE);
			size_t hash_ca = _RenderSystem.CreateColliderAsset(L"ca2", hash_CLs);

			//ECS Initialize(test, 251111)
			ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Transform, C_Render, C_Collider, T_Render_Geometry>();

			size_t lookup = _ECSSystem.CreateEntity<C_Transform, C_Render, C_Collider, T_Render_Geometry>();

			_ECSSystem.AddComponent<C_Transform>(key, { {5.0f, 5.0f, 5.0f}, Quarternion(0.0f, 0.0f, 0.0f), {0.0f, 0.0f, 0.0f} });

			_ECSSystem.AddComponent<C_Render>(key, { true, hash_ra });

			_ECSSystem.AddComponent<C_Collider>(key, { hash_ca });
		}
	}

	//Initialize RTV, DSV
	{
		//Init TargetView geometry
		std::vector<std::vector<Vector3>> points;
		std::vector<Vertex_PT> vertices;
		std::vector<UINT> indices;
		GeometryGenerate_Plane(points, vertices, indices);
		size_t hash_mesh = _RenderSystem.CreateMeshFromGeometry<Vertex_PT>(L"Plane", std::move(points), std::move(vertices), std::move(indices));
		//RTV
		{
			size_t hash_material = _RenderSystem.CreateMaterial<Vertex_PT>(L"Mat_RTV", L"VS_PT.hlsl", L"PS_Distortion.hlsl");
			std::vector<TX_HASH> tx_hashs;
			tx_hashs.push_back({ E_Texture::Diffuse, _RenderSystem.m_hash_RTV_0 });
			_RenderSystem.Material_SetTextures(hash_material, tx_hashs);
			std::vector<Mesh_Material> mesh_mats;
			mesh_mats.push_back({ hash_mesh, hash_material });
			size_t hash_ra = _RenderSystem.CreateRenderAsset(L"ra_rtv", mesh_mats);

			ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Transform, C_Render, T_Render_UI>();
			size_t lookup = _ECSSystem.CreateEntity<C_Transform, C_Render, T_Render_UI>();

			_ECSSystem.AddComponent<C_Transform>(key, { {(float)m_iWidth, (float)m_iHeight, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f} });
			_ECSSystem.AddComponent<C_Render>(key, { true, hash_ra });
		}
		//DSV
		{
			size_t hash_material = _RenderSystem.CreateMaterial<Vertex_PT>(L"Mat_Depth", L"VS_PT.hlsl", L"PS_DepthDebug.hlsl");
			std::vector<TX_HASH> tx_hashs;
			tx_hashs.push_back({ E_Texture::Diffuse, _RenderSystem.m_hash_DSV_0 });
			_RenderSystem.Material_SetTextures(hash_material, tx_hashs);
			std::vector<Mesh_Material> mesh_mats;
			mesh_mats.push_back({ hash_mesh, hash_material });
			size_t hash_ra = _RenderSystem.CreateRenderAsset(L"ra_dsv", mesh_mats);

			ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Transform, C_Render, T_Render_UI>();
			size_t lookup = _ECSSystem.CreateEntity<C_Transform, C_Render, T_Render_UI>();

			_ECSSystem.AddComponent<C_Transform>(key, { {300.0f, 300.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f} });
			_ECSSystem.AddComponent<C_Render>(key, { true, hash_ra });
		}
	}
#ifdef _ECS
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

	//PardCode25(house)
	{
		size_t hash_mesh = _RenderSystem.CreateMesh<Vertex_PTN>(L"../Assets/Meshes/house.obj", E_Collider::AABB);
		std::vector<std::wstring> VSs, PSs;
		VSs.push_back(L"VS_PTN.hlsl"); VSs.push_back(L"VS_PTN.hlsl"); VSs.push_back(L"VS_PTN.hlsl"); VSs.push_back(L"VS_PTN.hlsl");
		PSs.push_back(L"PS_PTN.hlsl"); PSs.push_back(L"PS_PTN.hlsl"); PSs.push_back(L"PS_PTN.hlsl"); PSs.push_back(L"PS_PTN.hlsl");
		std::vector<size_t> hash_materials = _RenderSystem.CreateMaterials<Vertex_PTN>(L"../Assets/Meshes/house.mtl", VSs, PSs);
		std::vector<TX_HASH> hash_tx_hash[4];
		hash_tx_hash[0].push_back({ E_Texture::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/house_barrel.jpg", WIC_FLAGS_NONE) });
		_RenderSystem.Material_SetTextures(hash_materials[0], hash_tx_hash[0]);
		hash_tx_hash[1].push_back({ E_Texture::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/house_brick.jpg", WIC_FLAGS_NONE) });
		_RenderSystem.Material_SetTextures(hash_materials[1], hash_tx_hash[1]);
		hash_tx_hash[2].push_back({ E_Texture::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/house_windows.jpg", WIC_FLAGS_NONE) });
		_RenderSystem.Material_SetTextures(hash_materials[2], hash_tx_hash[2]);
		hash_tx_hash[3].push_back({ E_Texture::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/house_wood.jpg", WIC_FLAGS_NONE) });
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
	//girlobj
	{
		size_t hash_mesh = _RenderSystem.CreateMesh<Vertex_PTN>(L"../Assets/Meshes/girl.obj", E_Collider::SPHERE);
		std::vector<std::wstring> VSs, PSs;
		VSs.push_back(L"VS_PTN.hlsl"); VSs.push_back(L"VS_PTN.hlsl"); VSs.push_back(L"VS_PTN.hlsl");
		VSs.push_back(L"VS_PTN.hlsl"); VSs.push_back(L"VS_PTN.hlsl"); VSs.push_back(L"VS_PTN.hlsl"); VSs.push_back(L"VS_PTN.hlsl");
		PSs.push_back(L"PS_PTN.hlsl"); PSs.push_back(L"PS_PTN.hlsl"); PSs.push_back(L"PS_PTN.hlsl");
		PSs.push_back(L"PS_PTN.hlsl"); PSs.push_back(L"PS_PTN.hlsl"); PSs.push_back(L"PS_PTN.hlsl"); PSs.push_back(L"PS_PTN.hlsl");
		std::vector<size_t> hash_materials = _RenderSystem.CreateMaterials<Vertex_PTN>(L"../Assets/Meshes/girl.mtl", VSs, PSs);

		std::vector<TX_HASH> hash_tx_hash[7];
		auto pMat = _ResourceSystem.GetResource<Material>(hash_materials[0]);
		hash_tx_hash[0].push_back({ E_Texture::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/" + _tomw(pMat->GetTexturesPaths()[0][0]), WIC_FLAGS_NONE) });
		//txs_bot[0].push_back({ E_Texture::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/tEXTURE/bot color.jpg", WIC_FLAGS_NONE) });
		_RenderSystem.Material_SetTextures(hash_materials[0], hash_tx_hash[0]);

		pMat = _ResourceSystem.GetResource<Material>(hash_materials[1]);
		hash_tx_hash[1].push_back({ E_Texture::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/" + _tomw(pMat->GetTexturesPaths()[0][0]), WIC_FLAGS_NONE) });
		//txs_bot[1].push_back({ E_Texture::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/tEXTURE/BOdy Skin Base Color.png", WIC_FLAGS_NONE) });
		_RenderSystem.Material_SetTextures(hash_materials[1], hash_tx_hash[1]);

		pMat = _ResourceSystem.GetResource<Material>(hash_materials[2]);
		hash_tx_hash[2].push_back({ E_Texture::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/" + _tomw(pMat->GetTexturesPaths()[0][0]) , WIC_FLAGS_NONE) });
		//hash_tx_hash[2].push_back({ E_Texture::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/tEXTURE/FACE Base Color alpha.png", WIC_FLAGS_NONE) });
		_RenderSystem.Material_SetTextures(hash_materials[2], hash_tx_hash[2]);

		hash_tx_hash[3].push_back({ E_Texture::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/tEXTURE/COLORS.jpg", WIC_FLAGS_NONE) });
		_RenderSystem.Material_SetTextures(hash_materials[3], hash_tx_hash[3]);

		hash_tx_hash[4].push_back({ E_Texture::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/tEXTURE/BOdy Skin Base Color.png", WIC_FLAGS_NONE) });
		_RenderSystem.Material_SetTextures(hash_materials[4], hash_tx_hash[4]);

		hash_tx_hash[5].push_back({ E_Texture::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/tEXTURE/COLORS.jpg", WIC_FLAGS_NONE) });
		_RenderSystem.Material_SetTextures(hash_materials[5], hash_tx_hash[5]);

		hash_tx_hash[6].push_back({ E_Texture::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/tEXTURE/top color.png", WIC_FLAGS_NONE) });
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

	//BlendCheck
	{
		size_t hash_mesh = _RenderSystem.CreateMesh<Vertex_PTN>(L"../Assets/Meshes/Cube.obj", E_Collider::AABB);
		size_t hash_material = _RenderSystem.CreateMaterial<Vertex_PTN>(L"../Assets/Meshes/house.mtl", L"VS_PTN.hlsl", L"PS_PTN.hlsl");
		std::vector<TX_HASH> hash_tx_hash;
		hash_tx_hash.push_back({ E_Texture::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/wireFence.dds", DDS_FLAGS_NONE) });
		_RenderSystem.Material_SetTextures(hash_material, hash_tx_hash);

		_RenderSystem.objs.push_back(new TempObj());
		TempObj* obj = _RenderSystem.objs.back();
		obj->m_vScale = Vector3(300.0f, 300.0f, 300.0f);
		obj->m_vRotate = Vector3(0.0f, 0.0f, 0.0f);
		obj->m_vPosition = Vector3(0.0f, 0.0f, 0.0f);
		obj->m_Mesh_Material.push_back({ hash_mesh , hash_material });
	}

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
