#include "RenderSystem.h"
#include "EngineSystem.h"
//Systems
#include "InputSystem.h"
#include "ResourceSystem.h"
#include "ECSSystem.h"
#include "CameraSystem.h"
#include "CollisionSystem.h"

//D3D Wrappers
#include "Direct3D.h"
#include "SwapChain.h"
#include "InputLayout.h"
#include "Buffers.h"
#include "States.h"
#include "Views.h"
#include "Shaders.h"

//Resources
#include "Texture.h"
#include "Mesh.h"
#include "Material.h"
#include "Assets.h"
#include "Geometry.h"
#include "Animation.h"

#include "TestBlockMacro.h"

//해시, 상수버퍼
size_t g_hash_cb_directionalLight;
size_t g_hash_cb_pointlight;
size_t g_hash_cb_spotlight;
size_t g_hash_cb_wvpitmat;
size_t g_hash_cb_time;
size_t g_hash_cb_campos;
size_t g_hash_cb_lightmat;
size_t g_hash_cb_bonemat;
size_t g_hash_cb_fog;
size_t g_hash_cb_debug_box;
size_t g_hash_cb_debug_sphere;

//해시, 디버그렌더
size_t g_hash_VS_Debug;
size_t g_hash_GS_Debug_Box;
size_t g_hash_PS_Debug_PC;
size_t g_hash_VS_Debug_Sphere;
size_t g_hash_HS_Debug_Sphere;
size_t g_hash_DS_Debug_Sphere;
size_t g_hash_PS_Picking;


RenderSystem::RenderSystem()
{
}

void RenderSystem::Init()
{
	//상수버퍼 기본세팅
	g_hash_cb_directionalLight	= _EngineSystem.CreateConstantBuffer(typeid(CB_DirectionalLight), sizeof(CB_DirectionalLight));
	g_hash_cb_pointlight		= _EngineSystem.CreateConstantBuffer(typeid(CB_PointLight), sizeof(CB_PointLight));
	g_hash_cb_spotlight			= _EngineSystem.CreateConstantBuffer(typeid(CB_SpotLight), sizeof(CB_SpotLight));
	g_hash_cb_wvpitmat			= _EngineSystem.CreateConstantBuffer(typeid(CB_WVPITMatrix), sizeof(CB_WVPITMatrix));
	g_hash_cb_time				= _EngineSystem.CreateConstantBuffer(typeid(CB_Time), sizeof(CB_Time));
	g_hash_cb_campos			= _EngineSystem.CreateConstantBuffer(typeid(CB_Campos), sizeof(CB_Campos));
	g_hash_cb_lightmat			= _EngineSystem.CreateConstantBuffer(typeid(CB_LightMatrix), sizeof(CB_LightMatrix));
	g_hash_cb_bonemat			= _EngineSystem.CreateConstantBuffer(typeid(CB_BoneMatrix), sizeof(CB_BoneMatrix));
	g_hash_cb_fog				= _EngineSystem.CreateConstantBuffer(typeid(CB_Fog), sizeof(CB_Fog));
	g_hash_cb_debug_box			= _EngineSystem.CreateConstantBuffer(typeid(CB_Debug_Box), sizeof(CB_Debug_Box));
	g_hash_cb_debug_sphere		= _EngineSystem.CreateConstantBuffer(typeid(CB_Debug_Sphere), sizeof(CB_Debug_Sphere));

	//디버그용 셰이더
	g_hash_VS_Debug				= _EngineSystem.CreateVertexShader(L"VS_Debug.hlsl", "vsmain", "vs_5_0");
	g_hash_GS_Debug_Box			= _EngineSystem.CreateGeometryShader(L"GS_Debug_Box.hlsl", "gsmain", "gs_5_0");
	g_hash_PS_Debug_PC			= _EngineSystem.CreatePixelShader(L"PS_PC.hlsl", "psmain", "ps_5_0");
	g_hash_VS_Debug_Sphere		= _EngineSystem.CreateVertexShader(L"VS_Debug_Sphere.hlsl", "vsmain", "vs_5_0");
	g_hash_HS_Debug_Sphere		= _EngineSystem.CreateHullShader(L"HS_Debug_Sphere.hlsl", "hsmain", "hs_5_0");
	g_hash_DS_Debug_Sphere		= _EngineSystem.CreateDomainShader(L"DS_Debug_Sphere.hlsl", "dsmain", "ds_5_0");
	g_hash_PS_Picking			= _EngineSystem.CreatePixelShader(L"PS_Picking.hlsl", "psmain", "ps_5_0");

	_EngineSystem.OnResize(g_iWidth, g_iHeight);
}

void RenderSystem::Frame(float deltatime, float elapsedtime)
{
	if (g_fTime_Log >= 1.0f)
		std::cout << "Frame : " << "EngineSystem" << " Class" << '\n';

}

void RenderSystem::PreRender(float deltatime, float elapsedtime)
{
	if (g_fTime_Log >= 1.0f)
		std::cout << "PreRender : " << "EngineSystem" << " Class" << '\n';
	
	//RTV초기화
	_EngineSystem.ClearRenderTargetView(_EngineSystem.m_hash_RTV_0, 0.0f, 0.3f, 0.4f, 1.0f);
	_EngineSystem.ClearDepthStencilView(_EngineSystem.m_hash_DSV_0);
	_EngineSystem.SetOM_RenderTargets({ _EngineSystem.m_hash_RTV_0 }, _EngineSystem.m_hash_DSV_0);
	_EngineSystem.SetRS_Viewport(&_EngineSystem.m_vp_BB);
}

