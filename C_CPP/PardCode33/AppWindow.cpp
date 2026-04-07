#include "AppWindow.h"
#include "EngineSystem.h"
#include "RenderSystem.h"
#include "InputSystem.h"
#include "CameraSystem.h"
#include "ResourceSystem.h"
#include "ImguiSystem.h"
#include "TimerSystem.h"
#include "CollisionSystem.h"
#include "GeometryGenerator.h"
#include "ECSSystem.h"
#include "BehaviorSystem.h"
#include "AnimationSystem.h"
#include "ComputeSystem.h"

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
	_EngineSystem;
	_RenderSystem;
	_ImguiSystem;
	_AnimationSystem;
	_BehaviorSystem;
	_ComputeSystem;
}
const std::wstring g_szName_View = L"View_";
const std::wstring g_szName_mat = L"Mat_";
const std::wstring g_szName_ra = L"Ra_";
const std::wstring g_szName_ca = L"Ca_";
UINT g_iWidth = 800.0f;
UINT g_iHeight = 600.0f;
HWND g_hWnd;
bool g_bIsRun = false;
float g_fTime_Log = 0.0f;
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
	_EngineSystem.Init();
	_RenderSystem.Init();
	_ImguiSystem.Init(g_hWnd, _EngineSystem.GetD3DDevice(), _EngineSystem.GetD3DDeviceContext());
	_CameraSystem.Init();
	_ECSSystem.Init();
	_BehaviorSystem.Init();
	_AnimationSystem.Init();
	_ComputeSystem.Init();

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

	_InputSystem.SetMouseCenter(g_hWnd);
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
			_ECSSystem.AddComponent<C_Transform>(key, { {1.0f, 1.0f, 1.0f}, Quaternion(dir), pos });

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
			_EngineSystem.m_hash_light_directional = _ECSSystem.CreateEntity<C_Light, C_Light_Direction, T_Light_Directional>();

			_ECSSystem.AddComponent<C_Light>(key, { Vector4(0.3f, 0.3f, 0.3f, 1.0f) , Vector4(0.7f, 0.7f, 0.7f, 1.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), 100.0f });
			_ECSSystem.AddComponent<C_Light_Direction>(key, { Vector3(0.0f, -1.0f, 1.0f) });
		}

		//Point Light
		{
			ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Transform, C_Light, C_Light_Attenuation, T_Light_Point>();
			_EngineSystem.m_hash_light_point = _ECSSystem.CreateEntity<C_Transform, C_Light, C_Light_Attenuation, T_Light_Point>();

			_ECSSystem.AddComponent<C_Transform>(key, { {}, {}, Vector3(10.0f, 10.0f, 0.0f) });
			_ECSSystem.AddComponent<C_Light>(key, { Vector4(0.2f, 0.5f, 0.2f, 1.0f) , Vector4(0.7f, 0.7f, 0.7f, 1.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), 100.0f });
			_ECSSystem.AddComponent<C_Light_Attenuation>(key, { 0.49f, 0.5f, 0.01f, 500.0f });
		}
		
		//Spot Light
		{
			ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Transform, C_Light, C_Light_Attenuation, C_Light_Spot, T_Light_Spot>();
			_EngineSystem.m_hash_light_spot = _ECSSystem.CreateEntity<C_Transform, C_Light, C_Light_Attenuation, C_Light_Spot, T_Light_Spot>();

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
			std::vector<Vertex_PC> iTriangleCount;
			std::vector<UINT> indices;
			GeometryGenerate_Gizmo(points, iTriangleCount, indices);
			size_t hash_mesh = _EngineSystem.CreateMeshFromGeometry<Vertex_PC>(L"Gizmo", std::move(points), std::move(iTriangleCount), std::move(indices));
			{

			}*/
		}


		//SkySphere
		{
			const std::wstring szName = L"SkySphere";
			size_t hash_geometry = _EngineSystem.CreateGeometry(L"../Assets/Meshes/sphere.obj");
			size_t hash_mesh = _EngineSystem.CreateMeshFromGeometry<Vertex_PTN>(hash_geometry);
			size_t hash_material = _EngineSystem.CreateMaterial(g_szName_mat + szName);
			_EngineSystem.Material_SetVS(hash_material, L"VS_PTN.hlsl");
			_EngineSystem.Material_SetPS(hash_material, L"PS_.hlsl");
			_EngineSystem.Material_SetIL<Vertex_PTN>(hash_material, L"VS_PTN.hlsl");

			std::vector<TX_HASH> tx_hashs;
			tx_hashs.push_back({ E_Texture::Diffuse, _EngineSystem.CreateTexture(L"../Assets/Textures/butter8.png") });
			_EngineSystem.Material_SetTextures(hash_material, tx_hashs);
			std::vector<Mesh_Material> mesh_mats;
			mesh_mats.push_back({ hash_mesh, hash_material });
			size_t hash_asset_Render = _EngineSystem.CreateRenderAsset(g_szName_ra + szName, mesh_mats);

			//ECS Initialize(test, 251111)
			ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Info, C_Transform, C_Input, C_Behavior, C_Render, T_Render_Sky>();
			size_t lookup = _ECSSystem.CreateEntity<C_Info, C_Transform, C_Input, C_Behavior, C_Render, T_Render_Sky>();

			_ECSSystem.AddComponent<C_Info>(key, { szName, lookup });

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

			_ECSSystem.AddComponent<C_Render>(key, { true, hash_asset_Render });
		}

#ifdef _Floor
		{
			const std::wstring szName = L"floor";
			std::map<UINT, std::vector<Vertex_PTNTB_Skinned>> verticesByMaterial;
			std::map<UINT, std::vector<UINT>> indicesByMaterial;
			std::vector<std::vector<Vector3>> pointsByMeshs;
			GeometryGenerate_Plane(pointsByMeshs, verticesByMaterial[0], indicesByMaterial[0]);

			size_t hash_mesh = _EngineSystem.CreateMeshFromGeometry<Vertex_PTN>(szName, verticesByMaterial, indicesByMaterial, pointsByMeshs);
			_EngineSystem.CreateColliders(hash_mesh, E_Collider::AABB);

			size_t hash_material = _EngineSystem.CreateMaterial(g_szName_mat + szName);
			_EngineSystem.Material_SetVS(hash_material, L"VS_PTN.hlsl");
			_EngineSystem.Material_SetPS(hash_material, L"PS_PTN.hlsl");
			_EngineSystem.Material_SetIL<Vertex_PTN>(hash_material, L"VS_PTN.hlsl");

			std::vector<TX_HASH> tx_hashs;
			tx_hashs.push_back({ E_Texture::Diffuse, _EngineSystem.CreateTexture(L"../Assets/Textures/butter8.png") });
			_EngineSystem.Material_SetTextures(hash_material, tx_hashs);
			std::vector<Mesh_Material> mesh_mats;
			mesh_mats.push_back({ hash_mesh, hash_material });
			size_t hash_asset_Render = _EngineSystem.CreateRenderAsset(g_szName_ra + szName, mesh_mats);

			ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Info, C_Transform, C_Render, C_Collider, T_Render_Geometry_Static>();
			size_t lookup = _ECSSystem.CreateEntity<C_Info, C_Transform, C_Render, C_Collider, T_Render_Geometry_Static>();

			_ECSSystem.AddComponent<C_Info>(key, { szName, lookup });

			_ECSSystem.AddComponent<C_Transform>(key, { {500.0f, 500.0f, 1.0f}, {Quaternion(90.0f, 0.0f, 0.0f)}, {0.0f, 0.0f, 0.0f} });

			_ECSSystem.AddComponent<C_Render>(key, { true, hash_asset_Render });

			_ECSSystem.AddComponent<C_Collider>(key, { E_Collider::AABB });
		}
#endif // _Floor

