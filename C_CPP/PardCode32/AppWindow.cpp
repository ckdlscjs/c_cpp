#include "AppWindow.h"
#include "RenderSystem.h"
#include "InputSystem.h"
#include "CameraSystem.h"
#include "ResourceSystem.h"
#include "ImguiSystem.h"
#include "LightSystem.h"
#include "TimerSystem.h"
#include "CollisionSystem.h"
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
const std::wstring g_szName_mat = L"Mat_";
const std::wstring g_szName_ra = L"Ra_";
const std::wstring g_szName_ca = L"Ca_";
UINT g_iWidth = 800.0f;
UINT g_iHeight = 600.0f;

void AppWindow::OnCreate()
{
	std::cout << "OnCreate" << '\n';
	_TimerSystem.Init();
	_InputSystem.Init();
	_ResourceSystem.Init();
	_CollisionSystem.Init();
	_RenderSystem.Init(m_hWnd, g_iWidth, g_iHeight);
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
		//기본카메라 세팅
		{
			const std::wstring szName = L"MainCam";
			ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Info, C_Input, C_Transform, C_Behavior, C_Camera, C_Projection, C_Orthographic, T_Camera_Ortho_LT>();
			_CameraSystem.lookup_maincam = _ECSSystem.CreateEntity<C_Info, C_Input, C_Transform, C_Behavior, C_Camera, C_Projection, C_Orthographic, T_Camera_Ortho_LT>();

			_ECSSystem.AddComponent<C_Info>(key, { szName });

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
			_ECSSystem.AddComponent<C_Transform>(key, { {1.0f, 1.0f, 1.0f}, Quarternion(dir), pos });

			std::array<unsigned char, E_Behavior::COUNT> behavior;
			behavior[E_Behavior::MOVE_FORWARD] = 'W';
			behavior[E_Behavior::MOVE_LEFT] = 'A';
			behavior[E_Behavior::MOVE_BACKWARD] = 'S';
			behavior[E_Behavior::MOVE_RIGHT] = 'D';
			behavior[E_Behavior::MOVE_UP] = 'Q';
			behavior[E_Behavior::MOVE_DOWN] = 'E';
			_ECSSystem.AddComponent<C_Behavior>(key, { behavior });

			C_Camera camera;
			camera.fScreenWidth = g_iWidth;
			camera.fScreenHeight = g_iHeight;
			camera.fFov = 75.0f;
			camera.fNear = 0.1f;
			camera.fFar = 10000.0f;
			_ECSSystem.AddComponent<C_Camera>(key, std::move(camera));
		}
	}

	//Light Initialize
	{
		//Directional Light
		{
			ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Light, C_Light_Direction, T_Light_Directional>();
			_LightSystem.lookup_directional = _ECSSystem.CreateEntity<C_Light, C_Light_Direction, T_Light_Directional>();

			_ECSSystem.AddComponent<C_Light>(key, { Vector4(0.3f, 0.3f, 0.3f, 1.0f) , Vector4(0.7f, 0.7f, 0.7f, 1.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), 100.0f });
			_ECSSystem.AddComponent<C_Light_Direction>(key, { Vector3(0.0f, -1.0f, 1.0f) });
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
		//gizmo
		{
			/*std::vector<std::vector<Vector3>> points;
			std::vector<Vertex_PC> vertices;
			std::vector<UINT> indices;
			GeometryGenerate_Gizmo(points, vertices, indices);
			size_t hash_mesh = _RenderSystem.CreateMeshFromGeometry<Vertex_PC>(L"Gizmo", std::move(points), std::move(vertices), std::move(indices));
			{

			}*/
		}


		//SkySphere
		{
			const std::wstring szName = L"SkySphere";
			size_t hash_geometry = _RenderSystem.CreateGeometry(L"../Assets/Meshes/sphere.obj");
			size_t hash_mesh = _RenderSystem.CreateMeshFromGeometry<Vertex_PTN>(hash_geometry);
			size_t hash_material = _RenderSystem.CreateMaterial(g_szName_mat + szName);
			_RenderSystem.Material_SetVS(hash_material, L"VS_PTN.hlsl");
			_RenderSystem.Material_SetPS(hash_material, L"PS_.hlsl");
			_RenderSystem.Material_SetIL<Vertex_PTN>(hash_material, L"VS_PTN.hlsl");

			std::vector<TX_HASH> tx_hashs;
			tx_hashs.push_back({ E_Texture::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/butter8.png") });
			_RenderSystem.Material_SetTextures(hash_material, tx_hashs);
			std::vector<Mesh_Material> mesh_mats;
			mesh_mats.push_back({ hash_mesh, hash_material });
			size_t hash_ra = _RenderSystem.CreateRenderAsset(g_szName_ra + szName, mesh_mats);

			//ECS Initialize(test, 251111)
			ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Info, C_Transform, C_Input, C_Behavior, C_Render, T_Render_Sky>();
			size_t lookup = _ECSSystem.CreateEntity<C_Info, C_Transform, C_Input, C_Behavior, C_Render, T_Render_Sky>();

			_ECSSystem.AddComponent<C_Info>(key, { szName });

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

		//floor
		{
			const std::wstring szName = L"floor";
			std::map<UINT, std::vector<Vertex_PTNTB_Skinned>> verticesByMaterial;
			std::map<UINT, std::vector<UINT>> indicesByMaterial;
			std::vector<std::vector<Vector3>> pointsByMeshs;
			GeometryGenerate_Plane<Vertex_PTN>(pointsByMeshs, verticesByMaterial[0], indicesByMaterial[0]);

			size_t hash_mesh = _RenderSystem.CreateMeshFromGeometry<Vertex_PTN>(szName, verticesByMaterial, indicesByMaterial, pointsByMeshs);
			_RenderSystem.CreateColliders(hash_mesh, E_Collider::AABB);

			size_t hash_material = _RenderSystem.CreateMaterial(g_szName_mat + szName);
			_RenderSystem.Material_SetVS(hash_material, L"VS_PTN.hlsl");
			_RenderSystem.Material_SetPS(hash_material, L"PS_PTN.hlsl");
			_RenderSystem.Material_SetIL<Vertex_PTN>(hash_material, L"VS_PTN.hlsl");

			std::vector<TX_HASH> tx_hashs;
			tx_hashs.push_back({ E_Texture::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/butter8.png") });
			_RenderSystem.Material_SetTextures(hash_material, tx_hashs);
			std::vector<Mesh_Material> mesh_mats;
			mesh_mats.push_back({ hash_mesh, hash_material });
			size_t hash_ra = _RenderSystem.CreateRenderAsset(g_szName_ra + szName, mesh_mats);

			ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Info, C_Transform, C_Render, T_Collider, T_Render_Geometry_Static>();
			size_t lookup = _ECSSystem.CreateEntity<C_Info, C_Transform, C_Render, T_Collider, T_Render_Geometry_Static>();

			_ECSSystem.AddComponent<C_Info>(key, { szName });

			_ECSSystem.AddComponent<C_Transform>(key, { {500.0f, 500.0f, 1.0f}, {Quarternion(90.0f, 0.0f, 0.0f)}, {0.0f, 0.0f, 0.0f} });

			_ECSSystem.AddComponent<C_Render>(key, { true, hash_ra });
		}
#ifdef _RandOBJ
		const size_t cnt = 500;

		//rand obj
		{
			const std::wstring szName = L"RandCube";
			size_t hash_geometry = _RenderSystem.CreateGeometry(L"../Assets/Meshes/cube.obj");

			size_t hash_mesh = _RenderSystem.CreateMeshFromGeometry<Vertex_PTN>(hash_geometry);
			_RenderSystem.CreateColliders(hash_mesh, E_Collider::AABB);

			size_t hash_material = _RenderSystem.CreateMaterial(g_szName_mat + szName);
			_RenderSystem.Material_SetVS(hash_material, L"VS_PTN.hlsl");
			_RenderSystem.Material_SetIL<Vertex_PTN>(hash_material, L"VS_PTN.hlsl");
			_RenderSystem.Material_SetGS(hash_material, L"GS_PTN.hlsl");
			_RenderSystem.Material_SetPS(hash_material, L"PS_PTN.hlsl");

			std::vector<TX_HASH> tx_hashs;
			tx_hashs.push_back({ E_Texture::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/butter6.webp") });
			_RenderSystem.Material_SetTextures(hash_material, tx_hashs);
			std::vector<Mesh_Material> mesh_mats;
			mesh_mats.push_back({ hash_mesh, hash_material });
			size_t hash_ra = _RenderSystem.CreateRenderAsset(g_szName_ra + szName, mesh_mats);

			
			ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Transform, C_Input, C_Behavior, C_Render, T_Collider, T_Render_Geometry_Static>();

			{
				for (int i = 0; i < cnt; i++)
				{
					size_t lookup = _ECSSystem.CreateEntity<C_Transform, C_Input, C_Behavior, C_Render, T_Collider, T_Render_Geometry_Static>();
					std::bitset<256> vkmask;
					vkmask[VK_UP] = true;
					//vkmask[VK_LEFT] = true;
					vkmask[VK_DOWN] = true;
					//vkmask[VK_RIGHT] = true;
					_ECSSystem.AddComponent<C_Input>(key, { vkmask });

					_ECSSystem.AddComponent<C_Transform>(key, { {5.0f, 5.0f, 5.0f},  Quarternion(dis(gen) * 10.0f, dis(gen) * 10.0f, dis(gen) * 10.0f), {(float)lookup, (float)lookup, (float)lookup} });

					std::array<unsigned char, E_Behavior::COUNT> behavior;
					behavior[E_Behavior::MOVE_FORWARD] = VK_UP;
					behavior[E_Behavior::MOVE_LEFT] = VK_LEFT;
					behavior[E_Behavior::MOVE_BACKWARD] = VK_DOWN;
					behavior[E_Behavior::MOVE_RIGHT] = VK_RIGHT;
					_ECSSystem.AddComponent<C_Behavior>(key, { behavior });

					_ECSSystem.AddComponent<C_Render>(key, { true, hash_ra });
				}
			}
		}

		{
			const std::wstring szName = L"RandSphere";
			size_t hash_geometry = _RenderSystem.CreateGeometry(L"../Assets/Meshes/sphere.obj");

			size_t hash_mesh = _RenderSystem.CreateMeshFromGeometry<Vertex_PTN>(hash_geometry);
			_RenderSystem.CreateColliders(hash_mesh, E_Collider::SPHERE);

			size_t hash_material = _RenderSystem.CreateMaterial(g_szName_mat + szName);
			_RenderSystem.Material_SetVS(hash_material, L"VS_PTN.hlsl");
			_RenderSystem.Material_SetPS(hash_material, L"PS_PTN.hlsl");
			_RenderSystem.Material_SetIL<Vertex_PTN>(hash_material, L"VS_PTN.hlsl");

			std::vector<TX_HASH> tx_hashs;
			tx_hashs.push_back({ E_Texture::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/butter5.webp") });
			_RenderSystem.Material_SetTextures(hash_material, tx_hashs);
			std::vector<Mesh_Material> mesh_mats;
			mesh_mats.push_back({ hash_mesh, hash_material });
			size_t hash_ra = _RenderSystem.CreateRenderAsset(g_szName_ra + szName, mesh_mats);
			

			ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Transform, C_Input, C_Behavior, C_Render, T_Collider, T_Render_Geometry_Static>();

			{
				for (int i = 0; i < cnt; i++)
				{
					size_t lookup = _ECSSystem.CreateEntity<C_Transform, C_Input, C_Behavior, C_Render, T_Collider, T_Render_Geometry_Static>();
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
				}
			}
		}
#endif // _RandOBJ

#ifdef _Billboard
		//Billboard
		{
			const std::wstring szName = L"Billboard";
			std::map<UINT, std::vector<Vertex_PTNTB_Skinned>> verticesByMaterial;
			std::map<UINT, std::vector<UINT>> indicesByMaterial;
			std::vector<std::vector<Vector3>> pointsByMeshs;
			GeometryGenerate_Plane<Vertex_PTN>(pointsByMeshs, verticesByMaterial[0], indicesByMaterial[0]);

			size_t hash_mesh = _RenderSystem.CreateMeshFromGeometry<Vertex_PTN>(szName, verticesByMaterial, indicesByMaterial, pointsByMeshs);
			_RenderSystem.CreateColliders(hash_mesh, E_Collider::AABB);

			size_t hash_material = _RenderSystem.CreateMaterial(g_szName_mat + szName);
			_RenderSystem.Material_SetVS(hash_material, L"VS_PTN.hlsl");
			_RenderSystem.Material_SetPS(hash_material, L"PS_PTN.hlsl");
			_RenderSystem.Material_SetIL<Vertex_PTN>(hash_material, L"VS_PTN.hlsl");

			std::vector<TX_HASH> tx_hashs;
			tx_hashs.push_back({ E_Texture::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/butter8.png") });
			_RenderSystem.Material_SetTextures(hash_material, tx_hashs);
			std::vector<Mesh_Material> mesh_mats;
			mesh_mats.push_back({ hash_mesh, hash_material });
			size_t hash_ra = _RenderSystem.CreateRenderAsset(g_szName_ra + szName, mesh_mats);
			

			ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Transform, C_Render, T_Collider, T_Render_Billboard>();
			size_t lookup = _ECSSystem.CreateEntity<C_Transform, C_Render, T_Collider, T_Render_Billboard>();

			_ECSSystem.AddComponent<C_Transform>(key, { {50.0f, 50.0f, 1.0f}, {}, {150.0f, 50.0f, -30.0f} });

			_ECSSystem.AddComponent<C_Render>(key, { true, hash_ra });
		}
#endif // _Billboard

#ifdef _NORMALMAP
		{
			const std::wstring szName = L"NormalMap";
			size_t hash_geometry = _RenderSystem.CreateGeometry(L"../Assets/Meshes/sphere.obj");

			size_t hash_mesh = _RenderSystem.CreateMeshFromGeometry<Vertex_PTNTB>(hash_geometry);
			_RenderSystem.CreateColliders(hash_mesh, E_Collider::SPHERE);

			size_t hash_material = _RenderSystem.CreateMaterial(g_szName_mat + szName);
			_RenderSystem.Material_SetVS(hash_material, L"VS_PTNTB.hlsl");
			_RenderSystem.Material_SetPS(hash_material, L"PS_PTNTB.hlsl");
			_RenderSystem.Material_SetIL<Vertex_PTNTB>(hash_material, L"VS_PTNTB.hlsl");

			std::vector<TX_HASH> tx_hashs;
			tx_hashs.push_back({ E_Texture::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/brick_d.jpg") });
			tx_hashs.push_back({ E_Texture::Normal, _RenderSystem.CreateTexture(L"../Assets/Textures/brick_n.jpg") });
			_RenderSystem.Material_SetTextures(hash_material, tx_hashs);
			std::vector<Mesh_Material> mesh_mats;
			mesh_mats.push_back({ hash_mesh, hash_material });
			size_t hash_ra = _RenderSystem.CreateRenderAsset(g_szName_ra + szName, mesh_mats);
			
			//ECS Initialize(test, 251111)
			ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Transform, C_Render, T_Collider, T_Render_Geometry_Static >();
			size_t lookup = _ECSSystem.CreateEntity<C_Transform, C_Render, T_Collider, T_Render_Geometry_Static>();

			_ECSSystem.AddComponent<C_Transform>(key, { {50.0f, 50.0f, 50.0f}, {}, {230.0f, 100.0f, 0.0f} });

			_ECSSystem.AddComponent<C_Render>(key, { true, hash_ra });
		}
#endif // _NORMALMAP

#ifdef _HOUSE
		{
			const std::wstring szName = L"House";
			size_t hash_geometry = _RenderSystem.CreateGeometry(L"../Assets/Meshes/house.obj");

			size_t hash_mesh = _RenderSystem.CreateMeshFromGeometry<Vertex_PTNTB>(hash_geometry);
			_RenderSystem.CreateColliders(hash_mesh, E_Collider::AABB);

			size_t hash_material = _RenderSystem.CreateMaterial(g_szName_mat + szName);

			std::vector<size_t> hashs_material = _RenderSystem.CreateMaterialsFromGeometry(hash_geometry);
			std::vector<Mesh_Material> mesh_mats;
			for (const auto& iter : hashs_material)
				mesh_mats.push_back({ hash_mesh, iter });

			std::vector<std::wstring> paths =
			{
				L"../Assets/Textures/house_barrel.jpg",
				L"../Assets/Textures/house_brick.jpg",
				L"../Assets/Textures/house_windows.jpg",
				L"../Assets/Textures/house_wood.jpg"
			};

			for (int i = 0; i < hashs_material.size(); i++)
			{
				std::vector<TX_HASH> tx_hash;
				tx_hash.push_back({ E_Texture::Diffuse, _RenderSystem.CreateTexture(paths[i]) });
				_RenderSystem.Material_SetTextures(hashs_material[i], tx_hash);
			}

			size_t hash_ra = _RenderSystem.CreateRenderAsset(g_szName_ra + szName, mesh_mats);

			//ECS Initialize(test, 251111)
			ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Transform, C_Render, T_Collider, T_Render_Geometry_Static>();

			size_t lookup = _ECSSystem.CreateEntity<C_Transform, C_Render, T_Collider, T_Render_Geometry_Static>();

			_ECSSystem.AddComponent<C_Transform>(key, { {15.0f, 15.0f, 15.0f}, Quarternion(0.0f, 0.0f, 0.0f), {150.0f, 0.0f, 50.0f} });

			_ECSSystem.AddComponent<C_Render>(key, { true, hash_ra });
		}
#endif // _HOUSE

#ifdef _SPONZA
		{
			const std::wstring szName = L"Sponza";
			size_t hash_geometry = _RenderSystem.CreateGeometry(L"../Assets/Meshes/sponza_basic.obj");

			size_t hash_mesh = _RenderSystem.CreateMeshFromGeometry<Vertex_PTN>(hash_geometry);
			_RenderSystem.CreateColliders(hash_mesh, E_Collider::AABB);

			std::vector<size_t> hashs_material = _RenderSystem.CreateMaterialsFromGeometry(hash_geometry);
			std::vector<Mesh_Material> mesh_mats;
			for (const auto& iter : hashs_material)
				mesh_mats.push_back({ hash_mesh, iter });
		
			std::vector<std::wstring> paths =
			{
				L"../Assets/Textures/sponza_bricks_a_diff.jpg",
				L"../Assets/Textures/sponza_arch_diff.jpg",
				L"../Assets/Textures/sponza_column_a_diff.jpg",
				L"../Assets/Textures/sponza_column_b_diff.jpg",
				L"../Assets/Textures/sponza_column_c_diff.jpg",
				L"../Assets/Textures/sponza_flagpole_diff.jpg"
			};
			for (int i = 0; i < hashs_material.size(); i++)
			{
				std::vector<TX_HASH> tx_hash;
				tx_hash.push_back({ E_Texture::Diffuse, _RenderSystem.CreateTexture(paths[i])});
				_RenderSystem.Material_SetTextures(hashs_material[i], tx_hash);
			}

			size_t hash_ra = _RenderSystem.CreateRenderAsset(g_szName_ra + szName, mesh_mats);
			
			//ECS Initialize(test, 251111)
			ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Transform, C_Render, T_Collider, T_Render_Geometry_Static>();

			size_t lookup = _ECSSystem.CreateEntity<C_Transform, C_Render, T_Collider, T_Render_Geometry_Static>();

			_ECSSystem.AddComponent<C_Transform>(key, { {5.0f, 5.0f, 5.0f}, Quarternion(0.0f, 45.0f, 0.0f), {100.0f, 0.0f, -150.0f} });

			_ECSSystem.AddComponent<C_Render>(key, { true, hash_ra });
		}
#endif // _SPONZA

#ifdef _MECHAGIRL
		//MechaGirl
		{
			const std::wstring szName = L"MechaGirl";
			size_t hash_geometry = _RenderSystem.CreateGeometry(L"../Assets/Meshes/MechanicGirl/Mechanic_Girl-85698ecb/fbx/FBX/SK_MechanicGirl_AllPartsTogether.fbx");

			size_t hash_mesh = _RenderSystem.CreateMeshFromGeometry<Vertex_PTNTB>(hash_geometry);
			_RenderSystem.CreateColliders(hash_mesh, E_Collider::SPHERE);

			std::vector<size_t> hashs_material = _RenderSystem.CreateMaterialsFromGeometry(hash_geometry);
			std::vector<Mesh_Material> mesh_mats;
			for (const auto& iter : hashs_material)
				mesh_mats.push_back({ hash_mesh, iter });

			std::vector<std::wstring> paths =
			{
				L"../Assets/Textures/butter.dds",
				L"../Assets/Textures/butter.dds",
				L"../Assets/Textures/butter.dds",
				L"../Assets/Textures/butter.dds",
				L"../Assets/Textures/butter.dds",
				L"../Assets/Textures/butter.dds",
				L"../Assets/Textures/butter.dds",
				L"../Assets/Textures/butter.dds",
				L"../Assets/Textures/butter.dds",
			};
			for (int i = 0; i < hashs_material.size(); i++)
			{
				std::vector<TX_HASH> tx_hash;
				tx_hash.push_back({ E_Texture::Diffuse, _RenderSystem.CreateTexture(paths[i]) });
				_RenderSystem.Material_SetTextures(hashs_material[i], tx_hash);
			}
			
			size_t hash_ra = _RenderSystem.CreateRenderAsset(g_szName_ra + szName, mesh_mats);
			
			//ECS Initialize(test, 251111)
			ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Transform, C_Render, T_Collider, T_Render_Geometry_Static>();

			size_t lookup = _ECSSystem.CreateEntity<C_Transform, C_Render, T_Collider, T_Render_Geometry_Static>();

			_ECSSystem.AddComponent<C_Transform>(key, { {0.5f, 0.5f, 0.5f}, Quarternion(0.0f, 90.0f, 0.0f), {0.0f, 0.0f, -50.0f} });

			_ECSSystem.AddComponent<C_Render>(key, { true, hash_ra });
		}
#endif // _MECHAGIRL
	}

#ifdef _NENE
	{
		const std::wstring szName = L"Nene";
		size_t hash_geometry = _RenderSystem.CreateGeometry(L"../Assets/Meshes/nene.obj");

		size_t hash_mesh = _RenderSystem.CreateMeshFromGeometry<Vertex_PTN>(hash_geometry);
		_RenderSystem.CreateColliders(hash_mesh, E_Collider::SPHERE);

		std::vector<size_t> hashs_material = _RenderSystem.CreateMaterialsFromGeometry(hash_geometry);
		std::vector<Mesh_Material> mesh_mats;
		for (const auto& iter : hashs_material)
			mesh_mats.push_back({ hash_mesh, iter });

		size_t hash_ra = _RenderSystem.CreateRenderAsset(g_szName_ra + szName, mesh_mats);
		
		//ECS Initialize(test, 251111)
		ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Transform, C_Render, T_Collider, T_Render_Geometry_Static>();

		size_t lookup = _ECSSystem.CreateEntity<C_Transform, C_Render, T_Collider, T_Render_Geometry_Static>();

		_ECSSystem.AddComponent<C_Transform>(key, { {3.0f, 3.0f, 3.0f}, Quarternion(0.0f, 45.0f, 0.0f), {100.0f, 0.0f, 0.0f} });

		_ECSSystem.AddComponent<C_Render>(key, { true, hash_ra });
		}
#endif // _NENE

#ifdef _GIRL
	//girl fbx
	{
		const std::wstring szName = L"Girl";
		size_t hash_geometry = _RenderSystem.CreateGeometry(L"../Assets/Meshes/girl.obj");

		size_t hash_mesh = _RenderSystem.CreateMeshFromGeometry<Vertex_PTNTB>(hash_geometry);
		_RenderSystem.CreateColliders(hash_mesh, E_Collider::SPHERE);

		std::vector<size_t> hashs_material = _RenderSystem.CreateMaterialsFromGeometry(hash_geometry);
		std::vector<Mesh_Material> mesh_mats;
		for (const auto& iter : hashs_material)
			mesh_mats.push_back({ hash_mesh, iter });
	
		size_t hash_ra = _RenderSystem.CreateRenderAsset(szName, mesh_mats);

		//ECS Initialize(test, 251111)
		ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Transform, C_Render, T_Collider, T_Render_Geometry_Static>();

		size_t lookup = _ECSSystem.CreateEntity<C_Transform, C_Render, T_Collider, T_Render_Geometry_Static>();

		_ECSSystem.AddComponent<C_Transform>(key, { {50.0f, 50.0f, 50.0f}, Quarternion(0.0f, 0.0f, 0.0f), {-100.0f, 0.0f, 0.0f} });

		_ECSSystem.AddComponent<C_Render>(key, { true, hash_ra });
	}
#endif // _GIRL

#ifdef _MutantWalk
	//_MutantWalk
	{
		const std::wstring szName = L"MutantWalk";
		size_t hash_geometry = _RenderSystem.CreateGeometry(L"../Assets/Meshes/Mutant Walking.fbx");

		size_t hash_mesh = _RenderSystem.CreateMeshFromGeometry<Vertex_PTNTB_Skinned>(hash_geometry);
		_RenderSystem.CreateColliders(hash_mesh, E_Collider::SPHERE);

		size_t hash_animation = _RenderSystem.CreateAnimaitonFromGeometry(hash_geometry);
		std::vector<size_t> hashs_material = _RenderSystem.CreateMaterialsFromGeometry(hash_geometry);
		std::vector<Mesh_Material> mesh_mats;
		for (auto& iter : hashs_material)
		{
			_RenderSystem.Material_SetShaders(iter, 3);
			mesh_mats.push_back({ hash_mesh, iter });
		}
			
		size_t hash_ra = _RenderSystem.CreateRenderAsset(g_szName_ra + szName, mesh_mats);
	
		//ECS Initialize(test, 251111)
		ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Transform, C_Render, T_Collider, C_Animation, T_Render_Geometry_Skeletal>();

		size_t lookup = _ECSSystem.CreateEntity<C_Transform, C_Render, T_Collider, C_Animation, T_Render_Geometry_Skeletal>();

		_ECSSystem.AddComponent<C_Transform>(key, { {0.3f, 0.3f, 0.3f}, Quarternion(0.0f, 90.0f, 0.0f), {-150.0f, 0.0f, 0.0f} });

		_ECSSystem.AddComponent<C_Render>(key, { true, hash_ra });

		_ECSSystem.AddComponent<C_Animation>(key, { hash_animation });
	}

#endif // _MutantWalk

#ifdef _Praying
	//_Praying
	{
		const std::wstring szName = L"Praying";
		size_t hash_geometry = _RenderSystem.CreateGeometry(L"../Assets/Meshes/Praying.fbx");

		size_t hash_mesh = _RenderSystem.CreateMeshFromGeometry<Vertex_PTNTB_Skinned>(hash_geometry);
		_RenderSystem.CreateColliders(hash_mesh, E_Collider::SPHERE);

		size_t hash_animation = _RenderSystem.CreateAnimaitonFromGeometry(hash_geometry);
		std::vector<size_t> hashs_material = _RenderSystem.CreateMaterialsFromGeometry(hash_geometry);
		std::vector<Mesh_Material> mesh_mats;
		for (auto& iter : hashs_material)
		{
			_RenderSystem.Material_SetShaders(iter, 3);
			mesh_mats.push_back({ hash_mesh, iter });
		}

		size_t hash_ra = _RenderSystem.CreateRenderAsset(g_szName_ra + szName, mesh_mats);

		//ECS Initialize(test, 251111)
		ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Transform, C_Render, T_Collider, C_Animation, T_Render_Geometry_Skeletal>();

		size_t lookup = _ECSSystem.CreateEntity<C_Transform, C_Render, T_Collider, C_Animation, T_Render_Geometry_Skeletal>();

		_ECSSystem.AddComponent<C_Transform>(key, { {0.3f, 0.3f, 0.3f}, Quarternion(0.0f, 90.0f, 0.0f), {-150.0f, 0.0f, -100.0f} });

		_ECSSystem.AddComponent<C_Render>(key, { true, hash_ra });

		_ECSSystem.AddComponent<C_Animation>(key, { hash_animation });
	}
#endif // _Praying

#ifdef _Herald
	//_Herald
	{
		const std::wstring szName = L"Herald";
		size_t hash_geometry = _RenderSystem.CreateGeometry(L"../Assets/Meshes/Herald.fbx");

		size_t hash_mesh = _RenderSystem.CreateMeshFromGeometry<Vertex_PTNTB_Skinned>(hash_geometry);
		_RenderSystem.CreateColliders(hash_mesh, E_Collider::SPHERE);

		size_t hash_animation = _RenderSystem.CreateAnimaitonFromGeometry(hash_geometry);
		std::vector<size_t> hashs_material = _RenderSystem.CreateMaterialsFromGeometry(hash_geometry);
		std::vector<Mesh_Material> mesh_mats;
		for (auto& iter : hashs_material)
		{
			_RenderSystem.Material_SetShaders(iter, 3);
			mesh_mats.push_back({ hash_mesh, iter });
		}

		size_t hash_ra = _RenderSystem.CreateRenderAsset(g_szName_ra + szName, mesh_mats);

		//ECS Initialize(test, 251111)
		ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Transform, C_Render, T_Collider, C_Animation, T_Render_Geometry_Skeletal>();

		size_t lookup = _ECSSystem.CreateEntity<C_Transform, C_Render, T_Collider, C_Animation, T_Render_Geometry_Skeletal>();

		_ECSSystem.AddComponent<C_Transform>(key, { {0.001f, 0.001f, 0.001f}, Quarternion(0.0f, 90.0f, 0.0f), {-150.0f, 0.0f, -40.0f} });

		_ECSSystem.AddComponent<C_Render>(key, { true, hash_ra });

		_ECSSystem.AddComponent<C_Animation>(key, { hash_animation });
	}
#endif // _Praying

#ifdef _Rapunzel
	//_Rapunzel
	{
		const std::wstring szName = L"Rapunzel";
		size_t hash_geometry = _RenderSystem.CreateGeometry(L"../Assets/Meshes/Rapunzel.fbx");

		size_t hash_mesh = _RenderSystem.CreateMeshFromGeometry<Vertex_PTN_Skinned>(hash_geometry);
		_RenderSystem.CreateColliders(hash_mesh, E_Collider::SPHERE);

		size_t hash_animation = _RenderSystem.CreateAnimaitonFromGeometry(hash_geometry);
		std::vector<size_t> hashs_material = _RenderSystem.CreateMaterialsFromGeometry(hash_geometry);
		std::vector<Mesh_Material> mesh_mats;
		for (auto& iter : hashs_material)
		{
			_RenderSystem.Material_SetShaders(iter, 2);
			mesh_mats.push_back({ hash_mesh, iter });
		}

		size_t hash_ra = _RenderSystem.CreateRenderAsset(g_szName_ra + szName, mesh_mats);

		//ECS Initialize(test, 251111)
		ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Info, C_Transform, C_Render, T_Collider, C_Animation, T_Render_Geometry_Skeletal>();

		size_t lookup = _ECSSystem.CreateEntity<C_Info, C_Transform, C_Render, T_Collider, C_Animation, T_Render_Geometry_Skeletal>();

		_ECSSystem.AddComponent<C_Info>(key, { szName });

		_ECSSystem.AddComponent<C_Transform>(key, { {10.0f, 10.0f, 10.0f}, Quarternion(0.0f, 180.0f, 0.0f), {-100.0f, 0.0f, -40.0f} });

		_ECSSystem.AddComponent<C_Render>(key, { true, hash_ra });

		_ECSSystem.AddComponent<C_Animation>(key, { hash_animation });
	}
#endif // _Rapunzel
	
#ifdef _Doro
	//_Doro
	{
		const std::wstring szName = L"Doro";
		size_t hash_geometry = _RenderSystem.CreateGeometry(L"../Assets/Meshes/Doro.fbx");

		size_t hash_mesh = _RenderSystem.CreateMeshFromGeometry<Vertex_PTNTB>(hash_geometry);
		_RenderSystem.CreateColliders(hash_mesh, E_Collider::SPHERE);

		size_t hash_animation = _RenderSystem.CreateAnimaitonFromGeometry(hash_geometry);
		std::vector<size_t> hashs_material = _RenderSystem.CreateMaterialsFromGeometry(hash_geometry);
		std::vector<Mesh_Material> mesh_mats;
		for (auto& iter : hashs_material)
		{
			_RenderSystem.Material_SetShaders(iter, 1);
			mesh_mats.push_back({ hash_mesh, iter });
		}

		size_t hash_ra = _RenderSystem.CreateRenderAsset(g_szName_ra + szName, mesh_mats);

		//ECS Initialize(test, 251111)
		ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Info, C_Transform, C_Render, T_Collider, C_Animation, T_Render_Geometry_Static>();

		size_t lookup = _ECSSystem.CreateEntity<C_Info, C_Transform, C_Render, T_Collider, C_Animation, T_Render_Geometry_Static>();

		_ECSSystem.AddComponent<C_Info>(key, { szName });

		_ECSSystem.AddComponent<C_Transform>(key, { {10.0f, 10.0f, 10.0f}, Quarternion(90.0f, 0.0f, 0.0f), {-50.0f, 10.0f, -40.0f} });

		_ECSSystem.AddComponent<C_Render>(key, { true, hash_ra });

		_ECSSystem.AddComponent<C_Animation>(key, { hash_animation });
	}
#endif // _Doro

#ifdef _DRAGON
	//_DRAGON
	{
		size_t hash_geometry = _RenderSystem.CreateGeometry(L"../Assets/Meshes/dragon/Dragon 2.5_fbx.fbx");
		//size_t hash_mesh = _RenderSystem.CreateMeshFromGeometry<Vertex_PTNTB>(hash_geometry);
		//std::map<UINT, MTL_TEXTURES> textures;
		//size_t hash_mesh = _RenderSystem.CreateMesh<Vertex_PTNTB>(L"../Assets/Meshes/MechanicGirl/Mechanic_Girl-85698ecb/obj/OBJ/SK_MechanicGirl_AllPartsTogether.obj");
		//size_t hash_mesh = _RenderSystem.CreateMesh<Vertex_PTNTB>(L"../Assets/Meshes/dragon/Dragon 2.5_fbx.fbx", textures);

		//std::vector<size_t> hash_materials(3, _RenderSystem.CreateMaterial<Vertex_PTN>(L"Mat_MW"));
		//for (const auto& hash_material : hash_materials)
		//{
		//	_RenderSystem.Material_SetVS(hash_material, L"VS_PTN.hlsl");
		//	_RenderSystem.Material_SetPS(hash_material, L"PS_PTN.hlsl");
		//	_RenderSystem.Material_SetIL<Vertex_PTNTB>(hash_material, L"VS_PTN.hlsl");
		//}

		//std::vector<TX_HASH> tx_hashs[3];
		//tx_hashs[0].push_back({ E_Texture::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/butter.dds") });
		//_RenderSystem.Material_SetTextures(hash_materials[0], tx_hashs[0]);
		//tx_hashs[1].push_back({ E_Texture::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/butter.dds") });
		//_RenderSystem.Material_SetTextures(hash_materials[1], tx_hashs[1]);
		//tx_hashs[2].push_back({ E_Texture::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/butter.dds") });
		//_RenderSystem.Material_SetTextures(hash_materials[2], tx_hashs[2]);

		//std::vector<Mesh_Material> mesh_mats;
		//mesh_mats.push_back({ hash_mesh, hash_materials[0] });
		//mesh_mats.push_back({ hash_mesh, hash_materials[1] });
		//mesh_mats.push_back({ hash_mesh, hash_materials[2] });
		//size_t hash_ra = _RenderSystem.CreateRenderAsset(L"ra_MW", mesh_mats);

		//const std::unordered_set<size_t>& hash_CLs = _RenderSystem.CreateColliders(hash_mesh, E_Collider::SPHERE);
		//size_t hash_ca = _RenderSystem.CreateColliderAsset(L"ca_MW", hash_CLs);

		////ECS Initialize(test, 251111)
		//ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Transform, C_Render, C_Collider, T_Render_Geometry>();

		//size_t lookup = _ECSSystem.CreateEntity<C_Transform, C_Render, C_Collider, T_Render_Geometry>();

		//_ECSSystem.AddComponent<C_Transform>(key, { {0.3f, 0.3f, 0.3f}, Quarternion(90.0f, 90.0f, 0.0f), {-100.0f, 0.0f, -50.0f} });

		//_ECSSystem.AddComponent<C_Render>(key, { true, hash_ra });

		//_ECSSystem.AddComponent<C_Collider>(key, { hash_ca });
	}

#endif // _DRAGON
	
	
#ifdef _EnviornmentMap
	//Initilze Cubemap
	{
		Vector3 pos(50.0f, 0.0f, 0.0f);
		float x = pos.GetX();
		float y = pos.GetY();
		float z = pos.GetZ();
		Vector3 targets[6] =
		{
			{ x + 1.0f, y, z }, // +X
			{ x - 1.0f, y, z }, // -X
			{ x, y + 1.0f, z }, // +Y
			{ x, y - 1.0f, z }, // -Y
			{ x, y, z + 1.0f }, // +Z
			{ x, y, z - 1.0f }  // -Z
		};

		Vector3 ups[6] =
		{
			{ 0.0f,  1.0f,  0.0f}, // +X: Up is Y
			{ 0.0f,  1.0f,  0.0f}, // -X: Up is Y
			{ 0.0f,  0.0f, -1.0f}, // +Y: Up is -Z (중요)
			{ 0.0f,  0.0f,  1.0f}, // -Y: Up is +Z (중요)
			{ 0.0f,  1.0f,  0.0f}, // +Z: Up is Y
			{ 0.0f,  1.0f,  0.0f}  // -Z: Up is Y
		};

		//큐브맵 텍스쳐
		UINT cubemapsize = 256;
		_RenderSystem.m_vp_CubeMap.Width = _RenderSystem.m_vp_CubeMap.Height = cubemapsize;
		_RenderSystem.CreateCubeMapTexture(cubemapsize);

		//pos = Vector3(50.0f, 50.0f, 0.0f);
		//큐브맵 카메라 세팅
		{
			for (int idx = 0; idx < 6; idx++)
			{
				auto campos = pos;
				auto matView = GetMat_View(campos, targets[idx], ups[idx]);
				auto matWorld = GetMat_Inverse(matView);
				ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Transform, C_Camera, C_Projection, T_Camera_Cubemap>();
				_CameraSystem.lookup_cubemapcam[idx] = _ECSSystem.CreateEntity<C_Transform, C_Camera, C_Projection, T_Camera_Cubemap>();

				_ECSSystem.AddComponent<C_Transform>(key, { {1.0f, 1.0f, 1.0f}, matWorld, campos });

				C_Camera camera;
				camera.fScreenWidth = cubemapsize;
				camera.fScreenHeight = cubemapsize;
				camera.fFov = 90.0f;	//큐브맵의 카메라는 90도여야한다
				camera.fNear = 0.1f;
				camera.fFar = 10000.0f;
				_ECSSystem.AddComponent<C_Camera>(key, std::move(camera));
			}
		}

		{
			const std::wstring szName = L"Environment_Sphere";
			size_t hash_geometry = _RenderSystem.CreateGeometry(L"../Assets/Meshes/sphere.obj");
			size_t hash_mesh = _RenderSystem.CreateMeshFromGeometry<Vertex_PTN>(hash_geometry);
			size_t hash_material = _RenderSystem.CreateMaterial(g_szName_mat + szName);
			_RenderSystem.Material_SetVS(hash_material, L"VS_CubeMap.hlsl");
			_RenderSystem.Material_SetPS(hash_material, L"PS_CubeMap.hlsl");
			_RenderSystem.Material_SetIL<Vertex_PTN>(hash_material, L"VS_CubeMap.hlsl");

			std::vector<TX_HASH> tx_hashs;
			tx_hashs.push_back({ E_Texture::Diffuse, _RenderSystem.CreateTexture(L"../Assets/Textures/metal.jpg") });
			_RenderSystem.Material_SetTextures(hash_material, tx_hashs);
			std::vector<Mesh_Material> mesh_mats;
			mesh_mats.push_back({ hash_mesh, hash_material });
			size_t hash_ra = _RenderSystem.CreateRenderAsset(g_szName_ra + szName, mesh_mats);

			const std::unordered_set<size_t>& hash_CLs = _RenderSystem.CreateColliders(hash_mesh, E_Collider::SPHERE);
			size_t hash_ca = _RenderSystem.CreateColliderAsset(L"ca_Environment", hash_CLs);

			ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Transform, C_Render, C_Collider, T_Render_CubeMap>();

			size_t lookup = _ECSSystem.CreateEntity<C_Transform, C_Render, C_Collider, T_Render_CubeMap>();
			_RenderSystem.m_hash_CubemapLookup = lookup;

			_ECSSystem.AddComponent<C_Transform>(key, { {30.0f, 30.0f, 30.0f}, {0.0f, 0.0f, 0.0f}, pos });

			_ECSSystem.AddComponent<C_Render>(key, { true, hash_ra });

			_ECSSystem.AddComponent<C_Collider>(key, { hash_ca });
		}
	}
#endif //_EnvironmentMap

	//ShadowMap
	{
		const std::wstring szName = L"ShadowMap";
		_RenderSystem.m_hash_DSV_ShadowMap = _RenderSystem.CreateShadowMapTexture(g_iWidth, g_iHeight);
		size_t hash_material = _RenderSystem.CreateMaterial(g_szName_mat + szName);
		_RenderSystem.Material_SetPS(hash_material, L"PS_ShadowMap.hlsl");
		_RenderSystem.m_hash_Mat_ShadowMap = hash_material;
	}

	//Initialize RTV, DSV
	{
		//Init TargetView geometry
		std::map<UINT, std::vector<Vertex_PTNTB_Skinned>> verticesByMaterial;
		std::map<UINT, std::vector<UINT>> indicesByMaterial;
		std::vector<std::vector<Vector3>> pointsByMeshs;
		GeometryGenerate_Plane<Vertex_PT>(pointsByMeshs, verticesByMaterial[0], indicesByMaterial[0]);
		size_t hash_mesh = _RenderSystem.CreateMeshFromGeometry<Vertex_PT>(L"Plane", verticesByMaterial, indicesByMaterial, pointsByMeshs);
		//RTV
		{
			const std::wstring szName = L"BP_RTV";
			size_t hash_material = _RenderSystem.CreateMaterial(g_szName_mat + szName);
			_RenderSystem.Material_SetVS(hash_material, L"VS_PT.hlsl");
			_RenderSystem.Material_SetPS(hash_material, L"PS_Distortion.hlsl");
			_RenderSystem.Material_SetIL<Vertex_PT>(hash_material, L"VS_PT.hlsl");

			std::vector<TX_HASH> tx_hashs;
			tx_hashs.push_back({ E_Texture::Diffuse, _RenderSystem.m_hash_RTV_0 });
			_RenderSystem.Material_SetTextures(hash_material, tx_hashs);
			std::vector<Mesh_Material> mesh_mats;
			mesh_mats.push_back({ hash_mesh, hash_material });
			size_t hash_ra = _RenderSystem.CreateRenderAsset(g_szName_ra + szName, mesh_mats);

			ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Transform, C_Render, T_Render_UI>();
			size_t lookup = _ECSSystem.CreateEntity<C_Transform, C_Render, T_Render_UI>();

			_ECSSystem.AddComponent<C_Transform>(key, { {(float)g_iWidth, (float)g_iHeight, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f} });
			_ECSSystem.AddComponent<C_Render>(key, { true, hash_ra });
		}
		//DSV
		{
			const std::wstring szName = L"BP_DSV";
			size_t hash_material = _RenderSystem.CreateMaterial(g_szName_mat + szName);
			_RenderSystem.Material_SetVS(hash_material, L"VS_PT.hlsl");
			_RenderSystem.Material_SetPS(hash_material, L"PS_DepthDebug.hlsl");
			_RenderSystem.Material_SetIL<Vertex_PT>(hash_material, L"VS_PT.hlsl");

			std::vector<TX_HASH> tx_hashs;
			//_RenderSystem.m_hash_DSV_ShadowMap
			//tx_hashs.push_back({ E_Texture::Diffuse, _RenderSystem.m_hash_DSV_ShadowMap });
			tx_hashs.push_back({ E_Texture::Diffuse, _RenderSystem.m_hash_DSV_0 });
			_RenderSystem.Material_SetTextures(hash_material, tx_hashs);
			std::vector<Mesh_Material> mesh_mats;
			mesh_mats.push_back({ hash_mesh, hash_material });
			size_t hash_ra = _RenderSystem.CreateRenderAsset(g_szName_ra + szName, mesh_mats);

			ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Transform, C_Render, T_Render_UI>();
			size_t lookup = _ECSSystem.CreateEntity<C_Transform, C_Render, T_Render_UI>();

			_ECSSystem.AddComponent<C_Transform>(key, { {300.0f, 300.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f} });
			_ECSSystem.AddComponent<C_Render>(key, { true, hash_ra });
		}
	}

#ifdef _LEGACY
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

		std::vector<size_t> hash_mats = _RenderSystem.CreateMaterialsFromFile<Vertex_PTN>(L"../Assets/Meshes/spaceship.mtl", VSs, PSs);
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

		std::vector<size_t> hash_mats = _RenderSystem.CreateMaterialsFromFile<Vertex_PTN>(L"../Assets/Meshes/asteroid.mtl", VSs, PSs);
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