void RenderSystem::Render(float deltatime, float elapsedtime)
{
	if (g_fTime_Log >= 1.0f)
		std::cout << "Render : " << "EngineSystem" << " Class" << '\n';
	
	CB_DirectionalLight cb_directional;
	{
		size_t lookup				= _EngineSystem.m_hash_light_directional;
		const auto& lightDir		= _ECSSystem.GetComponent<C_Light_Direction>(lookup);
		const auto& c_light			= _ECSSystem.GetComponent<C_Light>(lookup);
		cb_directional.mAmbient		= c_light.vAmbient;
		cb_directional.mDiffuse		= c_light.vDiffuse;
		cb_directional.mSpecular	= c_light.vSpecular;
		cb_directional.vDirection	= Vector4(Quarternion(lightDir.dir).GetForwardAxis(), c_light.fShiness);
	}
	_EngineSystem.UpdateConstantBuffer(g_hash_cb_directionalLight, &cb_directional);
	_EngineSystem.SetPS_ConstantBuffer(g_hash_cb_directionalLight, 0);

	CB_PointLight cb_point;
	{
		size_t lookup				= _EngineSystem.m_hash_light_point;
		const auto& c_transform		= _ECSSystem.GetComponent<C_Transform>(lookup);
		const auto& c_light			= _ECSSystem.GetComponent<C_Light>(lookup);
		const auto& c_attenuation	= _ECSSystem.GetComponent<C_Light_Attenuation>(lookup);
		cb_point.mAmbient			= c_light.vAmbient;
		cb_point.mDiffuse			= c_light.vDiffuse;
		cb_point.mSpecular			= c_light.vSpecular;
		cb_point.vPosition			= Vector4(c_transform.vPosition, c_light.fShiness);
		cb_point.fAttenuations		= Vector4(c_attenuation.fAtt_a0, c_attenuation.fAtt_a1, c_attenuation.fAtt_a2, c_attenuation.fRange);
	}
	_EngineSystem.UpdateConstantBuffer(g_hash_cb_pointlight, &cb_point);
	_EngineSystem.SetPS_ConstantBuffer(g_hash_cb_pointlight, 1);

	CB_SpotLight cb_spot;
	{
		size_t lookup				= _EngineSystem.m_hash_light_spot;
		const auto& c_transform		= _ECSSystem.GetComponent<C_Transform>(lookup);
		const auto& c_light			= _ECSSystem.GetComponent<C_Light>(lookup);
		const auto& c_attenuation	= _ECSSystem.GetComponent<C_Light_Attenuation>(lookup);
		const auto& c_spot			= _ECSSystem.GetComponent<C_Light_Spot>(lookup);
		cb_spot.mAmbient			= c_light.vAmbient;
		cb_spot.mDiffuse			= c_light.vDiffuse;
		cb_spot.mSpecular			= c_light.vSpecular;
		cb_spot.vDirection			= c_transform.qRotate.GetForwardAxis();
		cb_spot.vPosition			= Vector4(c_transform.vPosition, c_light.fShiness);
		cb_spot.fAttenuations		= Vector4(c_attenuation.fAtt_a0, c_attenuation.fAtt_a1, c_attenuation.fAtt_a2, c_attenuation.fRange);
		cb_spot.fSpots				= Vector4(c_spot.fSpot, c_spot.fCos_OuterCone, c_spot.fCos_InnerCone, 0.0f);
	}
	_EngineSystem.UpdateConstantBuffer(g_hash_cb_spotlight, &cb_spot);
	_EngineSystem.SetPS_ConstantBuffer(g_hash_cb_spotlight, 2);

	CB_LightMatrix cb_lightMat;
	{
		size_t lookup				= _EngineSystem.m_hash_light_directional;
		const auto& lightDir		= _ECSSystem.GetComponent<C_Light_Direction>(lookup);
		const auto& c_cam			= _ECSSystem.GetComponent<C_Camera>(_CameraSystem.lookup_maincam);
		Vector4 pos					= -lightDir.dir * 5000.0f;
		cb_lightMat.matLightView	= GetMat_View(pos.ToVector3(), lightDir.dir);
		cb_lightMat.matLightProj	= GetMat_Orthographic(c_cam.fScreenWidth, c_cam.fScreenHeight, c_cam.fNear, 10000.0f);
		cb_lightMat.vPos			= pos;
	}
	_EngineSystem.UpdateConstantBuffer(g_hash_cb_lightmat, &cb_lightMat);
	_EngineSystem.SetVS_ConstantBuffer(g_hash_cb_lightmat, 1);

	size_t lookup_maincam			= _CameraSystem.lookup_maincam;
	const auto& c_cam_main			= _ECSSystem.GetComponent<C_Camera>(lookup_maincam);
	const auto& c_cam_proj			= _ECSSystem.GetComponent<C_Projection>(lookup_maincam);
	const auto& c_cam_ortho			= _ECSSystem.GetComponent<C_Orthographic>(lookup_maincam);
	const Matrix4x4& cam_matWorld	= c_cam_main.matWorld;
	const Matrix4x4& cam_matView	= c_cam_main.matView;
	const Matrix4x4& cam_matProj	= c_cam_proj.matProj;
	const Matrix4x4& cam_matOrhto	= c_cam_ortho.matOrtho;

	//ShadowMap
	RenderShadowMap(cb_lightMat.matLightView, cb_lightMat.matLightProj);

#ifdef _EnviornmentMap
	RenderCubeMap();
#endif // _EnviornmentMap

	const auto& c_cam_transform = _ECSSystem.GetComponent<C_Transform>(lookup_maincam);
	CB_Campos cb_campos;
	cb_campos.vPosition = c_cam_transform.vPosition;
	_EngineSystem.UpdateConstantBuffer(g_hash_cb_campos, &cb_campos);
	_EngineSystem.SetVS_ConstantBuffer(g_hash_cb_campos, 5);

	//Render SkySphere
	RenderSkySphere(cam_matView, cam_matProj);
	
	//Render Geometry
	RenderGeometry(cam_matView, cam_matProj);

	//Render RTV_CubeMap
#ifdef  _EnviornmentMap
	RenderEnviornmentMap(cam_matView, cam_matProj);
#endif //  _EnvironmentMap

	//Render Billboard
	RenderBillboard(c_cam_transform.vPosition, cam_matView, cam_matProj);

	//Render Pikcing
	if(_InputSystem.IsPressed_LBTN())
		RenderGeometry_Picking(cam_matView, cam_matProj);

	//Render DebugGeometry
	if (_InputSystem.IsDebugRender())
		RenderGeometry_Debug(cam_matView, cam_matProj);

	//Render UI
	RenderUI(cam_matOrhto);
}

void RenderSystem::PostRender()
{
	if (g_fTime_Log >= 1.0f)
		std::cout << "PostRender : " << "EngineSystem" << " Class" << '\n';
	_EngineSystem.SwapchainPresent(false);
}

RenderSystem::~RenderSystem()
{

}


/*
*******************************
* RenderPasses
*******************************
*/

void RenderSystem::RenderSkySphere(const Matrix4x4& matView, const Matrix4x4& matProj)
{
	_EngineSystem.SetOM_BlendState(E_BSState::Opaque, NULL);
	_EngineSystem.SetOM_DepthStenilState(E_DSState::SKYBOX);
	_EngineSystem.SetPS_SamplerState(E_Sampler::LINEAR_WRAP);
	_EngineSystem.SetRS_RasterizerState(E_RSState::SOLID_CULLBACK_CCW);
	ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Transform, C_Render, T_Render_Sky>();
	std::vector<Archetype*> queries = _ECSSystem.QueryArchetypes(key);
	for (auto& archetype : queries)
	{
		size_t st_row = 0;
		size_t st_col = 0;
		for (size_t row = st_row; row < archetype->GetCount_Chunks(); row++)
		{
			auto& transforms = archetype->GetComponents<C_Transform>(row);
			auto& renders = archetype->GetComponents<C_Render>(row);
			for (size_t col = st_col; col < archetype->GetCount_Chunk(row); col++)
			{
				const Vector3& scale = transforms[col].vScale;
				const Quarternion& rotate = transforms[col].qRotate;
				const Vector3& position = transforms[col].vPosition;
				CB_WVPITMatrix cb_wvpitmat;
				cb_wvpitmat.matWorld = GetMat_World(scale, rotate, position);
				cb_wvpitmat.matView = matView;
				cb_wvpitmat.matProj = matProj;
				cb_wvpitmat.matInvTrans = GetMat_InverseTranspose(cb_wvpitmat.matWorld);
				_EngineSystem.UpdateConstantBuffer(g_hash_cb_wvpitmat, &cb_wvpitmat);
				_EngineSystem.SetVS_ConstantBuffer(g_hash_cb_wvpitmat, 0);

				const auto& MeshMats = _ResourceSystem.GetResource<RenderAsset>(renders[col].hash_ra)->m_hMeshMats;
				for (UINT j = 0; j < MeshMats.size(); j++)
				{
					auto& iter = MeshMats[j];
					BaseMesh* pMesh = _ResourceSystem.GetResource<BaseMesh>(iter.hash_mesh);
					_EngineSystem.SetIA_VertexBuffer(pMesh->GetVB());
					_EngineSystem.SetIA_IndexBuffer(pMesh->GetIB());

					Material* pMaterial = _ResourceSystem.GetResource<Material>(iter.hash_material);
					_EngineSystem.SetIA_InputLayout(pMaterial->GetIL());
					_EngineSystem.SetVS_Shader(pMaterial->GetVS());
					_EngineSystem.SetPS_Shader(pMaterial->GetPS());

					const std::vector<size_t>* texs = pMaterial->GetTextures();
					int cnt = 0;
					for (int idxTex = 0; idxTex < (UINT)E_Texture::count; idxTex++)
					{
						for (const auto& hashTx : texs[idxTex])
						{
							size_t hashSRV = _ResourceSystem.GetResource<Texture>(hashTx)->GetSRV();
							_EngineSystem.SetPS_ShaderResourceView(hashSRV, cnt++);
						}
					}
					_EngineSystem.Draw_Indicies(pMesh->GetRendIndices()[j].count, pMesh->GetRendIndices()[j].idx, 0);
				}
			}
			st_col = 0;
		}
	}
}