#ifdef _GeoSphere
		{
			const std::wstring szName = L"GeoSphere";
			std::map<UINT, std::vector<Vertex_PTNTB_Skinned>> verticesByMaterial;
			std::map<UINT, std::vector<UINT>> indicesByMaterial;
			std::vector<std::vector<Vector3>> pointsByMeshs;
			GeometryGenerate_GeoSphere(1.0f, 2, pointsByMeshs, verticesByMaterial[0], indicesByMaterial[0]);

			size_t hash_mesh = _EngineSystem.CreateMeshFromGeometry<Vertex_PTN>(szName, verticesByMaterial, indicesByMaterial, pointsByMeshs);
			_EngineSystem.CreateColliders(hash_mesh, E_Collider::SPHERE);

			size_t hash_material = _EngineSystem.CreateMaterial(g_szName_mat + szName);
			_EngineSystem.Material_SetVS(hash_material, L"VS_PTN.hlsl");
			_EngineSystem.Material_SetPS(hash_material, L"PS_PTN.hlsl");
			_EngineSystem.Material_SetIL<Vertex_PTN>(hash_material, L"VS_PTN.hlsl");

			std::vector<TX_HASH> tx_hashs;
			tx_hashs.push_back({ E_Texture::Diffuse, _EngineSystem.CreateTexture(L"../Assets/Textures/butter8.png") });
			_EngineSystem.Material_SetTextures(hash_material, tx_hashs);
			std::vector<Mesh_Material> mesh_mats;
			mesh_mats.push_back({ hash_mesh, hash_material });
			size_t hash_asset_Render = _EngineSystem.CreateRenderAsset(g_szName_ra + szName, mesh_mats);

			ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Info, C_Transform, C_Render, C_Collider, T_Render_Geometry_Static>();
			size_t lookup = _ECSSystem.CreateEntity<C_Info, C_Transform, C_Render, C_Collider, T_Render_Geometry_Static>();

			_ECSSystem.AddComponent<C_Info>(key, { szName, lookup });

			_ECSSystem.AddComponent<C_Transform>(key, { {5.0f, 5.0f, 5.0f}, {Quaternion(0.0f, 0.0f, 0.0f)}, {0.0f, 50.0f, 0.0f} });

			_ECSSystem.AddComponent<C_Render>(key, { true, hash_asset_Render });

			_ECSSystem.AddComponent<C_Collider>(key, { E_Collider::SPHERE });
		}
#endif // _GeoSphere
		
		
#ifdef _RandOBJ
		const size_t cnt = 500;

		//rand obj
		{
			const std::wstring szName = L"RandCube";
			size_t hash_geometry = _EngineSystem.CreateGeometry(L"../Assets/Meshes/cube.obj");

			size_t hash_mesh = _EngineSystem.CreateMeshFromGeometry<Vertex_PTN>(hash_geometry);
			_EngineSystem.CreateColliders(hash_mesh, E_Collider::AABB);

			size_t hash_material = _EngineSystem.CreateMaterial(g_szName_mat + szName);
			_EngineSystem.Material_SetVS(hash_material, L"VS_PTN.hlsl");
			_EngineSystem.Material_SetIL<Vertex_PTN>(hash_material, L"VS_PTN.hlsl");
			_EngineSystem.Material_SetGS(hash_material, L"GS_PTN.hlsl");
			_EngineSystem.Material_SetPS(hash_material, L"PS_PTN.hlsl");

			std::vector<TX_HASH> tx_hashs;
			tx_hashs.push_back({ E_Texture::Diffuse, _EngineSystem.CreateTexture(L"../Assets/Textures/butter6.webp") });
			_EngineSystem.Material_SetTextures(hash_material, tx_hashs);
			std::vector<Mesh_Material> mesh_mats;
			mesh_mats.push_back({ hash_mesh, hash_material });
			size_t hash_asset_Render = _EngineSystem.CreateRenderAsset(g_szName_ra + szName, mesh_mats);

			
			ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Info, C_Transform, C_Input, C_Behavior, C_Render, C_Collider, T_Render_Geometry_Static>();

			{
				for (int i = 0; i < cnt; i++)
				{
					size_t lookup = _ECSSystem.CreateEntity<C_Info, C_Transform, C_Input, C_Behavior, C_Render, C_Collider, T_Render_Geometry_Static>();
					std::bitset<256> vkmask;
					vkmask[VK_UP] = true;
					//vkmask[VK_LEFT] = true;
					vkmask[VK_DOWN] = true;
					//vkmask[VK_RIGHT] = true;
					_ECSSystem.AddComponent<C_Info>(key, { szName, lookup });

					_ECSSystem.AddComponent<C_Input>(key, { vkmask });

					_ECSSystem.AddComponent<C_Transform>(key, { {5.0f, 5.0f, 5.0f},  Quaternion(dis(gen) * 10.0f, dis(gen) * 10.0f, dis(gen) * 10.0f), {(float)lookup, (float)lookup, (float)lookup} });

					std::array<unsigned char, E_Behavior::COUNT> behavior;
					behavior[E_Behavior::MOVE_FORWARD] = VK_UP;
					behavior[E_Behavior::MOVE_LEFT] = VK_LEFT;
					behavior[E_Behavior::MOVE_BACKWARD] = VK_DOWN;
					behavior[E_Behavior::MOVE_RIGHT] = VK_RIGHT;
					_ECSSystem.AddComponent<C_Behavior>(key, { behavior });

					_ECSSystem.AddComponent<C_Render>(key, { true, hash_asset_Render });

					_ECSSystem.AddComponent<C_Collider>(key, { E_Collider::AABB });
				}
			}
		}

		{
			const std::wstring szName = L"RandSphere";
			size_t hash_geometry = _EngineSystem.CreateGeometry(L"../Assets/Meshes/sphere.obj");

			size_t hash_mesh = _EngineSystem.CreateMeshFromGeometry<Vertex_PTN>(hash_geometry);
			_EngineSystem.CreateColliders(hash_mesh, E_Collider::SPHERE);

			size_t hash_material = _EngineSystem.CreateMaterial(g_szName_mat + szName);
			_EngineSystem.Material_SetVS(hash_material, L"VS_PTN.hlsl");
			_EngineSystem.Material_SetPS(hash_material, L"PS_PTN.hlsl");
			_EngineSystem.Material_SetIL<Vertex_PTN>(hash_material, L"VS_PTN.hlsl");

			std::vector<TX_HASH> tx_hashs;
			tx_hashs.push_back({ E_Texture::Diffuse, _EngineSystem.CreateTexture(L"../Assets/Textures/butter5.webp") });
			_EngineSystem.Material_SetTextures(hash_material, tx_hashs);
			std::vector<Mesh_Material> mesh_mats;
			mesh_mats.push_back({ hash_mesh, hash_material });
			size_t hash_asset_Render = _EngineSystem.CreateRenderAsset(g_szName_ra + szName, mesh_mats);
			

			ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Info, C_Transform, C_Input, C_Behavior, C_Render, C_Collider, T_Render_Geometry_Static>();

			{
				for (int i = 0; i < cnt; i++)
				{
					size_t lookup = _ECSSystem.CreateEntity<C_Info, C_Transform, C_Input, C_Behavior, C_Render, C_Collider, T_Render_Geometry_Static>();
					std::bitset<256> vkmask;
					//vkmask[VK_UP] = true;
					vkmask[VK_LEFT] = true;
					//vkmask[VK_DOWN] = true;
					vkmask[VK_RIGHT] = true;

					_ECSSystem.AddComponent<C_Info>(key, { szName, lookup });

					_ECSSystem.AddComponent<C_Input>(key, { vkmask });

					_ECSSystem.AddComponent<C_Transform>(key, { {5.0f, 5.0f, 5.0f}, Quaternion(dis(gen) * 10.0f, dis(gen) * 10.0f, dis(gen) * 10.0f), {(float)lookup,(float)lookup,(float)lookup} });

					std::array<unsigned char, E_Behavior::COUNT> behavior;
					behavior[E_Behavior::MOVE_FORWARD] = VK_UP;
					behavior[E_Behavior::MOVE_LEFT] = VK_LEFT;
					behavior[E_Behavior::MOVE_BACKWARD] = VK_DOWN;
					behavior[E_Behavior::MOVE_RIGHT] = VK_RIGHT;
					_ECSSystem.AddComponent<C_Behavior>(key, { behavior });

					_ECSSystem.AddComponent<C_Render>(key, { true, hash_asset_Render });

					_ECSSystem.AddComponent<C_Collider>(key, { E_Collider::SPHERE });
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
			GeometryGenerate_Plane(pointsByMeshs, verticesByMaterial[0], indicesByMaterial[0]);

			size_t hash_mesh = _EngineSystem.CreateMeshFromGeometry<Vertex_PTN>(szName, verticesByMaterial, indicesByMaterial, pointsByMeshs);
			_EngineSystem.CreateColliders(hash_mesh, E_Collider::AABB);

			size_t hash_material = _EngineSystem.CreateMaterial(g_szName_mat + szName);
			_EngineSystem.Material_SetVS(hash_material, L"VS_PTN.hlsl");
			_EngineSystem.Material_SetPS(hash_material, L"PS_PTN.hlsl");
			_EngineSystem.Material_SetIL<Vertex_PTN>(hash_material, L"VS_PTN.hlsl");

			std::vector<TX_HASH> tx_hashs;
			tx_hashs.push_back({ E_Texture::Diffuse, _EngineSystem.CreateTexture(L"../Assets/Textures/butter8.png") });
			_EngineSystem.Material_SetTextures(hash_material, tx_hashs);
			std::vector<Mesh_Material> mesh_mats;
			mesh_mats.push_back({ hash_mesh, hash_material });
			size_t hash_asset_Render = _EngineSystem.CreateRenderAsset(g_szName_ra + szName, mesh_mats);
			

			ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Info, C_Transform, C_Render, C_Collider, T_Render_Billboard>();
			size_t lookup = _ECSSystem.CreateEntity<C_Info, C_Transform, C_Render, C_Collider, T_Render_Billboard>();

			_ECSSystem.AddComponent<C_Info>(key, { szName, lookup });

			_ECSSystem.AddComponent<C_Transform>(key, { {50.0f, 50.0f, 1.0f}, {}, {150.0f, 50.0f, -30.0f} });

			_ECSSystem.AddComponent<C_Render>(key, { true, hash_asset_Render });

			_ECSSystem.AddComponent<C_Collider>(key, { E_Collider::AABB });
		}
#endif // _Billboard

#ifdef _NORMALMAP
		{
			const std::wstring szName = L"NormalMap";
			size_t hash_geometry = _EngineSystem.CreateGeometry(L"../Assets/Meshes/sphere.obj");

			size_t hash_mesh = _EngineSystem.CreateMeshFromGeometry<Vertex_PTNTB>(hash_geometry);
			_EngineSystem.CreateColliders(hash_mesh, E_Collider::SPHERE);

			size_t hash_material = _EngineSystem.CreateMaterial(g_szName_mat + szName);
			_EngineSystem.Material_SetVS(hash_material, L"VS_PTNTB.hlsl");
			_EngineSystem.Material_SetPS(hash_material, L"PS_PTNTB.hlsl");
			_EngineSystem.Material_SetIL<Vertex_PTNTB>(hash_material, L"VS_PTNTB.hlsl");

			std::vector<TX_HASH> tx_hashs;
			tx_hashs.push_back({ E_Texture::Diffuse, _EngineSystem.CreateTexture(L"../Assets/Textures/brick_d.jpg") });
			tx_hashs.push_back({ E_Texture::Normal, _EngineSystem.CreateTexture(L"../Assets/Textures/brick_n.jpg") });
			_EngineSystem.Material_SetTextures(hash_material, tx_hashs);
			std::vector<Mesh_Material> mesh_mats;
			mesh_mats.push_back({ hash_mesh, hash_material });
			size_t hash_asset_Render = _EngineSystem.CreateRenderAsset(g_szName_ra + szName, mesh_mats);
			
			//ECS Initialize(test, 251111)
			ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Info, C_Transform, C_Render, C_Collider, T_Render_Geometry_Static >();
			size_t lookup = _ECSSystem.CreateEntity<C_Info, C_Transform, C_Render, C_Collider, T_Render_Geometry_Static>();

			_ECSSystem.AddComponent<C_Info>(key, { szName, lookup });

			_ECSSystem.AddComponent<C_Transform>(key, { {50.0f, 50.0f, 50.0f}, {}, {230.0f, 100.0f, 0.0f} });

			_ECSSystem.AddComponent<C_Render>(key, { true, hash_asset_Render });

			_ECSSystem.AddComponent<C_Collider>(key, { E_Collider::SPHERE });
		}
#endif // _NORMALMAP

#ifdef _HOUSE
		{
			const std::wstring szName = L"House";
			size_t hash_geometry = _EngineSystem.CreateGeometry(L"../Assets/Meshes/house.obj");

			size_t hash_mesh = _EngineSystem.CreateMeshFromGeometry<Vertex_PTNTB>(hash_geometry);
			_EngineSystem.CreateColliders(hash_mesh, E_Collider::AABB);

			size_t hash_material = _EngineSystem.CreateMaterial(g_szName_mat + szName);

			std::vector<size_t> hashs_material = _EngineSystem.CreateMaterialsFromGeometry(hash_geometry);
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
				tx_hash.push_back({ E_Texture::Diffuse, _EngineSystem.CreateTexture(paths[i]) });
				_EngineSystem.Material_SetTextures(hashs_material[i], tx_hash);
			}

			size_t hash_asset_Render = _EngineSystem.CreateRenderAsset(g_szName_ra + szName, mesh_mats);

			//ECS Initialize(test, 251111)
			ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Info, C_Transform, C_Render, C_Collider, T_Render_Geometry_Static>();

			size_t lookup = _ECSSystem.CreateEntity<C_Info, C_Transform, C_Render, C_Collider, T_Render_Geometry_Static>();

			_ECSSystem.AddComponent<C_Info>(key, { szName, lookup });

			_ECSSystem.AddComponent<C_Transform>(key, { {15.0f, 15.0f, 15.0f}, Quaternion(0.0f, 0.0f, 0.0f), {-150.0f, 0.0f, 50.0f} });

			_ECSSystem.AddComponent<C_Render>(key, { true, hash_asset_Render });

			_ECSSystem.AddComponent<C_Collider>(key, { E_Collider::AABB });
		}
#endif // _HOUSE

#ifdef _SPONZA
		{
			const std::wstring szName = L"Sponza";
			size_t hash_geometry = _EngineSystem.CreateGeometry(L"../Assets/Meshes/sponza_basic.obj");

			size_t hash_mesh = _EngineSystem.CreateMeshFromGeometry<Vertex_PTN>(hash_geometry);
			_EngineSystem.CreateColliders(hash_mesh, E_Collider::AABB);

			std::vector<size_t> hashs_material = _EngineSystem.CreateMaterialsFromGeometry(hash_geometry);
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
				tx_hash.push_back({ E_Texture::Diffuse, _EngineSystem.CreateTexture(paths[i])});
				_EngineSystem.Material_SetTextures(hashs_material[i], tx_hash);
			}

			size_t hash_asset_Render = _EngineSystem.CreateRenderAsset(g_szName_ra + szName, mesh_mats);
			
			//ECS Initialize(test, 251111)
			ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Info, C_Transform, C_Render, C_Collider, T_Render_Geometry_Static>();

			size_t lookup = _ECSSystem.CreateEntity<C_Info, C_Transform, C_Render, C_Collider, T_Render_Geometry_Static>();

			_ECSSystem.AddComponent<C_Info>(key, { szName, lookup });

			_ECSSystem.AddComponent<C_Transform>(key, { {5.0f, 5.0f, 5.0f}, Quaternion(0.0f, 45.0f, 0.0f), {100.0f, 0.0f, -150.0f} });

			_ECSSystem.AddComponent<C_Render>(key, { true, hash_asset_Render });

			_ECSSystem.AddComponent<C_Collider>(key, { E_Collider::AABB });
		}