void RenderSystem::RenderGeometry(const Matrix4x4& matView, const Matrix4x4& matProj)
{
	_EngineSystem.SetOM_BlendState(E_BSState::Opaque, NULL);
	_EngineSystem.SetOM_DepthStenilState(E_DSState::DEFAULT);
	_EngineSystem.SetPS_SamplerState(E_Sampler::LINEAR_WRAP);
	_EngineSystem.SetPS_SamplerState(E_Sampler::POINT_CLAMP_COMPARISON, 6);
	_EngineSystem.SetRS_RasterizerState(E_RSState::SOLID_CULLBACK_CW);
	//static
	{
		ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Transform, C_Render, T_Render_Geometry_Static>();
		std::vector<Archetype*> queries = _ECSSystem.QueryArchetypes(key);
		for (auto& archetype : queries)
		{
			size_t st_row = 0;
			size_t st_col = 0;
			for (size_t row = st_row; row < archetype->GetCount_Chunks(); row++)
			{
				auto& transforms = archetype->GetComponents<C_Transform>(row);
				auto& renders = archetype->GetComponents<C_Render>(row);
				for (size_t col = st_col; col < archetype->GetCount_Chunk(row); col++)
				{
					if (!renders[col].bRenderable) continue;
					const Vector3& scale = transforms[col].vScale;
					const Quarternion& rotate = transforms[col].qRotate;
					const Vector3& position = transforms[col].vPosition;
					CB_WVPITMatrix cb_wvpitmat;
					cb_wvpitmat.matWorld = GetMat_World(scale, rotate, position);
					cb_wvpitmat.matView = matView;
					cb_wvpitmat.matProj = matProj;
					cb_wvpitmat.matInvTrans = GetMat_InverseTranspose(cb_wvpitmat.matWorld);
					_EngineSystem.UpdateConstantBuffer(g_hash_cb_wvpitmat, &cb_wvpitmat);
					_EngineSystem.SetVS_ConstantBuffer(g_hash_cb_wvpitmat, 0);

					const auto& MeshMats = _ResourceSystem.GetResource<RenderAsset>(renders[col].hash_ra)->m_hMeshMats;
					for (UINT j = 0; j < MeshMats.size(); j++)
					{
						auto& iter = MeshMats[j];
						BaseMesh* pMesh = _ResourceSystem.GetResource<BaseMesh>(iter.hash_mesh);
						_EngineSystem.SetIA_VertexBuffer(pMesh->GetVB());
						_EngineSystem.SetIA_IndexBuffer(pMesh->GetIB());

						Material* pMaterial = _ResourceSystem.GetResource<Material>(iter.hash_material);
						_EngineSystem.SetIA_InputLayout(pMaterial->GetIL());
						_EngineSystem.SetVS_Shader(pMaterial->GetVS());
						_EngineSystem.SetGS_Shader(pMaterial->GetGS());
						_EngineSystem.SetPS_Shader(pMaterial->GetPS());

						const std::vector<size_t>* texs = pMaterial->GetTextures();
						int cnt = 0;
						for (int idxTex = 0; idxTex < (UINT)E_Texture::count; idxTex++)
						{
							for (const auto& hashTx : texs[idxTex])
							{
								size_t hashSRV = _ResourceSystem.GetResource<Texture>(hashTx)->GetSRV();
								_EngineSystem.SetPS_ShaderResourceView(hashSRV, cnt++);
							}
						}
						_EngineSystem.Draw_Indicies(pMesh->GetRendIndices()[j].count, pMesh->GetRendIndices()[j].idx, 0);
					}
				}
				st_col = 0;
			}
		}
	}

	//Skeletal
	{
		ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Transform, C_Render, C_Animation, T_Render_Geometry_Skeletal>();
		std::vector<Archetype*> queries = _ECSSystem.QueryArchetypes(key);
		for (auto& archetype : queries)
		{
			size_t st_row = 0;
			size_t st_col = 0;
			for (size_t row = st_row; row < archetype->GetCount_Chunks(); row++)
			{
				auto& transforms = archetype->GetComponents<C_Transform>(row);
				auto& renders = archetype->GetComponents<C_Render>(row);
				auto& animations = archetype->GetComponents<C_Animation>(row);
				for (size_t col = st_col; col < archetype->GetCount_Chunk(row); col++)
				{
					if (!renders[col].bRenderable) continue;
					const Vector3& scale = transforms[col].vScale;
					const Quarternion& rotate = transforms[col].qRotate;
					const Vector3& position = transforms[col].vPosition;

					CB_WVPITMatrix cb_wvpitmat;
					cb_wvpitmat.matWorld = GetMat_World(scale, rotate, position);
					cb_wvpitmat.matView = matView;
					cb_wvpitmat.matProj = matProj;
					cb_wvpitmat.matInvTrans = GetMat_InverseTranspose(cb_wvpitmat.matWorld);
					_EngineSystem.UpdateConstantBuffer(g_hash_cb_wvpitmat, &cb_wvpitmat);
					_EngineSystem.SetVS_ConstantBuffer(g_hash_cb_wvpitmat, 0);

					CB_BoneMatrix cb_bonemat;
					std::memcpy(cb_bonemat.bones, animations[col].matAnims, sizeof(cb_bonemat.bones));
					_EngineSystem.UpdateConstantBuffer(g_hash_cb_bonemat, &cb_bonemat);
					_EngineSystem.SetVS_ConstantBuffer(g_hash_cb_bonemat, 2);

					const auto& MeshMats = _ResourceSystem.GetResource<RenderAsset>(renders[col].hash_ra)->m_hMeshMats;
					for (UINT j = 0; j < MeshMats.size(); j++)
					{
						auto& iter = MeshMats[j];
						BaseMesh* pMesh = _ResourceSystem.GetResource<BaseMesh>(iter.hash_mesh);
						_EngineSystem.SetIA_VertexBuffer(pMesh->GetVB());
						_EngineSystem.SetIA_IndexBuffer(pMesh->GetIB());

						Material* pMaterial = _ResourceSystem.GetResource<Material>(iter.hash_material);
						_EngineSystem.SetIA_InputLayout(pMaterial->GetIL());
						_EngineSystem.SetVS_Shader(pMaterial->GetVS());
						_EngineSystem.SetGS_Shader(pMaterial->GetGS());
						_EngineSystem.SetPS_Shader(pMaterial->GetPS());

						const std::vector<size_t>* texs = pMaterial->GetTextures();
						int cnt = 0;
						for (int idxTex = 0; idxTex < (UINT)E_Texture::count; idxTex++)
						{
							for (const auto& hashTx : texs[idxTex])
							{
								size_t hashSRV = _ResourceSystem.GetResource<Texture>(hashTx)->GetSRV();
								_EngineSystem.SetPS_ShaderResourceView(hashSRV, cnt++);
							}
						}
						_EngineSystem.Draw_Indicies(pMesh->GetRendIndices()[j].count, pMesh->GetRendIndices()[j].idx, 0);
					}
				}
				st_col = 0;
			}
		}
	}
}

void RenderSystem::RenderBillboard(const Vector3& campos, const Matrix4x4& matView, const Matrix4x4& matProj)
{
	_EngineSystem.SetOM_BlendState(E_BSState::Opaque, NULL);
	_EngineSystem.SetOM_DepthStenilState(E_DSState::DEFAULT);
	_EngineSystem.SetPS_SamplerState(E_Sampler::LINEAR_WRAP);
	_EngineSystem.SetRS_RasterizerState(E_RSState::SOLID_CULLBACK_CW);
	ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Transform, C_Render, T_Render_Billboard>();
	std::vector<Archetype*> queries = _ECSSystem.QueryArchetypes(key);
	for (auto& archetype : queries)
	{
		size_t st_row = 0;
		size_t st_col = 0;
		for (size_t row = st_row; row < archetype->GetCount_Chunks(); row++)
		{
			auto& transforms = archetype->GetComponents<C_Transform>(row);
			auto& renders = archetype->GetComponents<C_Render>(row);
			for (size_t col = st_col; col < archetype->GetCount_Chunk(row); col++)
			{
				if (!renders[col].bRenderable) continue;
				const Vector3& scale = transforms[col].vScale;
				const Quarternion& rotate = transforms[col].qRotate;
				const Vector3& position = transforms[col].vPosition;
				CB_WVPITMatrix cb_wvpitmat;
				/*
				float dx = c_cam_transform.vPosition.GetX() - position.GetX();
				float dz = c_cam_transform.vPosition.GetZ() - position.GetZ();
				double angle = _RADTODEG(atan2f(dx, dz)) + 180.0f;
				*/
				//yaw빌보드 구현, +z(화면안쪽) 이므로 물체-카메라로 역방향으로 회전해야 같은yaw로 회전해 빌보드로 보이게된다

				Quarternion qRot((position - campos).Normalize());
				cb_wvpitmat.matWorld = GetMat_Scale(scale) * GetMat_RotYaw(qRot.ToRotate().GetY()) * GetMat_Translation(position);
				cb_wvpitmat.matView = matView;
				cb_wvpitmat.matProj = matProj;
				cb_wvpitmat.matInvTrans = GetMat_InverseTranspose(cb_wvpitmat.matWorld);
				_EngineSystem.UpdateConstantBuffer(g_hash_cb_wvpitmat, &cb_wvpitmat);
				_EngineSystem.SetVS_ConstantBuffer(g_hash_cb_wvpitmat, 0);

				const auto& MeshMats = _ResourceSystem.GetResource<RenderAsset>(renders[col].hash_ra)->m_hMeshMats;
				for (UINT j = 0; j < MeshMats.size(); j++)
				{
					auto& iter = MeshMats[j];
					BaseMesh* pMesh = _ResourceSystem.GetResource<BaseMesh>(iter.hash_mesh);
					_EngineSystem.SetIA_VertexBuffer(pMesh->GetVB());
					_EngineSystem.SetIA_IndexBuffer(pMesh->GetIB());

					Material* pMaterial = _ResourceSystem.GetResource<Material>(iter.hash_material);
					_EngineSystem.SetIA_InputLayout(pMaterial->GetIL());
					_EngineSystem.SetVS_Shader(pMaterial->GetVS());
					_EngineSystem.SetGS_Shader(pMaterial->GetGS());
					_EngineSystem.SetPS_Shader(pMaterial->GetPS());

					const std::vector<size_t>* texs = pMaterial->GetTextures();
					int cnt = 0;
					for (int idxTex = 0; idxTex < (UINT)E_Texture::count; idxTex++)
					{
						for (const auto& hashTx : texs[idxTex])
						{
							size_t hashSRV = _ResourceSystem.GetResource<Texture>(hashTx)->GetSRV();
							_EngineSystem.SetPS_ShaderResourceView(hashSRV, cnt++);
						}
					}
					_EngineSystem.Draw_Indicies(pMesh->GetRendIndices()[j].count, pMesh->GetRendIndices()[j].idx, 0);
				}
			}
			st_col = 0;
		}
	}
}