#endif // _SPONZA

#ifdef _MECHAGIRL
		//MechaGirl
		{
			const std::wstring szName = L"MechaGirl";
			size_t hash_geometry = _EngineSystem.CreateGeometry(L"../Assets/Meshes/MechanicGirl/Mechanic_Girl-85698ecb/fbx/FBX/SK_MechanicGirl_AllPartsTogether.fbx");

			size_t hash_mesh = _EngineSystem.CreateMeshFromGeometry<Vertex_PTNTB>(hash_geometry);
			_EngineSystem.CreateColliders(hash_mesh, E_Collider::AABB);

			std::vector<size_t> hashs_material = _EngineSystem.CreateMaterialsFromGeometry(hash_geometry);
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
				tx_hash.push_back({ E_Texture::Diffuse, _EngineSystem.CreateTexture(paths[i]) });
				_EngineSystem.Material_SetTextures(hashs_material[i], tx_hash);
			}
			
			size_t hash_asset_Render = _EngineSystem.CreateRenderAsset(g_szName_ra + szName, mesh_mats);
			
			//ECS Initialize(test, 251111)
			ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Info, C_Transform, C_Render, C_Collider, T_Render_Geometry_Static>();

			size_t lookup = _ECSSystem.CreateEntity<C_Info, C_Transform, C_Render, C_Collider, T_Render_Geometry_Static>();

			_ECSSystem.AddComponent<C_Info>(key, { szName, lookup });

			_ECSSystem.AddComponent<C_Transform>(key, { {0.5f, 0.5f, 0.5f}, Quaternion(0.0f, 90.0f, 0.0f), {0.0f, 0.0f, -50.0f} });

			_ECSSystem.AddComponent<C_Render>(key, { true, hash_asset_Render });
		}
#endif // _MECHAGIRL
	}