void RenderSystem::RenderShadowMap(const Matrix4x4& matView, const Matrix4x4& matProj)
{
	_EngineSystem.SetOM_BlendState(E_BSState::Opaque, NULL);
	_EngineSystem.SetOM_DepthStenilState(E_DSState::DEFAULT);
	_EngineSystem.SetPS_SamplerState(E_Sampler::LINEAR_WRAP);
	_EngineSystem.SetRS_RasterizerState(E_RSState::SOLID_CULLBACK_CW);

	//그림자맵 텍스쳐를 구성하기위한 타겟(rtv null, dsv만) 세팅
	_EngineSystem.ClearDepthStencilView(_EngineSystem.m_hash_DSV_ShadowMap);
	_EngineSystem.SetOM_RenderTargets(std::vector<size_t>(), _EngineSystem.m_hash_DSV_ShadowMap);


	//Render Geometry
	{
		ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Transform, C_Render, T_Render_Geometry_Static>();
		std::vector<Archetype*> queries = _ECSSystem.QueryArchetypes(key);
		for (auto& archetype : queries)
		{
			size_t st_row = 0;
			size_t st_col = 0;
			for (size_t row = st_row; row < archetype->GetCount_Chunks(); row++)
			{
				auto& transforms = archetype->GetComponents<C_Transform>(row);
				auto& renders = archetype->GetComponents<C_Render>(row);
				for (size_t col = st_col; col < archetype->GetCount_Chunk(row); col++)
				{
					if (!renders[col].bRenderable) continue;
					const Vector3& scale = transforms[col].vScale;
					const Quarternion& rotate = transforms[col].qRotate;
					const Vector3& position = transforms[col].vPosition;
					CB_WVPITMatrix cb_wvpitmat;
					cb_wvpitmat.matWorld = GetMat_World(scale, rotate, position);
					cb_wvpitmat.matView = matView;
					cb_wvpitmat.matProj = matProj;
					cb_wvpitmat.matInvTrans = GetMat_InverseTranspose(cb_wvpitmat.matWorld);
					_EngineSystem.UpdateConstantBuffer(g_hash_cb_wvpitmat, &cb_wvpitmat);
					_EngineSystem.SetVS_ConstantBuffer(g_hash_cb_wvpitmat, 0);

					const auto& MeshMats = _ResourceSystem.GetResource<RenderAsset>(renders[col].hash_ra)->m_hMeshMats;
					for (UINT j = 0; j < MeshMats.size(); j++)
					{
						auto& iter = MeshMats[j];
						BaseMesh* pMesh = _ResourceSystem.GetResource<BaseMesh>(iter.hash_mesh);
						_EngineSystem.SetIA_VertexBuffer(pMesh->GetVB());
						_EngineSystem.SetIA_IndexBuffer(pMesh->GetIB());

						Material* pMaterial = _ResourceSystem.GetResource<Material>(iter.hash_material);
						_EngineSystem.SetIA_InputLayout(pMaterial->GetIL());
						_EngineSystem.SetVS_Shader(pMaterial->GetVS());

						Material* pShadowMapMaterial = _ResourceSystem.GetResource<Material>(_EngineSystem.m_hash_Mat_ShadowMap);
						_EngineSystem.SetPS_Shader(pShadowMapMaterial->GetPS());

						_EngineSystem.Draw_Indicies(pMesh->GetRendIndices()[j].count, pMesh->GetRendIndices()[j].idx, 0);
					}
				}
				st_col = 0;
			}
		}
	}

	//Skeletal
	{
		ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Transform, C_Render, C_Animation, T_Render_Geometry_Skeletal>();
		std::vector<Archetype*> queries = _ECSSystem.QueryArchetypes(key);
		for (auto& archetype : queries)
		{
			size_t st_row = 0;
			size_t st_col = 0;
			for (size_t row = st_row; row < archetype->GetCount_Chunks(); row++)
			{
				auto& transforms = archetype->GetComponents<C_Transform>(row);
				auto& renders = archetype->GetComponents<C_Render>(row);
				auto& animations = archetype->GetComponents<C_Animation>(row);
				for (size_t col = st_col; col < archetype->GetCount_Chunk(row); col++)
				{
					if (!renders[col].bRenderable) continue;
					const Vector3& scale = transforms[col].vScale;
					const Quarternion& rotate = transforms[col].qRotate;
					const Vector3& position = transforms[col].vPosition;

					CB_WVPITMatrix cb_wvpitmat;
					cb_wvpitmat.matWorld = GetMat_World(scale, rotate, position);
					cb_wvpitmat.matView = matView;
					cb_wvpitmat.matProj = matProj;
					cb_wvpitmat.matInvTrans = GetMat_InverseTranspose(cb_wvpitmat.matWorld);
					_EngineSystem.UpdateConstantBuffer(g_hash_cb_wvpitmat, &cb_wvpitmat);
					_EngineSystem.SetVS_ConstantBuffer(g_hash_cb_wvpitmat, 0);

					CB_BoneMatrix cb_bonemat;
					std::memcpy(cb_bonemat.bones, animations[col].matAnims, sizeof(cb_bonemat.bones));
					_EngineSystem.UpdateConstantBuffer(g_hash_cb_bonemat, &cb_bonemat);
					_EngineSystem.SetVS_ConstantBuffer(g_hash_cb_bonemat, 2);

					const auto& MeshMats = _ResourceSystem.GetResource<RenderAsset>(renders[col].hash_ra)->m_hMeshMats;
					for (UINT j = 0; j < MeshMats.size(); j++)
					{
						auto& iter = MeshMats[j];
						BaseMesh* pMesh = _ResourceSystem.GetResource<BaseMesh>(iter.hash_mesh);
						_EngineSystem.SetIA_VertexBuffer(pMesh->GetVB());
						_EngineSystem.SetIA_IndexBuffer(pMesh->GetIB());

						Material* pMaterial = _ResourceSystem.GetResource<Material>(iter.hash_material);
						_EngineSystem.SetIA_InputLayout(pMaterial->GetIL());
						_EngineSystem.SetVS_Shader(pMaterial->GetVS());

						Material* pShadowMapMaterial = _ResourceSystem.GetResource<Material>(_EngineSystem.m_hash_Mat_ShadowMap);
						_EngineSystem.SetPS_Shader(pShadowMapMaterial->GetPS());

						const std::vector<size_t>* texs = pMaterial->GetTextures();
						int cnt = 0;
						for (int idxTex = 0; idxTex < (UINT)E_Texture::count; idxTex++)
						{
							for (const auto& hashTx : texs[idxTex])
							{
								size_t hashSRV = _ResourceSystem.GetResource<Texture>(hashTx)->GetSRV();
								_EngineSystem.SetPS_ShaderResourceView(hashSRV, cnt++);
							}
						}
						_EngineSystem.Draw_Indicies(pMesh->GetRendIndices()[j].count, pMesh->GetRendIndices()[j].idx, 0);
					}
				}
				st_col = 0;
			}
		}
	}

	//Render RTV_CubeMap
#ifdef  _EnviornmentMap
	{
		ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Transform, C_Render, T_Render_CubeMap>();
		std::vector<Archetype*> queries = _ECSSystem.QueryArchetypes(key);
		for (auto& archetype : queries)
		{
			size_t st_row = 0;
			size_t st_col = 0;
			for (size_t row = st_row; row < archetype->GetCount_Chunks(); row++)
			{
				auto& transforms = archetype->GetComponents<C_Transform>(row);
				auto& renders = archetype->GetComponents<C_Render>(row);
				for (size_t col = st_col; col < archetype->GetCount_Chunk(row); col++)
				{
					if (!renders[col].bRenderable) continue;
					const Vector3& scale = transforms[col].vScale;
					const Quarternion& rotate = transforms[col].qRotate;
					const Vector3& position = transforms[col].vPosition;
					CB_WVPITMatrix cb_wvpitmat;
					cb_wvpitmat.matWorld = GetMat_World(scale, rotate, position);
					cb_wvpitmat.matView = matView;
					cb_wvpitmat.matProj = matProj;
					cb_wvpitmat.matInvTrans = GetMat_InverseTranspose(cb_wvpitmat.matWorld);
					m_pCCBs[g_hash_cb_wvpitmat]->UpdateBufferData(m_pCDirect3D->GetDeviceContext(), &cb_wvpitmat);
					SetVS_ConstantBuffer(m_pCCBs[g_hash_cb_wvpitmat]->GetBuffer(), 3);

					const auto& MeshMats = m_pCRAs[renders[col].hash_ra]->m_hMeshMats;
					for (UINT j = 0; j < MeshMats.size(); j++)
					{
						auto& iter = MeshMats[j];
						BaseMesh* pMesh = _ResourceSystem.GetResource<BaseMesh>(iter.hash_mesh);
						SetIA_VertexBuffer(m_pCVBs[pMesh->GetVB()]->GetBuffer(), m_pCVBs[pMesh->GetVB()]->GetVertexSize());
						SetIA_IndexBuffer(m_pCIBs[pMesh->GetIB()]->GetBuffer());


						Material* pMaterial = _ResourceSystem.GetResource<Material>(iter.hash_material);
						SetIA_InputLayout(m_pCILs[pMaterial->GetIL()]->GetInputLayout());
						SetVS_Shader(m_pCVSs[pMaterial->GetVS()]->GetShader());
						if (m_pCGSs.find(pMaterial->GetGS()) != m_pCGSs.end())
							SetGS_Shader(m_pCGSs[pMaterial->GetGS()]->GetShader());
						else
							SetGS_Shader(nullptr);
						Material* pShadowMapMaterial = _ResourceSystem.GetResource<Material>(m_hash_Mat_ShadowMap);
						SetPS_Shader(m_pCPSs[pShadowMapMaterial->GetPS()]->GetShader());

						Draw_Indicies(pMesh->GetRendIndices()[j].count, pMesh->GetRendIndices()[j].idx, 0);
					}
				}
				st_col = 0;
			}
		}
	}
#endif //  _EnvironmentMap
	const auto& c_cam_transform = _ECSSystem.GetComponent<C_Transform>(_CameraSystem.lookup_maincam);
	//Render Billboard
	{
		ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Transform, C_Render, T_Render_Billboard>();
		std::vector<Archetype*> queries = _ECSSystem.QueryArchetypes(key);
		for (auto& archetype : queries)
		{
			size_t st_row = 0;
			size_t st_col = 0;
			for (size_t row = st_row; row < archetype->GetCount_Chunks(); row++)
			{
				auto& transforms = archetype->GetComponents<C_Transform>(row);
				auto& renders = archetype->GetComponents<C_Render>(row);
				for (size_t col = st_col; col < archetype->GetCount_Chunk(row); col++)
				{
					if (!renders[col].bRenderable) continue;
					const Vector3& scale = transforms[col].vScale;
					const Quarternion& rotate = transforms[col].qRotate;
					const Vector3& position = transforms[col].vPosition;
					CB_WVPITMatrix cb_wvpitmat;
					/*
					float dx = c_cam_transform.vPosition.GetX() - position.GetX();
					float dz = c_cam_transform.vPosition.GetZ() - position.GetZ();
					double angle = _RADTODEG(atan2f(dx, dz)) + 180.0f;
					*/
					//yaw빌보드 구현, +z(화면안쪽) 이므로 물체-카메라로 역방향으로 회전해야 같은yaw로 회전해 빌보드로 보이게된다

					Quarternion qRot((position - c_cam_transform.vPosition).Normalize());
					cb_wvpitmat.matWorld = GetMat_Scale(scale) * GetMat_RotYaw(qRot.ToRotate().GetY()) * GetMat_Translation(position);
					cb_wvpitmat.matView = matView;
					cb_wvpitmat.matProj = matProj;
					cb_wvpitmat.matInvTrans = GetMat_InverseTranspose(cb_wvpitmat.matWorld);
					_EngineSystem.UpdateConstantBuffer(g_hash_cb_wvpitmat, &cb_wvpitmat);
					_EngineSystem.SetVS_ConstantBuffer(g_hash_cb_wvpitmat, 0);

					const auto& MeshMats = _ResourceSystem.GetResource<RenderAsset>(renders[col].hash_ra)->m_hMeshMats;
					for (UINT j = 0; j < MeshMats.size(); j++)
					{
						auto& iter = MeshMats[j];
						BaseMesh* pMesh = _ResourceSystem.GetResource<BaseMesh>(iter.hash_mesh);
						_EngineSystem.SetIA_VertexBuffer(pMesh->GetVB());
						_EngineSystem.SetIA_IndexBuffer(pMesh->GetIB());

						Material* pMaterial = _ResourceSystem.GetResource<Material>(iter.hash_material);
						_EngineSystem.SetIA_InputLayout(pMaterial->GetIL());
						_EngineSystem.SetVS_Shader(pMaterial->GetVS());

						Material* pShadowMapMaterial = _ResourceSystem.GetResource<Material>(_EngineSystem.m_hash_Mat_ShadowMap);
						_EngineSystem.SetPS_Shader(pShadowMapMaterial->GetPS());

						_EngineSystem.Draw_Indicies(pMesh->GetRendIndices()[j].count, pMesh->GetRendIndices()[j].idx, 0);
					}
				}
				st_col = 0;
			}
		}
	}

	//원복
	_EngineSystem.ClearRenderTargetView(_EngineSystem.m_hash_RTV_0, 0.0f, 0.3f, 0.4f, 1.0f);
	_EngineSystem.ClearDepthStencilView(_EngineSystem.m_hash_DSV_0);
	_EngineSystem.SetOM_RenderTargets({ _EngineSystem.m_hash_RTV_0 }, _EngineSystem.m_hash_DSV_0);
	
	_EngineSystem.SetPS_ShaderResourceView(_EngineSystem.m_hash_DSV_ShadowMap, 6);
}