#ifdef _NENE
	{
		const std::wstring szName = L"Nene";
		size_t hash_geometry = _EngineSystem.CreateGeometry(L"../Assets/Meshes/nene.obj");

		size_t hash_mesh = _EngineSystem.CreateMeshFromGeometry<Vertex_PTN>(hash_geometry);
		_EngineSystem.CreateColliders(hash_mesh, E_Collider::SPHERE);

		std::vector<size_t> hashs_material = _EngineSystem.CreateMaterialsFromGeometry(hash_geometry);
		std::vector<Mesh_Material> mesh_mats;
		for (const auto& iter : hashs_material)
			mesh_mats.push_back({ hash_mesh, iter });

		size_t hash_asset_Render = _EngineSystem.CreateRenderAsset(g_szName_ra + szName, mesh_mats);

		size_t hash_STB_vertices = _EngineSystem.CreateComputeVertices(hash_mesh);
		size_t hash_material_Compute = _EngineSystem.CreateMaterial(g_szName_mat + L"Compute_" + szName);
		_EngineSystem.Material_SetCS(hash_material_Compute, L"CS_Collision_Triangle.hlsl");
		std::vector<TX_HASH> tx_hashs_compute;
		tx_hashs_compute.push_back({ E_Texture::Compute_SRV, hash_STB_vertices });
		tx_hashs_compute.push_back({ E_Texture::Compute_UAV, g_hash_stb_collisionResults });
		_EngineSystem.Material_SetTextures(hash_material_Compute, tx_hashs_compute);
		size_t hash_asset_Compute = _EngineSystem.CreateComputeAsset(g_szName_ca + szName, { hash_material_Compute });

		//ECS Initialize(test, 251111)
		ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Info, C_Transform, C_Render, C_Collider, C_Compute, T_Render_Geometry_Static>();

		size_t lookup = _ECSSystem.CreateEntity<C_Info, C_Transform, C_Render, C_Collider, C_Compute, T_Render_Geometry_Static>();

		_ECSSystem.AddComponent<C_Info>(key, { szName, lookup });

		_ECSSystem.AddComponent<C_Transform>(key, { {3.0f, 3.0f, 3.0f}, Quaternion(0.0f, 45.0f, 0.0f), {100.0f, 0.0f, 0.0f} });

		_ECSSystem.AddComponent<C_Render>(key, { true, hash_asset_Render });

		_ECSSystem.AddComponent<C_Collider>(key, { E_Collider::SPHERE });

		_ECSSystem.AddComponent<C_Compute>(key, { hash_asset_Compute });
		}
#endif // _NENE

#ifdef _GIRL
	//girl fbx
	{
		const std::wstring szName = L"Girl";
		size_t hash_geometry = _EngineSystem.CreateGeometry(L"../Assets/Meshes/girl.obj");

		size_t hash_mesh = _EngineSystem.CreateMeshFromGeometry<Vertex_PTNTB>(hash_geometry);
		_EngineSystem.CreateColliders(hash_mesh, E_Collider::AABB);

		std::vector<size_t> hashs_material = _EngineSystem.CreateMaterialsFromGeometry(hash_geometry);
		std::vector<Mesh_Material> mesh_mats;
		for (const auto& iter : hashs_material)
			mesh_mats.push_back({ hash_mesh, iter });
	
		size_t hash_asset_Render = _EngineSystem.CreateRenderAsset(szName, mesh_mats);

		//ECS Initialize(test, 251111)
		ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Transform, C_Render, C_Collider, T_Render_Geometry_Static>();

		size_t lookup = _ECSSystem.CreateEntity<C_Transform, C_Render, C_Collider, T_Render_Geometry_Static>();

		_ECSSystem.AddComponent<C_Transform>(key, { {50.0f, 50.0f, 50.0f}, Quaternion(0.0f, 0.0f, 0.0f), {-100.0f, 0.0f, 0.0f} });

		_ECSSystem.AddComponent<C_Render>(key, { true, hash_asset_Render });
	}
#endif // _GIRL

#ifdef _MutantWalk
	//_MutantWalk
	{
		const std::wstring szName = L"MutantWalk";
		size_t hash_geometry = _EngineSystem.CreateGeometry(L"../Assets/Meshes/Mutant Walking.fbx");

		size_t hash_mesh = _EngineSystem.CreateMeshFromGeometry<Vertex_PTNTB_Skinned>(hash_geometry);
		_EngineSystem.CreateColliders(hash_mesh, E_Collider::AABB);
		
		size_t hash_animation = _EngineSystem.CreateAnimaitonFromGeometry(hash_geometry);
		_AnimationSystem.AddAnimbones(hash_animation);
		std::vector<size_t> hashs_material = _EngineSystem.CreateMaterialsFromGeometry(hash_geometry);
		std::vector<Mesh_Material> mesh_mats;
		for (auto& iter : hashs_material)
		{
			_EngineSystem.Material_SetShaders(iter, 3);
			mesh_mats.push_back({ hash_mesh, iter });
		}
		size_t hash_asset_Render = _EngineSystem.CreateRenderAsset(g_szName_ra + szName, mesh_mats);

		size_t hash_STB_vertices = _EngineSystem.CreateComputeVertices(hash_mesh);
		size_t hash_material_Compute = _EngineSystem.CreateMaterial(g_szName_mat + L"Compute_" + szName);
		_EngineSystem.Material_SetCS(hash_material_Compute, L"CS_Collision_Triangle.hlsl");
		std::vector<TX_HASH> tx_hashs_compute;
		tx_hashs_compute.push_back({ E_Texture::Compute_SRV, hash_STB_vertices });
		tx_hashs_compute.push_back({ E_Texture::Compute_UAV, g_hash_stb_collisionResults });
		_EngineSystem.Material_SetTextures(hash_material_Compute, tx_hashs_compute);
		size_t hash_asset_Compute = _EngineSystem.CreateComputeAsset(g_szName_ca + szName, { hash_material_Compute });
	
		//ECS Initialize(test, 251111)
		ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Info, C_Transform, C_Render, C_Collider, C_Compute, C_Animation, T_Render_Geometry_Skeletal>();

		size_t lookup = _ECSSystem.CreateEntity<C_Info, C_Transform, C_Render, C_Collider, C_Compute, C_Animation, T_Render_Geometry_Skeletal>();

		_ECSSystem.AddComponent<C_Info>(key, {szName, lookup});

		_ECSSystem.AddComponent<C_Transform>(key, { {0.3f, 0.3f, 0.3f}, Quaternion(0.0f, 90.0f, 0.0f), {-150.0f, 0.0f, 0.0f} });

		_ECSSystem.AddComponent<C_Render>(key, { true, hash_asset_Render });

		_ECSSystem.AddComponent<C_Compute>(key, { hash_asset_Compute });

		_ECSSystem.AddComponent<C_Collider>(key, { E_Collider::AABB });

		_AnimationSystem.AddAnimbones(lookup);
		_ECSSystem.AddComponent<C_Animation>(key, { hash_animation, lookup });
	}

#endif // _MutantWalk

#ifdef _Praying
	//_Praying
	{
		const std::wstring szName = L"Praying";
		size_t hash_geometry = _EngineSystem.CreateGeometry(L"../Assets/Meshes/Praying.fbx");

		size_t hash_mesh = _EngineSystem.CreateMeshFromGeometry<Vertex_PTNTB_Skinned>(hash_geometry);
		_EngineSystem.CreateColliders(hash_mesh, E_Collider::SPHERE);

		size_t hash_animation = _EngineSystem.CreateAnimaitonFromGeometry(hash_geometry);
		std::vector<size_t> hashs_material = _EngineSystem.CreateMaterialsFromGeometry(hash_geometry);
		std::vector<Mesh_Material> mesh_mats;
		for (auto& iter : hashs_material)
		{
			_EngineSystem.Material_SetShaders(iter, 3);
			mesh_mats.push_back({ hash_mesh, iter });
		}

		size_t hash_asset_Render = _EngineSystem.CreateRenderAsset(g_szName_ra + szName, mesh_mats);

		size_t hash_STB_vertices = _EngineSystem.CreateComputeVertices(hash_mesh);
		size_t hash_material_Compute = _EngineSystem.CreateMaterial(g_szName_mat + L"Compute_" + szName);
		_EngineSystem.Material_SetCS(hash_material_Compute, L"CS_Collision_Triangle.hlsl");
		std::vector<TX_HASH> tx_hashs_compute;
		tx_hashs_compute.push_back({ E_Texture::Compute_SRV, hash_STB_vertices });
		tx_hashs_compute.push_back({ E_Texture::Compute_UAV, g_hash_stb_collisionResults });
		_EngineSystem.Material_SetTextures(hash_material_Compute, tx_hashs_compute);
		size_t hash_asset_Compute = _EngineSystem.CreateComputeAsset(g_szName_ca + szName, { hash_material_Compute });

		//ECS Initialize(test, 251111)
		ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Info, C_Transform, C_Render, C_Collider, C_Animation, C_Compute, T_Render_Geometry_Skeletal>();

		size_t lookup = _ECSSystem.CreateEntity<C_Info, C_Transform, C_Render, C_Collider, C_Animation, C_Compute, T_Render_Geometry_Skeletal>();

		_ECSSystem.AddComponent<C_Info>(key, { szName, lookup });

		_ECSSystem.AddComponent<C_Transform>(key, { {0.3f, 0.3f, 0.3f}, Quaternion(0.0f, 90.0f, 0.0f), {-150.0f, 0.0f, -100.0f} });

		_ECSSystem.AddComponent<C_Render>(key, { true, hash_asset_Render });

		_ECSSystem.AddComponent<C_Compute>(key, { hash_asset_Compute });
		_AnimationSystem.AddAnimbones(lookup);
		_ECSSystem.AddComponent<C_Animation>(key, { hash_animation, lookup });

		_ECSSystem.AddComponent<C_Collider>(key, { E_Collider::SPHERE });

	}