void RenderSystem::RenderEnviornmentMap(const Matrix4x4& matView, const Matrix4x4& matProj)
{
	_EngineSystem.SetOM_BlendState(E_BSState::Opaque, NULL);
	_EngineSystem.SetOM_DepthStenilState(E_DSState::DEFAULT);
	_EngineSystem.SetPS_SamplerState(E_Sampler::ANISOTROPIC_WRAP);
	_EngineSystem.SetRS_RasterizerState(E_RSState::SOLID_CULLBACK_CW);
	ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Transform, C_Render, T_Render_CubeMap>();
	std::vector<Archetype*> queries = _ECSSystem.QueryArchetypes(key);
	for (auto& archetype : queries)
	{
		size_t st_row = 0;
		size_t st_col = 0;
		for (size_t row = st_row; row < archetype->GetCount_Chunks(); row++)
		{
			auto& transforms = archetype->GetComponents<C_Transform>(row);
			auto& renders = archetype->GetComponents<C_Render>(row);
			for (size_t col = st_col; col < archetype->GetCount_Chunk(row); col++)
			{
				if (!renders[col].bRenderable) continue;
				const Vector3& scale = transforms[col].vScale;
				const Quarternion& rotate = transforms[col].qRotate;
				const Vector3& position = transforms[col].vPosition;
				CB_WVPITMatrix cb_wvpitmat;
				cb_wvpitmat.matWorld = GetMat_World(scale, rotate, position);
				cb_wvpitmat.matView = matView;
				cb_wvpitmat.matProj = matProj;
				cb_wvpitmat.matInvTrans = GetMat_InverseTranspose(cb_wvpitmat.matWorld);
				_EngineSystem.UpdateConstantBuffer(g_hash_cb_wvpitmat, &cb_wvpitmat);
				_EngineSystem.SetVS_ConstantBuffer(g_hash_cb_wvpitmat, 0);

				const auto& MeshMats = _ResourceSystem.GetResource<RenderAsset>(renders[col].hash_ra)->m_hMeshMats;
				for (UINT j = 0; j < MeshMats.size(); j++)
				{
					auto& iter = MeshMats[j];
					BaseMesh* pMesh = _ResourceSystem.GetResource<BaseMesh>(iter.hash_mesh);
					_EngineSystem.SetIA_VertexBuffer(pMesh->GetVB());
					_EngineSystem.SetIA_IndexBuffer(pMesh->GetIB());

					Material* pMaterial = _ResourceSystem.GetResource<Material>(iter.hash_material);
					_EngineSystem.SetIA_InputLayout(pMaterial->GetIL());
					_EngineSystem.SetVS_Shader(pMaterial->GetVS());
					_EngineSystem.SetGS_Shader(pMaterial->GetGS());
					_EngineSystem.SetPS_Shader(pMaterial->GetPS());

					const std::vector<size_t>* texs = pMaterial->GetTextures();
					int cnt = 0;
					for (int idxTex = 0; idxTex < (UINT)E_Texture::count; idxTex++)
					{
						for (const auto& hashTx : texs[idxTex])
						{
							size_t hashSRV = _ResourceSystem.GetResource<Texture>(hashTx)->GetSRV();
							_EngineSystem.SetPS_ShaderResourceView(hashSRV, cnt++);
						}
					}
					_EngineSystem.SetPS_ShaderResourceView(_EngineSystem.m_hash_SRV_CubeMap, cnt++);
					_EngineSystem.Draw_Indicies(pMesh->GetRendIndices()[j].count, pMesh->GetRendIndices()[j].idx, 0);
				}
			}
			st_col = 0;
		}
	}
}