#endif // _Praying

#ifdef _Herald
	//_Herald
	{
		const std::wstring szName = L"Herald";
		size_t hash_geometry = _EngineSystem.CreateGeometry(L"../Assets/Meshes/Herald.fbx");

		size_t hash_mesh = _EngineSystem.CreateMeshFromGeometry<Vertex_PTNTB_Skinned>(hash_geometry);
		_EngineSystem.CreateColliders(hash_mesh, E_Collider::AABB);

		size_t hash_animation = _EngineSystem.CreateAnimaitonFromGeometry(hash_geometry);
		std::vector<size_t> hashs_material = _EngineSystem.CreateMaterialsFromGeometry(hash_geometry);
		std::vector<Mesh_Material> mesh_mats;
		for (auto& iter : hashs_material)
		{
			_EngineSystem.Material_SetShaders(iter, 3);
			mesh_mats.push_back({ hash_mesh, iter });
		}

		size_t hash_asset_Render = _EngineSystem.CreateRenderAsset(g_szName_ra + szName, mesh_mats);

		size_t hash_STB_vertices = _EngineSystem.CreateComputeVertices(hash_mesh);
		size_t hash_material_Compute = _EngineSystem.CreateMaterial(g_szName_mat + L"Compute_" + szName);
		_EngineSystem.Material_SetCS(hash_material_Compute, L"CS_Collision_Triangle.hlsl");
		std::vector<TX_HASH> tx_hashs_compute;
		tx_hashs_compute.push_back({ E_Texture::Compute_SRV, hash_STB_vertices });
		tx_hashs_compute.push_back({ E_Texture::Compute_UAV, g_hash_stb_collisionResults });
		_EngineSystem.Material_SetTextures(hash_material_Compute, tx_hashs_compute);

		size_t hash_asset_Compute = _EngineSystem.CreateComputeAsset(g_szName_ca + szName, { hash_material_Compute });

		//ECS Initialize(test, 251111)
		ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Info, C_Transform, C_Render, C_Collider, C_Compute, C_Animation, T_Render_Geometry_Skeletal>();

		size_t lookup = _ECSSystem.CreateEntity<C_Info, C_Transform, C_Render, C_Collider, C_Compute, C_Animation, T_Render_Geometry_Skeletal>();

		_ECSSystem.AddComponent<C_Info>(key, { szName, lookup });

		_ECSSystem.AddComponent<C_Transform>(key, { {0.001f, 0.001f, 0.001f}, Quaternion(30.0f, 95.0f, 0.0f), {0.0f, 0.0f, 0.0f} });

		_ECSSystem.AddComponent<C_Render>(key, { true, hash_asset_Render });

		_AnimationSystem.AddAnimbones(lookup);
		_ECSSystem.AddComponent<C_Animation>(key, { hash_animation, lookup });

		_ECSSystem.AddComponent<C_Compute>(key, { hash_asset_Compute });

		_ECSSystem.AddComponent<C_Collider>(key, { E_Collider::AABB });
	}
#endif // _Praying

#ifdef _Rapunzel
	//_Rapunzel
	{
		const std::wstring szName = L"Rapunzel";
		size_t hash_geometry = _EngineSystem.CreateGeometry(L"../Assets/Meshes/Rapunzel.fbx");

		size_t hash_mesh = _EngineSystem.CreateMeshFromGeometry<Vertex_PTN_Skinned>(hash_geometry);
		_EngineSystem.CreateColliders(hash_mesh, E_Collider::AABB);

		size_t hash_animation = _EngineSystem.CreateAnimaitonFromGeometry(hash_geometry);
		std::vector<size_t> hashs_material = _EngineSystem.CreateMaterialsFromGeometry(hash_geometry);
		std::vector<Mesh_Material> mesh_mats;
		for (auto& iter : hashs_material)
		{
			_EngineSystem.Material_SetShaders(iter, 2);
			mesh_mats.push_back({ hash_mesh, iter });
		}

		size_t hash_asset_Render = _EngineSystem.CreateRenderAsset(g_szName_ra + szName, mesh_mats);

		//ECS Initialize(test, 251111)
		ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Info, C_Transform, C_Render, C_Collider, C_Animation, T_Render_Geometry_Skeletal>();

		size_t lookup = _ECSSystem.CreateEntity<C_Info, C_Transform, C_Render, C_Collider, C_Animation, T_Render_Geometry_Skeletal>();

		_ECSSystem.AddComponent<C_Info>(key, { szName, lookup });

		_ECSSystem.AddComponent<C_Transform>(key, { {10.0f, 10.0f, 10.0f}, Quaternion(0.0f, 180.0f, 0.0f), {-100.0f, 0.0f, -40.0f} });

		_ECSSystem.AddComponent<C_Render>(key, { true, hash_asset_Render });

		_AnimationSystem.AddAnimbones(lookup);
		_ECSSystem.AddComponent<C_Animation>(key, { hash_animation, lookup });
	}
#endif // _Rapunzel
	
#ifdef _Doro
	//_Doro
	{
		const std::wstring szName = L"Doro";
		size_t hash_geometry = _EngineSystem.CreateGeometry(L"../Assets/Meshes/Doro.fbx");

		size_t hash_mesh = _EngineSystem.CreateMeshFromGeometry<Vertex_PTN>(hash_geometry);
		_EngineSystem.CreateColliders(hash_mesh, E_Collider::SPHERE);

		size_t hash_animation = _EngineSystem.CreateAnimaitonFromGeometry(hash_geometry);
		std::vector<size_t> hashs_material = _EngineSystem.CreateMaterialsFromGeometry(hash_geometry);
		std::vector<Mesh_Material> mesh_mats;
		for (auto& iter : hashs_material)
		{
			_EngineSystem.Material_SetShaders(iter, 0);
			mesh_mats.push_back({ hash_mesh, iter });
		}

		size_t hash_asset_Render = _EngineSystem.CreateRenderAsset(g_szName_ra + szName, mesh_mats);

		//ECS Initialize(test, 251111)
		ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Info, C_Transform, C_Render, C_Collider, C_Animation, T_Render_Geometry_Static>();

		size_t lookup = _ECSSystem.CreateEntity<C_Info, C_Transform, C_Render, C_Collider, C_Animation, T_Render_Geometry_Static>();

		_ECSSystem.AddComponent<C_Info>(key, { szName, lookup });

		_ECSSystem.AddComponent<C_Transform>(key, { {10.0f, 10.0f, 10.0f}, Quaternion(90.0f, 0.0f, 0.0f), {-50.0f, 10.0f, -40.0f} });

		_ECSSystem.AddComponent<C_Render>(key, { true, hash_asset_Render });

		_AnimationSystem.AddAnimbones(lookup);
		_ECSSystem.AddComponent<C_Animation>(key, { hash_animation, lookup });

		_ECSSystem.AddComponent<C_Collider>(key, { E_Collider::SPHERE });
	}
#endif // _Doro

#ifdef _Zhao
	//_Zhao
	{
		const std::wstring szName = L"Zhao";
		size_t hash_geometry = _EngineSystem.CreateGeometry(L"../Assets/Meshes/Zhao.fbx");

		size_t hash_mesh = _EngineSystem.CreateMeshFromGeometry<Vertex_PTN>(hash_geometry);
		_EngineSystem.CreateColliders(hash_mesh, E_Collider::AABB);

		size_t hash_animation = _EngineSystem.CreateAnimaitonFromGeometry(hash_geometry);
		std::vector<size_t> hashs_material = _EngineSystem.CreateMaterialsFromGeometry(hash_geometry);
		std::vector<Mesh_Material> mesh_mats;
		for (auto& iter : hashs_material)
		{
			_EngineSystem.Material_SetShaders(iter, 0);
			mesh_mats.push_back({ hash_mesh, iter });
		}

		size_t hash_asset_Render = _EngineSystem.CreateRenderAsset(g_szName_ra + szName, mesh_mats);

		//ECS Initialize(test, 251111)
		ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Info, C_Transform, C_Render, C_Collider, C_Animation, T_Render_Geometry_Static>();

		size_t lookup = _ECSSystem.CreateEntity<C_Info, C_Transform, C_Render, C_Collider, C_Animation, T_Render_Geometry_Static>();

		_ECSSystem.AddComponent<C_Info>(key, { szName, lookup });

		_ECSSystem.AddComponent<C_Transform>(key, { {10.0f, 10.0f, 10.0f}, Quaternion(90.0f, 0.0f, 0.0f), {-50.0f, 10.0f, -80.0f} });

		_ECSSystem.AddComponent<C_Render>(key, { true, hash_asset_Render });

		_AnimationSystem.AddAnimbones(lookup);
		_ECSSystem.AddComponent<C_Animation>(key, { hash_animation, lookup });

		_ECSSystem.AddComponent<C_Collider>(key, { E_Collider::AABB });
	}