void RenderSystem::RenderCubeMap()
{
	//Render EnvironmentMap
	{
		_EngineSystem.SetRS_Viewport(&_EngineSystem.m_vp_CubeMap);
		for (int i = 0; i < 6; i++)
		{
			RenderCubeMapTexture(i);
		}
		//원복
		_EngineSystem.ClearRenderTargetView(_EngineSystem.m_hash_RTV_0, 0.0f, 0.3f, 0.4f, 1.0f);
		_EngineSystem.ClearDepthStencilView(_EngineSystem.m_hash_DSV_0);
		_EngineSystem.SetOM_RenderTargets({ _EngineSystem.m_hash_RTV_0 }, _EngineSystem.m_hash_DSV_0);
		_EngineSystem.SetRS_Viewport(&_EngineSystem.m_vp_BB);
		// RTV, SRV에 사용하는 버퍼의 밉맵을 형성한다(앨리어싱처리)
		_EngineSystem.GenerateMipMaps(_EngineSystem.m_hash_SRV_CubeMap);
	}
}

void RenderSystem::RenderCubeMapTexture(UINT cubemapIdx)
{
	FLOAT clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	_EngineSystem.ClearRenderTargetView(_EngineSystem.m_hash_RTV_CubeMap[cubemapIdx], 0.0f, 0.0f, 0.0f, 1.0f);
	_EngineSystem.ClearDepthStencilView(_EngineSystem.m_hash_DSV_CubeMap);
	_EngineSystem.SetOM_RenderTargets({ _EngineSystem.m_hash_RTV_CubeMap[cubemapIdx] }, _EngineSystem.m_hash_DSV_CubeMap);

	size_t lookup_cam = _CameraSystem.lookup_cubemapcam[cubemapIdx];
	const auto& c_cam_main = _ECSSystem.GetComponent<C_Camera>(lookup_cam);
	const auto& c_cam_proj = _ECSSystem.GetComponent<C_Projection>(lookup_cam);
	const Matrix4x4& cam_matWorld = c_cam_main.matWorld;
	const Matrix4x4& cam_matView = c_cam_main.matView;
	const Matrix4x4& cam_matProj = c_cam_proj.matProj;

	const auto& c_cam_transform = _ECSSystem.GetComponent<C_Transform>(lookup_cam);
	CB_Campos cb_campos;
	cb_campos.vPosition = c_cam_transform.vPosition;
	_EngineSystem.UpdateConstantBuffer(g_hash_cb_campos, &cb_campos);
	_EngineSystem.SetPS_ConstantBuffer(g_hash_cb_campos, 5);

	//Render SkySphere
	RenderSkySphere(cam_matView, cam_matProj);

	//Render Geometry
	RenderGeometry(cam_matView, cam_matProj);

	//Render Billboard
	RenderBillboard(c_cam_transform.vPosition, cam_matView, cam_matProj);
}

void RenderSystem::RenderUI(const Matrix4x4& matOrtho)
{
	_EngineSystem.SetIA_Topology(D3D_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	_EngineSystem.SetOM_BlendState(E_BSState::Opaque, NULL);
	_EngineSystem.SetOM_DepthStenilState(E_DSState::UI);
	_EngineSystem.SetPS_SamplerState(E_Sampler::LINEAR_WRAP);
	_EngineSystem.SetRS_RasterizerState(E_RSState::SOLID_CULLBACK_CW);

	//2D객체로 SRV임시체크
	_EngineSystem.SetOM_RenderTargets({}, NULL);
	_EngineSystem.ClearRenderTargetView(_EngineSystem.m_hash_RTV_BB, 0.0f, 0.0f, 0.0f, 1.0f);
	_EngineSystem.SetOM_RenderTargets({ _EngineSystem.m_hash_RTV_BB }, NULL);

	ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Transform, C_Render, T_Render_UI>();
	std::vector<Archetype*> queries = _ECSSystem.QueryArchetypes(key);
	for (auto& archetype : queries)
	{
		size_t st_row = 0;
		size_t st_col = 0;
		for (size_t row = st_row; row < archetype->GetCount_Chunks(); row++)
		{
			auto& transforms = archetype->GetComponents<C_Transform>(row);
			auto& renders = archetype->GetComponents<C_Render>(row);
			for (size_t col = st_col; col < archetype->GetCount_Chunk(row); col++)
			{
				const Vector3& scale = transforms[col].vScale;
				const Quarternion& rotate = transforms[col].qRotate;
				const Vector3& position = transforms[col].vPosition;
				CB_WVPITMatrix cb_wvpitmat;
				cb_wvpitmat.matWorld = GetMat_ConvertGeometryOrtho() * GetMat_World(scale, rotate, position);
				cb_wvpitmat.matView = GetMat_Identity();
				cb_wvpitmat.matProj = matOrtho;
				_EngineSystem.UpdateConstantBuffer(g_hash_cb_wvpitmat, &cb_wvpitmat);
				_EngineSystem.SetVS_ConstantBuffer(g_hash_cb_wvpitmat, 0);

				const auto& MeshMats = _ResourceSystem.GetResource<RenderAsset>(renders[col].hash_ra)->m_hMeshMats;
				for (UINT j = 0; j < MeshMats.size(); j++)
				{
					auto& iter = MeshMats[j];
					BaseMesh* pMesh = _ResourceSystem.GetResource<BaseMesh>(iter.hash_mesh);
					_EngineSystem.SetIA_VertexBuffer(pMesh->GetVB());
					_EngineSystem.SetIA_IndexBuffer(pMesh->GetIB());

					Material* pMaterial = _ResourceSystem.GetResource<Material>(iter.hash_material);
					_EngineSystem.SetIA_InputLayout(pMaterial->GetIL());
					_EngineSystem.SetVS_Shader(pMaterial->GetVS());
					_EngineSystem.SetPS_Shader(pMaterial->GetPS());
					_EngineSystem.SetGS_Shader(pMaterial->GetGS());

					const std::vector<size_t>* texs = pMaterial->GetTextures();
					int cnt = 0;
					for (int idxTex = 0; idxTex < (UINT)E_Texture::count; idxTex++)
					{
						for (const auto& hashTx : texs[idxTex])
						{
							size_t hashSRV = hashTx;
							_EngineSystem.SetPS_ShaderResourceView(hashSRV, cnt++);
						}
					}
					_EngineSystem.Draw_Indicies(pMesh->GetRendIndices()[j].count, pMesh->GetRendIndices()[j].idx, 0);
				}
			}
		}
		st_col = 0;
	}
}

void RenderSystem::RenderGeometry_Picking(const Matrix4x4& matView, const Matrix4x4& matProj)
{
	_EngineSystem.SetOM_BlendState(E_BSState::Opaque, NULL);
	_EngineSystem.SetOM_DepthStenilState(E_DSState::DEFAULT);
	_EngineSystem.SetPS_SamplerState(E_Sampler::LINEAR_WRAP);
	_EngineSystem.SetPS_SamplerState(E_Sampler::POINT_CLAMP_COMPARISON, 6);
	_EngineSystem.SetRS_RasterizerState(E_RSState::SOLID_CULLBACK_CW);
	//static
	{
		ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Transform, C_Render, C_Collider, T_Render_Geometry_Static>();
		std::vector<Archetype*> queries = _ECSSystem.QueryArchetypes(key);
		for (auto& archetype : queries)
		{
			size_t st_row = 0;
			size_t st_col = 0;
			for (size_t row = st_row; row < archetype->GetCount_Chunks(); row++)
			{
				auto& transforms = archetype->GetComponents<C_Transform>(row);
				auto& renders = archetype->GetComponents<C_Render>(row);
				auto& colliders = archetype->GetComponents<C_Collider>(row);
				for (size_t col = st_col; col < archetype->GetCount_Chunk(row); col++)
				{
					if (!renders[col].bRenderable) continue;
					if (!colliders[col].bPicking) continue;
					const Vector3& scale = transforms[col].vScale;
					const Quarternion& rotate = transforms[col].qRotate;
					const Vector3& position = transforms[col].vPosition;
					CB_WVPITMatrix cb_wvpitmat;
					cb_wvpitmat.matWorld = GetMat_World(scale, rotate, position);
					cb_wvpitmat.matView = matView;
					cb_wvpitmat.matProj = matProj;
					cb_wvpitmat.matInvTrans = GetMat_InverseTranspose(cb_wvpitmat.matWorld);
					_EngineSystem.UpdateConstantBuffer(g_hash_cb_wvpitmat, &cb_wvpitmat);
					_EngineSystem.SetVS_ConstantBuffer(g_hash_cb_wvpitmat, 0);

					const auto& MeshMats = _ResourceSystem.GetResource<RenderAsset>(renders[col].hash_ra)->m_hMeshMats;
					for (UINT j = 0; j < MeshMats.size(); j++)
					{
						auto& iter = MeshMats[j];
						BaseMesh* pMesh = _ResourceSystem.GetResource<BaseMesh>(iter.hash_mesh);
						_EngineSystem.SetIA_VertexBuffer(pMesh->GetVB());
						_EngineSystem.SetIA_IndexBuffer(pMesh->GetIB());

						Material* pMaterial = _ResourceSystem.GetResource<Material>(iter.hash_material);
						_EngineSystem.SetIA_InputLayout(pMaterial->GetIL());
						_EngineSystem.SetVS_Shader(pMaterial->GetVS());
						_EngineSystem.SetHS_Shader(NULL);
						_EngineSystem.SetDS_Shader(NULL);
						_EngineSystem.SetGS_Shader(NULL);
						_EngineSystem.SetPS_Shader(g_hash_PS_Picking);

						_EngineSystem.Draw_Indicies(3, colliders[col].pickingIdx, 0);
					}
				}
				st_col = 0;
			}
		}
	}

	//Skeletal
	{
		ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Transform, C_Render, C_Animation, C_Collider, T_Render_Geometry_Skeletal>();
		std::vector<Archetype*> queries = _ECSSystem.QueryArchetypes(key);
		for (auto& archetype : queries)
		{
			size_t st_row = 0;
			size_t st_col = 0;
			for (size_t row = st_row; row < archetype->GetCount_Chunks(); row++)
			{
				auto& transforms = archetype->GetComponents<C_Transform>(row);
				auto& renders = archetype->GetComponents<C_Render>(row);
				auto& animations = archetype->GetComponents<C_Animation>(row);
				auto& colliders = archetype->GetComponents<C_Collider>(row);
				for (size_t col = st_col; col < archetype->GetCount_Chunk(row); col++)
				{
					if (!renders[col].bRenderable) continue;
					if (!colliders[col].bPicking) continue;
					const Vector3& scale = transforms[col].vScale;
					const Quarternion& rotate = transforms[col].qRotate;
					const Vector3& position = transforms[col].vPosition;

					CB_WVPITMatrix cb_wvpitmat;
					cb_wvpitmat.matWorld = GetMat_World(scale, rotate, position);
					cb_wvpitmat.matView = matView;
					cb_wvpitmat.matProj = matProj;
					cb_wvpitmat.matInvTrans = GetMat_InverseTranspose(cb_wvpitmat.matWorld);
					_EngineSystem.UpdateConstantBuffer(g_hash_cb_wvpitmat, &cb_wvpitmat);
					_EngineSystem.SetVS_ConstantBuffer(g_hash_cb_wvpitmat, 0);

					CB_BoneMatrix cb_bonemat;
					std::memcpy(cb_bonemat.bones, animations[col].matAnims, sizeof(cb_bonemat.bones));
					_EngineSystem.UpdateConstantBuffer(g_hash_cb_bonemat, &cb_bonemat);
					_EngineSystem.SetVS_ConstantBuffer(g_hash_cb_bonemat, 2);

					const auto& MeshMats = _ResourceSystem.GetResource<RenderAsset>(renders[col].hash_ra)->m_hMeshMats;
					for (UINT j = 0; j < MeshMats.size(); j++)
					{
						auto& iter = MeshMats[j];
						BaseMesh* pMesh = _ResourceSystem.GetResource<BaseMesh>(iter.hash_mesh);
						_EngineSystem.SetIA_VertexBuffer(pMesh->GetVB());
						_EngineSystem.SetIA_IndexBuffer(pMesh->GetIB());

						Material* pMaterial = _ResourceSystem.GetResource<Material>(iter.hash_material);
						_EngineSystem.SetIA_InputLayout(pMaterial->GetIL());
						_EngineSystem.SetVS_Shader(pMaterial->GetVS());
						_EngineSystem.SetHS_Shader(NULL);
						_EngineSystem.SetDS_Shader(NULL);
						_EngineSystem.SetGS_Shader(NULL);
						_EngineSystem.SetPS_Shader(g_hash_PS_Picking);

						_EngineSystem.Draw_Indicies(3, colliders[col].pickingIdx, 0);
					}
				}
				st_col = 0;
			}
		}
	}
}

void RenderSystem::RenderGeometry_Debug(const Matrix4x4& matView, const Matrix4x4& matProj)
{
	_EngineSystem.SetOM_BlendState(E_BSState::Opaque, NULL);
	_EngineSystem.SetOM_DepthStenilState(E_DSState::DEFAULT);
	_EngineSystem.SetPS_SamplerState(E_Sampler::LINEAR_WRAP);
	_EngineSystem.SetRS_RasterizerState(E_RSState::WIRE_CULLBACK_CW);
	//DebugRender는 PS를 통일
	_EngineSystem.SetPS_Shader(g_hash_PS_Debug_PC);
	//Static
	{
		ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Transform, C_Render, C_Collider, T_Render_Geometry_Static>();
		std::vector<Archetype*> queries = _ECSSystem.QueryArchetypes(key);
		for (auto& archetype : queries)
		{
			size_t st_row = 0;
			size_t st_col = 0;
			for (size_t row = st_row; row < archetype->GetCount_Chunks(); row++)
			{
				auto& transforms = archetype->GetComponents<C_Transform>(row);
				auto& renders = archetype->GetComponents<C_Render>(row);
				auto& colliders = archetype->GetComponents<C_Collider>(row);

				for (size_t col = st_col; col < archetype->GetCount_Chunk(row); col++)
				{
					if (!renders[col].bRenderable) continue;
					const Vector3& scale = transforms[col].vScale;
					const Quarternion& rotate = transforms[col].qRotate;
					const Vector3& position = transforms[col].vPosition;
					CB_WVPITMatrix cb_wvpitmat;
					cb_wvpitmat.matWorld = GetMat_World(scale, rotate, position);
					cb_wvpitmat.matView = matView;
					cb_wvpitmat.matProj = matProj;
					cb_wvpitmat.matInvTrans = GetMat_InverseTranspose(cb_wvpitmat.matWorld);
					_EngineSystem.UpdateConstantBuffer(g_hash_cb_wvpitmat, &cb_wvpitmat);

					_EngineSystem.SetIA_VertexBuffer(NULL);
					_EngineSystem.SetIA_IndexBuffer(NULL);
					_EngineSystem.SetIA_InputLayout(NULL);

					if (colliders[col].type == E_Collider::AABB)
					{
						_EngineSystem.SetIA_Topology(D3D_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
						_EngineSystem.SetVS_Shader(g_hash_VS_Debug);
						_EngineSystem.SetHS_Shader(NULL);
						_EngineSystem.SetDS_Shader(NULL);
						_EngineSystem.SetGS_Shader(g_hash_GS_Debug_Box);
						_EngineSystem.SetGS_ConstantBuffer(g_hash_cb_wvpitmat, 0);
					}
					else if (colliders[col].type == E_Collider::SPHERE)
					{
						_EngineSystem.SetIA_Topology(D3D_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
						_EngineSystem.SetVS_Shader(g_hash_VS_Debug_Sphere);
						_EngineSystem.SetHS_Shader(g_hash_HS_Debug_Sphere);
						_EngineSystem.SetDS_Shader(g_hash_DS_Debug_Sphere);
						_EngineSystem.SetGS_Shader(NULL);
						_EngineSystem.SetDS_ConstantBuffer(g_hash_cb_wvpitmat, 0);
					}

					const auto& MeshMats = _ResourceSystem.GetResource<RenderAsset>(renders[col].hash_ra)->m_hMeshMats;
					for (UINT j = 0; j < MeshMats.size(); j++)
					{
						auto& iter = MeshMats[j];
						BaseMesh* pMesh = _ResourceSystem.GetResource<BaseMesh>(iter.hash_mesh);

						for (const auto& hash_collider : pMesh->GetCLs())
						{
							UINT drawCount = 0;
							if (colliders[col].type == E_Collider::AABB)
							{
								CB_Debug_Box cb_debug_box;
								_CollisionSystem.SetColliderDebugData(hash_collider, cb_debug_box);
								_EngineSystem.UpdateConstantBuffer(g_hash_cb_debug_box, &cb_debug_box);
								_EngineSystem.SetGS_ConstantBuffer(g_hash_cb_debug_box, 1);
								drawCount = 1;
							}
							else if (colliders[col].type == E_Collider::SPHERE)
							{
								CB_Debug_Sphere cb_debug_sphere;
								_CollisionSystem.SetColliderDebugData(hash_collider, cb_debug_sphere);
								_EngineSystem.UpdateConstantBuffer(g_hash_cb_debug_sphere, &cb_debug_sphere);
								_EngineSystem.SetHS_ConstantBuffer(g_hash_cb_debug_sphere, 1);
								_EngineSystem.SetDS_ConstantBuffer(g_hash_cb_debug_sphere, 1);
								drawCount = 60;
							}

							_EngineSystem.Draw_Vertices(drawCount, 0);
						}
					}
				}
				st_col = 0;
			}
		}
	}

	//Skeletal
	{
		ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Transform, C_Render, C_Animation, C_Collider, T_Render_Geometry_Skeletal>();
		std::vector<Archetype*> queries = _ECSSystem.QueryArchetypes(key);
		for (auto& archetype : queries)
		{
			size_t st_row = 0;
			size_t st_col = 0;
			for (size_t row = st_row; row < archetype->GetCount_Chunks(); row++)
			{
				auto& transforms = archetype->GetComponents<C_Transform>(row);
				auto& renders = archetype->GetComponents<C_Render>(row);
				auto& animations = archetype->GetComponents<C_Animation>(row);
				auto& colliders = archetype->GetComponents<C_Collider>(row);

				for (size_t col = st_col; col < archetype->GetCount_Chunk(row); col++)
				{
					if (!renders[col].bRenderable) continue;

					const Vector3& scale = transforms[col].vScale;
					const Quarternion& rotate = transforms[col].qRotate;
					const Vector3& position = transforms[col].vPosition;
					CB_WVPITMatrix cb_wvpitmat;
					cb_wvpitmat.matView = matView;
					cb_wvpitmat.matProj = matProj;
					cb_wvpitmat.matInvTrans = GetMat_InverseTranspose(cb_wvpitmat.matWorld);

					_EngineSystem.SetIA_VertexBuffer(NULL, 0);
					_EngineSystem.SetIA_IndexBuffer(NULL);
					_EngineSystem.SetIA_InputLayout(NULL);

					if (colliders[col].type == E_Collider::AABB)
					{
						_EngineSystem.SetIA_Topology(D3D_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
						_EngineSystem.SetVS_Shader(g_hash_VS_Debug);
						_EngineSystem.SetHS_Shader(NULL);
						_EngineSystem.SetDS_Shader(NULL);
						_EngineSystem.SetGS_Shader(g_hash_GS_Debug_Box);
					}
					else if (colliders[col].type == E_Collider::SPHERE)
					{
						_EngineSystem.SetIA_Topology(D3D_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
						_EngineSystem.SetVS_Shader(g_hash_VS_Debug_Sphere);
						_EngineSystem.SetHS_Shader(g_hash_HS_Debug_Sphere);
						_EngineSystem.SetDS_Shader(g_hash_DS_Debug_Sphere);
						_EngineSystem.SetGS_Shader(NULL);
					}

					const auto& MeshMats = _ResourceSystem.GetResource<RenderAsset>(renders[col].hash_ra)->m_hMeshMats;
					for (UINT j = 0; j < MeshMats.size(); j++)
					{
						auto& iter = MeshMats[j];
						BaseMesh* pMesh = _ResourceSystem.GetResource<BaseMesh>(iter.hash_mesh);
						for (int idx = 0; idx < pMesh->GetCLs().size(); idx++)
						{
							cb_wvpitmat.matWorld = animations[col].matAnims[idx] * GetMat_World(scale, rotate, position);
							_EngineSystem.UpdateConstantBuffer(g_hash_cb_wvpitmat, &cb_wvpitmat);

							size_t hash_collider = pMesh->GetCLs()[idx];

							UINT drawCount = 0;
							if (colliders[col].type == E_Collider::AABB)
							{
								_EngineSystem.SetGS_ConstantBuffer(g_hash_cb_wvpitmat, 0);
							
								CB_Debug_Box cb_debug_box;
								_CollisionSystem.SetColliderDebugData(hash_collider, cb_debug_box);
								_EngineSystem.UpdateConstantBuffer(g_hash_cb_debug_box, &cb_debug_box);
								_EngineSystem.SetGS_ConstantBuffer(g_hash_cb_debug_box, 1);
								drawCount = 1;
							}
							else if (colliders[col].type == E_Collider::SPHERE)
							{
								_EngineSystem.SetDS_ConstantBuffer(g_hash_cb_wvpitmat, 0);

								CB_Debug_Sphere cb_debug_sphere;
								_CollisionSystem.SetColliderDebugData(hash_collider, cb_debug_sphere);
								_EngineSystem.UpdateConstantBuffer(g_hash_cb_debug_sphere, &cb_debug_sphere);
								_EngineSystem.SetHS_ConstantBuffer(g_hash_cb_debug_sphere, 1);		 
								_EngineSystem.SetDS_ConstantBuffer(g_hash_cb_debug_sphere, 1);
								drawCount = 60;
							}

							_EngineSystem.Draw_Vertices(drawCount, 0);
						}
					}
				}
				st_col = 0;
			}
		}
	}

	_EngineSystem.SetVS_Shader(NULL);
	_EngineSystem.SetHS_Shader(NULL);
	_EngineSystem.SetDS_Shader(NULL);
	_EngineSystem.SetGS_Shader(NULL);
	_EngineSystem.SetPS_Shader(NULL);
}