#endif // _Zhao

#ifdef _DRAGON
	//_DRAGON
	{
		size_t hash_geometry = _EngineSystem.CreateGeometry(L"../Assets/Meshes/dragon/Dragon 2.5_fbx.fbx");
		//size_t hash_mesh = _EngineSystem.CreateMeshFromGeometry<Vertex_PTNTB>(hash_geometry);
		//std::map<UINT, MTL_TEXTURES> textures;
		//size_t hash_mesh = _EngineSystem.CreateMesh<Vertex_PTNTB>(L"../Assets/Meshes/MechanicGirl/Mechanic_Girl-85698ecb/obj/OBJ/SK_MechanicGirl_AllPartsTogether.obj");
		//size_t hash_mesh = _EngineSystem.CreateMesh<Vertex_PTNTB>(L"../Assets/Meshes/dragon/Dragon 2.5_fbx.fbx", textures);

		//std::vector<size_t> hash_materials(3, _EngineSystem.CreateMaterial<Vertex_PTN>(L"Mat_MW"));
		//for (const auto& hash_material : hash_materials)
		//{
		//	_EngineSystem.Material_SetVS(hash_material, L"VS_PTN.hlsl");
		//	_EngineSystem.Material_SetPS(hash_material, L"PS_PTN.hlsl");
		//	_EngineSystem.Material_SetIL<Vertex_PTNTB>(hash_material, L"VS_PTN.hlsl");
		//}

		//std::vector<TX_HASH> tx_hashs[3];
		//tx_hashs[0].push_back({ E_Texture::Diffuse, _EngineSystem.CreateTexture(L"../Assets/Textures/butter.dds") });
		//_EngineSystem.Material_SetTextures(hash_materials[0], tx_hashs[0]);
		//tx_hashs[1].push_back({ E_Texture::Diffuse, _EngineSystem.CreateTexture(L"../Assets/Textures/butter.dds") });
		//_EngineSystem.Material_SetTextures(hash_materials[1], tx_hashs[1]);
		//tx_hashs[2].push_back({ E_Texture::Diffuse, _EngineSystem.CreateTexture(L"../Assets/Textures/butter.dds") });
		//_EngineSystem.Material_SetTextures(hash_materials[2], tx_hashs[2]);

		//std::vector<Mesh_Material> mesh_mats;
		//mesh_mats.push_back({ hash_mesh, hash_materials[0] });
		//mesh_mats.push_back({ hash_mesh, hash_materials[1] });
		//mesh_mats.push_back({ hash_mesh, hash_materials[2] });
		//size_t hash_asset_Render = _EngineSystem.CreateRenderAsset(L"ra_MW", mesh_mats);

		//const std::unordered_set<size_t>& hash_CLs = _EngineSystem.CreateColliders(hash_mesh, E_Collider::SPHERE);
		//size_t hash_ca = _EngineSystem.CreateColliderAsset(L"ca_MW", hash_CLs);

		////ECS Initialize(test, 251111)
		//ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Transform, C_Render, C_Collider, T_Render_Geometry>();

		//size_t lookup = _ECSSystem.CreateEntity<C_Transform, C_Render, C_Collider, T_Render_Geometry>();

		//_ECSSystem.AddComponent<C_Transform>(key, { {0.3f, 0.3f, 0.3f}, Quaternion(90.0f, 90.0f, 0.0f), {-100.0f, 0.0f, -50.0f} });

		//_ECSSystem.AddComponent<C_Render>(key, { true, hash_asset_Render });

		//_ECSSystem.AddComponent<C_Collider>(key, { hash_ca });
	}

#endif // _DRAGON
	
	
#ifdef _EnviornmentMap
	{
		const std::wstring szName = L"CubeMap_Sphere";
		Vector3 CamPos(0.0f, 50.0f, 30.0f);

		//Initilze Cubemap
		{
			//큐브맵 텍스쳐
			{
				UINT cubemapsize = 256;
				_EngineSystem.m_vp_CubeMap.Width = _EngineSystem.m_vp_CubeMap.Height = cubemapsize;
				_EngineSystem.m_hash_SRV_CubeMap = _EngineSystem.m_hash_RTV_CubeMap = _EngineSystem.CreateViews(g_szName_View + L"SRTV_" + szName, _Target_Cubemap_ResourceView, _EngineSystem.m_vp_CubeMap.Width, _EngineSystem.m_vp_CubeMap.Height);
				_EngineSystem.m_hash_DSV_CubeMap = _EngineSystem.CreateViews(g_szName_View + L"SDSV_" + szName, _Target_Cubemap_DepthView, _EngineSystem.m_vp_CubeMap.Width, _EngineSystem.m_vp_CubeMap.Height);
			}

			//큐브맵 셰이더
			{
				size_t hash_material;
				hash_material = _EngineSystem.CreateMaterial(g_szName_mat + L"PTN_" + szName);
				_EngineSystem.Material_SetVS(hash_material, L"VS_CubeMap_PTN.hlsl");
				_EngineSystem.Material_SetIL<Vertex_PTN>(hash_material, L"VS_CubeMap_PTN.hlsl");
				_EngineSystem.Material_SetGS(hash_material, L"GS_CubeMap_PTN.hlsl");
				_EngineSystem.Material_SetPS(hash_material, L"PS_PTN.hlsl");		
				_EngineSystem.m_hash_Mat_CubeMap_PTN = hash_material;

				hash_material = _EngineSystem.CreateMaterial(g_szName_mat + L"PTN_Skinned_" + szName);
				_EngineSystem.Material_SetVS(hash_material, L"VS_CubeMap_PTN_Skinned.hlsl");
				_EngineSystem.Material_SetIL<Vertex_PTN_Skinned>(hash_material, L"VS_CubeMap_PTN_Skinned.hlsl");
				_EngineSystem.Material_SetGS(hash_material, L"GS_CubeMap_PTN_Skinned.hlsl");
				_EngineSystem.Material_SetPS(hash_material, L"PS_PTN.hlsl");
				_EngineSystem.m_hash_Mat_CubeMap_PTN_Skinned = hash_material;

				hash_material = _EngineSystem.CreateMaterial(g_szName_mat + L"PTNTB_" + szName);
				_EngineSystem.Material_SetVS(hash_material, L"VS_CubeMap_PTNTB.hlsl");
				_EngineSystem.Material_SetIL<Vertex_PTNTB>(hash_material, L"VS_CubeMap_PTNTB.hlsl");
				_EngineSystem.Material_SetGS(hash_material, L"GS_CubeMap_PTNTB.hlsl");
				_EngineSystem.Material_SetPS(hash_material, L"PS_PTNTB.hlsl");
				_EngineSystem.m_hash_Mat_CubeMap_PTNTB = hash_material;

				hash_material = _EngineSystem.CreateMaterial(g_szName_mat + L"PTNTB_Skinned" + szName);
				_EngineSystem.Material_SetVS(hash_material, L"VS_CubeMap_PTNTB_Skinned.hlsl");
				_EngineSystem.Material_SetIL<Vertex_PTNTB_Skinned>(hash_material, L"VS_CubeMap_PTNTB_Skinned.hlsl");
				_EngineSystem.Material_SetGS(hash_material, L"GS_CubeMap_PTNTB_Skinned.hlsl");
				_EngineSystem.Material_SetPS(hash_material, L"PS_PTNTB.hlsl");
				_EngineSystem.m_hash_Mat_CubeMap_PTNTB_Skinned = hash_material;
			}
			
			//큐브맵 카메라 세팅
			{
				ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Transform, C_Camera, C_Projection, T_Camera_Cubemap>();
				_CameraSystem.lookup_cubemapcam = _ECSSystem.CreateEntity<C_Transform, C_Camera, C_Projection, T_Camera_Cubemap>();

				_ECSSystem.AddComponent<C_Transform>(key, { {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, CamPos });

				C_Camera camera;
				camera.fScreenWidth = _EngineSystem.m_vp_CubeMap.Width; 
				camera.fScreenHeight = _EngineSystem.m_vp_CubeMap.Height;
				camera.fFov = 90.0f;	//큐브맵의 카메라는 90도여야한다
				camera.fNear = 0.1f;
				camera.fFar = 10000.0f;
				_ECSSystem.AddComponent<C_Camera>(key, std::move(camera));
			}

			//큐브맵 적용객체
			{
				size_t hash_geometry = _EngineSystem.CreateGeometry(L"../Assets/Meshes/sphere.obj");
				size_t hash_mesh = _EngineSystem.CreateMeshFromGeometry<Vertex_PTN>(hash_geometry);
				size_t hash_material = _EngineSystem.CreateMaterial(g_szName_mat + szName);

				_EngineSystem.Material_SetVS(hash_material, L"VS_PTN.hlsl");
				_EngineSystem.Material_SetIL<Vertex_PTN>(hash_material, L"VS_PTN.hlsl");
				_EngineSystem.Material_SetPS(hash_material, L"PS_CubeMap_PTN.hlsl");		//큐브맵 텍스쳐를 적용하기위해 별도의 PS를 이용한다

				std::vector<TX_HASH> tx_hashs;
				tx_hashs.push_back({ E_Texture::Diffuse, _EngineSystem.CreateTexture(L"../Assets/Textures/metal.jpg") });
				_EngineSystem.Material_SetTextures(hash_material, tx_hashs);
				std::vector<Mesh_Material> mesh_mats;
				mesh_mats.push_back({ hash_mesh, hash_material });
				size_t hash_asset_Render = _EngineSystem.CreateRenderAsset(g_szName_ra + szName, mesh_mats);

				const std::vector<size_t>& hash_CLs = _EngineSystem.CreateColliders(hash_mesh, E_Collider::SPHERE);
				_EngineSystem.CreateColliders(hash_mesh, E_Collider::SPHERE);

				ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Info, C_Transform, C_Render, C_Collider, T_Render_Geometry_Static, T_Render_CubeMap>();

				size_t lookup = _ECSSystem.CreateEntity<C_Info, C_Transform, C_Render, C_Collider, T_Render_Geometry_Static, T_Render_CubeMap>();

				_ECSSystem.AddComponent<C_Info>(key, { szName, lookup });

				_ECSSystem.AddComponent<C_Transform>(key, { {30.0f, 30.0f, 30.0f}, {0.0f, 0.0f, 0.0f}, CamPos });

				_ECSSystem.AddComponent<C_Render>(key, { true, hash_asset_Render });

				_ECSSystem.AddComponent<C_Collider>(key, { E_Collider::SPHERE });
			}
		}
	}
#endif //_EnvironmentMap

	//ShadowMap
	{
		const std::wstring szName = L"ShadowMap";
		_EngineSystem.m_hash_DSV_ShadowMap = _EngineSystem.CreateViews(szName, _Target_DepthView, g_iWidth, g_iHeight);
		size_t hash_material = _EngineSystem.CreateMaterial(g_szName_mat + szName);
		_EngineSystem.Material_SetPS(hash_material, L"PS_ShadowMap.hlsl");
		_EngineSystem.m_hash_Mat_ShadowMap = hash_material;
	}

	//Outline Shader
	{
		const std::wstring szName = L"Outline";
		size_t hash_material;

		hash_material = _EngineSystem.CreateMaterial(g_szName_mat + L"PTN_" + szName);
		_EngineSystem.Material_SetVS(hash_material, L"VS_Outline_PTN.hlsl");
		_EngineSystem.Material_SetIL<Vertex_PTN>(hash_material, L"VS_Outline_PTN.hlsl");
		_EngineSystem.Material_SetPS(hash_material, L"PS_Outline_Draw.hlsl");
		_EngineSystem.m_hash_Mat_Outline_PTN = hash_material;

		hash_material = _EngineSystem.CreateMaterial(g_szName_mat + L"PTN_Skinned_" + szName);
		_EngineSystem.Material_SetVS(hash_material, L"VS_Outline_PTN_Skinned.hlsl");
		_EngineSystem.Material_SetIL<Vertex_PTN_Skinned>(hash_material, L"VS_Outline_PTN_Skinned.hlsl");
		_EngineSystem.Material_SetPS(hash_material, L"PS_Outline_Draw.hlsl");
		_EngineSystem.m_hash_Mat_Outline_PTN_Skinned = hash_material;

		hash_material = _EngineSystem.CreateMaterial(g_szName_mat + L"PTNTB_" + szName);
		_EngineSystem.Material_SetVS(hash_material, L"VS_Outline_PTNTB.hlsl");
		_EngineSystem.Material_SetIL<Vertex_PTNTB>(hash_material, L"VS_Outline_PTNTB.hlsl");
		_EngineSystem.Material_SetPS(hash_material, L"PS_Outline_Draw.hlsl");
		_EngineSystem.m_hash_Mat_Outline_PTNTB = hash_material;

		hash_material = _EngineSystem.CreateMaterial(g_szName_mat + L"PTNTB_Skinned" + szName);
		_EngineSystem.Material_SetVS(hash_material, L"VS_Outline_PTNTB_Skinned.hlsl");
		_EngineSystem.Material_SetIL<Vertex_PTNTB_Skinned>(hash_material, L"VS_Outline_PTNTB_Skinned.hlsl");
		_EngineSystem.Material_SetPS(hash_material, L"PS_Outline_Draw.hlsl");
		_EngineSystem.m_hash_Mat_Outline_PTNTB_Skinned = hash_material;
	}

	//Initialize RTV, DSV
	{
		//Init TargetView geometry
		std::map<UINT, std::vector<Vertex_PTNTB_Skinned>> verticesByMaterial;
		std::map<UINT, std::vector<UINT>> indicesByMaterial;
		std::vector<std::vector<Vector3>> pointsByMeshs;
		GeometryGenerate_Plane(pointsByMeshs, verticesByMaterial[0], indicesByMaterial[0]);
		size_t hash_mesh = _EngineSystem.CreateMeshFromGeometry<Vertex_PT>(L"Plane", verticesByMaterial, indicesByMaterial, pointsByMeshs);
		//RTV
		{
			const std::wstring szName = L"RTT_UI_RTV";
			size_t hash_material = _EngineSystem.CreateMaterial(g_szName_mat + szName);
			_EngineSystem.Material_SetVS(hash_material, L"VS_PT.hlsl");
			_EngineSystem.Material_SetPS(hash_material, L"PS_Distortion.hlsl");
			_EngineSystem.Material_SetIL<Vertex_PT>(hash_material, L"VS_PT.hlsl");

			std::vector<TX_HASH> tx_hashs;
			tx_hashs.push_back({ E_Texture::Diffuse, _EngineSystem.m_hash_RTV_0 });
			_EngineSystem.Material_SetTextures(hash_material, tx_hashs);
			std::vector<Mesh_Material> mesh_mats;
			mesh_mats.push_back({ hash_mesh, hash_material });
			size_t hash_asset_Render = _EngineSystem.CreateRenderAsset(g_szName_ra + szName, mesh_mats);

			ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Info, C_Transform, C_Render, T_Render_UI>();
			size_t lookup = _ECSSystem.CreateEntity<C_Info, C_Transform, C_Render, T_Render_UI>();

			_ECSSystem.AddComponent<C_Info>(key, { szName, lookup });
			_ECSSystem.AddComponent<C_Transform>(key, { {(float)g_iWidth, (float)g_iHeight, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f} });
			_ECSSystem.AddComponent<C_Render>(key, { true, hash_asset_Render });
		}
		//DSV
		{
			const std::wstring szName = L"RTT_UI_DSV";
			size_t hash_material = _EngineSystem.CreateMaterial(g_szName_mat + szName);
			_EngineSystem.Material_SetVS(hash_material, L"VS_PT.hlsl");
			_EngineSystem.Material_SetPS(hash_material, L"PS_DepthDebug.hlsl");
			_EngineSystem.Material_SetIL<Vertex_PT>(hash_material, L"VS_PT.hlsl");

			std::vector<TX_HASH> tx_hashs;
			//_EngineSystem.m_hash_DSV_ShadowMap
			//tx_hashs.push_back({ E_Texture::Diffuse, _EngineSystem.m_hash_DSV_ShadowMap });
			tx_hashs.push_back({ E_Texture::Diffuse, _EngineSystem.m_hash_DSV_0 });
			_EngineSystem.Material_SetTextures(hash_material, tx_hashs);
			std::vector<Mesh_Material> mesh_mats;
			mesh_mats.push_back({ hash_mesh, hash_material });
			size_t hash_asset_Render = _EngineSystem.CreateRenderAsset(g_szName_ra + szName, mesh_mats);

			ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Info, C_Transform, C_Render, T_Render_UI>();
			size_t lookup = _ECSSystem.CreateEntity<C_Info, C_Transform, C_Render, T_Render_UI>();

			_ECSSystem.AddComponent<C_Info>(key, { szName, lookup });

			_ECSSystem.AddComponent<C_Transform>(key, { {300.0f, 300.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f} });
			_ECSSystem.AddComponent<C_Render>(key, { true, hash_asset_Render });
		}
	}

#ifdef _LEGACY
	//gizmo
	{
		std::vector<std::vector<Vector3>> points;
		std::vector<Vertex_PC> iTriangleCount;
		std::vector<UINT> indices;
		GeometryGenerate_Gizmo(points, iTriangleCount, indices);
		size_t hash_mesh = _EngineSystem.CreateMeshFromGeometry<Vertex_PC>(L"Gizmo", std::move(points), std::move(iTriangleCount), std::move(indices), E_Collider::AABB);
		{
			size_t hash_material = _EngineSystem.CreateMaterial<Vertex_PC>(L"Mat_Gizmo", L"VS_PC.hlsl", L"PS_PC.hlsl");
			_EngineSystem.Gizmo = new TempObj();
			TempObj* obj = _EngineSystem.Gizmo;
			obj->m_vScale = Vector3(100.0f, 100.0f, 100.0f);
			obj->m_vPosition = Vector3(0.0f, 0.0f, 0.0f);
			obj->m_Mesh_Material.push_back({ hash_mesh , hash_material });
		}
	}

	//PardCode22(earth)
	{
		size_t hash_mesh = _EngineSystem.CreateMesh<Vertex_PTN>(L"../Assets/Meshes/sphere.obj", E_Collider::SPHERE);
		size_t mash_material = _EngineSystem.CreateMaterial<Vertex_PTN>(L"Mat_Earth", L"VS_PTN.hlsl", L"PS_TextureFlow.hlsl");
		std::vector<TX_HASH> hash_tx_hash;
		hash_tx_hash.push_back({ E_Texture::Diffuse, _EngineSystem.CreateTexture(L"../Assets/Textures/PardCode22/earth_color.jpg", WIC_FLAGS_NONE) });
		hash_tx_hash.push_back({ E_Texture::Diffuse, _EngineSystem.CreateTexture(L"../Assets/Textures/PardCode22/earth_night.jpg", WIC_FLAGS_NONE) });
		hash_tx_hash.push_back({ E_Texture::Diffuse, _EngineSystem.CreateTexture(L"../Assets/Textures/PardCode22/earth_clouds.jpg", WIC_FLAGS_NONE) });
		hash_tx_hash.push_back({ E_Texture::Diffuse, _EngineSystem.CreateTexture(L"../Assets/Textures/PardCode22/earth_spec.jpg", WIC_FLAGS_NONE) });
		_EngineSystem.Material_SetTextures(mash_material, hash_tx_hash);

		_EngineSystem.objs.push_back(new TempObj());
		TempObj* obj = _EngineSystem.objs.back();
		obj->m_vScale = Vector3(100.0f, 100.0f, 100.0f);
		obj->m_vRotate = Vector3(0.0f, 0.0f, 0.0f);
		obj->m_vPosition = Vector3(0.0f, 0.0f, 0.0f);
		obj->m_Mesh_Material.push_back({ hash_mesh , mash_material });
	}

	//ReflectPlane
	{
		std::vector<std::vector<Vector3>> points;
		std::vector<Vertex_PTN> iTriangleCount;
		std::vector<UINT> indices;
		GeometryGenerate_Plane(points, iTriangleCount, indices);
		size_t hash_mesh = _EngineSystem.CreateMeshFromGeometry<Vertex_PTN>(L"Plane_Reflect", std::move(points), std::move(iTriangleCount), std::move(indices), E_Collider::AABB);
		{
			size_t hash_material = _EngineSystem.CreateMaterial<Vertex_PTN>(L"Mat_Reflect", L"VS_PTN.hlsl", L"PS_PTN_Transparent.hlsl");
			std::vector<TX_HASH> tx_hash;
			tx_hash.push_back({ E_Texture::Diffuse, _EngineSystem.CreateTexture(L"../Assets/Textures/blue01.dds", DDS_FLAGS_NONE) });
			_EngineSystem.Material_SetTextures(hash_material, tx_hash);

			_EngineSystem.ReflectPlane = new TempObj();
			TempObj* obj = _EngineSystem.ReflectPlane;
			obj->m_vScale = Vector3(300.0f, 300.0f, 300.0f);
			obj->m_vRotate = Vector3(90.0f, 0.0f, 0.0f);
			obj->m_vPosition = Vector3(0.0f, -150.0f, 0.0f);
			obj->m_Mesh_Material.push_back({ hash_mesh , hash_material });
		}
	}

	//BlendCheck
	{
		size_t hash_mesh = _EngineSystem.CreateMesh<Vertex_PTN>(L"../Assets/Meshes/Cube.obj", E_Collider::AABB);
		size_t hash_material = _EngineSystem.CreateMaterial<Vertex_PTN>(L"../Assets/Meshes/house.mtl", L"VS_PTN.hlsl", L"PS_PTN.hlsl");
		std::vector<TX_HASH> hash_tx_hash;
		hash_tx_hash.push_back({ E_Texture::Diffuse, _EngineSystem.CreateTexture(L"../Assets/Textures/wireFence.dds", DDS_FLAGS_NONE) });
		_EngineSystem.Material_SetTextures(hash_material, hash_tx_hash);

		_EngineSystem.objs.push_back(new TempObj());
		TempObj* obj = _EngineSystem.objs.back();
		obj->m_vScale = Vector3(300.0f, 300.0f, 300.0f);
		obj->m_vRotate = Vector3(0.0f, 0.0f, 0.0f);
		obj->m_vPosition = Vector3(0.0f, 0.0f, 0.0f);
		obj->m_Mesh_Material.push_back({ hash_mesh , hash_material });
	}

	//SpaceShip
	{
		size_t hash_mesh = _EngineSystem.CreateMesh<Vertex_PTN>(L"../Assets/Meshes/spaceship.obj", E_Collider::SPHERE);
		std::vector<std::wstring> VSs, PSs;
		VSs.push_back(L"VS_PTN.hlsl"); PSs.push_back(L"PS_PTN.hlsl");

		std::vector<size_t> hash_mats = _EngineSystem.CreateMaterialsFromFile<Vertex_PTN>(L"../Assets/Meshes/spaceship.mtl", VSs, PSs);
		std::vector<TX_HASH> TX_HASHS[1];
		TX_HASHS[0].push_back({ E_Texture::Diffuse, _EngineSystem.CreateTexture(L"../Assets/Textures/stars_spaceship.jpg", WIC_FLAGS_IGNORE_SRGB) });
		_EngineSystem.Material_SetTextures(hash_mats[0], TX_HASHS[0]);

		_EngineSystem.SpaceShip = new TempObj();
		TempObj* obj = _EngineSystem.SpaceShip;
		obj->m_vScale = Vector3(25.0f, 25.0f, 25.0f);
		obj->m_vRotate = Vector3(0.0f, -90.0f, 0.0f);
		obj->m_Mesh_Material.push_back({ hash_mesh , hash_mats[0] });
	}

	//Asteroid
	{
		size_t hash_mesh = _EngineSystem.CreateMesh<Vertex_PTN>(L"../Assets/Meshes/asteroid.obj", E_Collider::SPHERE);
		std::vector<std::wstring> VSs, PSs;
		VSs.push_back(L"VS_PTN.hlsl"); PSs.push_back(L"PS_PTN.hlsl");

		std::vector<size_t> hash_mats = _EngineSystem.CreateMaterialsFromFile<Vertex_PTN>(L"../Assets/Meshes/asteroid.mtl", VSs, PSs);
		std::vector<TX_HASH> TX_HASHS[1];
		TX_HASHS[0].push_back({ E_Texture::Diffuse, _EngineSystem.CreateTexture(L"../Assets/Textures/stars_asteroid.jpg", WIC_FLAGS_IGNORE_SRGB) });
		_EngineSystem.Material_SetTextures(hash_mats[0], TX_HASHS[0]);

		for (int i = 0; i < 300; i++)
		{
			_EngineSystem.objs.push_back(new TempObj());
			TempObj* obj = _EngineSystem.objs.back();
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
	_TimerSystem.Frame();
	float deltaTime = _TimerSystem.GetDeltaTime();
	float elpasedTime = _TimerSystem.GetElapsedTime();
	float FPS = _TimerSystem.GetFps();
	g_fTime_Log += deltaTime;
	if (g_fTime_Log >= 1.0f)
	{
		std::cout << "OnUpdate" << '\n';
		std::cout << "ElapsedTime : " << elpasedTime << '\n';
		std::cout << "FPS : " << FPS << '\n';
		std::cout << '\n';
	}

	//FrameIntent
	{
		_InputSystem.Frame();						//선결조건1(입력)

		_CameraSystem.Frame(deltaTime);				//선결조건2(개체들)
		_BehaviorSystem.Frame(deltaTime);			
		_AnimationSystem.Frame(deltaTime);

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
	
	if (g_fTime_Log >= 1.0f)
		g_fTime_Log = 0.0f;
}

void AppWindow::OnDestroy()
{
	std::cout << "OnDestroy" << '\n';
	g_bIsRun = false;
}
