#include "RenderSystem.h"
#include "Direct3D.h"
#include "SwapChain.h"
#include "InputLayout.h"
#include "CameraSystem.h"
#include "ResourceSystem.h"
#include "Texture.h"
#include "Mesh.h"
#include "LightSystem.h"
#include "Light.h"
#include "CollisionSystem.h"
#include "Material.h"
#include "Buffers.h"
#include "Shaders.h"
#include "States.h"
#include "Views.h"
#include "ECSSystem.h"
#include "Assets.h"

#include "TestBlockMacro.h"

size_t g_hash_cbdirectionalLight = typeid(CB_DirectionalLight).hash_code();
size_t g_hash_cbpointlight = typeid(CB_PointLight).hash_code();
size_t g_hash_cbspotlight = typeid(CB_SpotLight).hash_code();
size_t g_hash_cbwvpmat = typeid(CB_WVPMatrix).hash_code();
size_t g_hash_cbwvpitmat = typeid(CB_WVPITMatrix).hash_code();
size_t g_hash_cbtime = typeid(CB_Time).hash_code();
size_t g_hash_cbcampos = typeid(CB_Campos).hash_code();
size_t g_hash_cblightmat = typeid(CB_LightMatrix).hash_code();

RenderSystem::RenderSystem()
{
}

void RenderSystem::Init(HWND hWnd, UINT width, UINT height)
{
	//COM객체 사용을 위한 초기화, 텍스쳐사용에필요
	_ASEERTION_CREATE(CoInitialize(nullptr), "Coninitialize not successfully");
	
	m_iWidth = width;
	m_iHeight = height;
	m_pCDirect3D = new Direct3D();
	_ASEERTION_NULCHK(m_pCDirect3D, "D3D is nullptr");

	m_pCSwapChain = new SwapChain(m_pCDirect3D->GetDevice(), hWnd, width, height);
	_ASEERTION_NULCHK(m_pCSwapChain, "SC is nullptr");
	
	m_pCSamplers = new SamplerState(m_pCDirect3D->GetDevice());
	_ASEERTION_NULCHK(m_pCSamplers, "Samplers is nullptr");

	m_pCRasterizers = new RasterizerState(m_pCDirect3D->GetDevice());
	_ASEERTION_NULCHK(m_pCRasterizers, "Rasterizer is nullptr");

	m_pCDepthStencils = new DepthStencilState(m_pCDirect3D->GetDevice());
	_ASEERTION_NULCHK(m_pCDepthStencils, "DepthStencil is nullptr");

	m_pCBlends = new BlendState(m_pCDirect3D->GetDevice());
	_ASEERTION_NULCHK(m_pCDepthStencils, "DepthStencil is nullptr");

	//상수버퍼 기본세팅
	_RenderSystem.CreateConstantBuffer(typeid(CB_DirectionalLight), sizeof(CB_DirectionalLight));
	_RenderSystem.CreateConstantBuffer(typeid(CB_PointLight), sizeof(CB_PointLight));
	_RenderSystem.CreateConstantBuffer(typeid(CB_SpotLight), sizeof(CB_SpotLight));
	_RenderSystem.CreateConstantBuffer(typeid(CB_WVPMatrix), sizeof(CB_WVPMatrix));
	_RenderSystem.CreateConstantBuffer(typeid(CB_WVPITMatrix), sizeof(CB_WVPITMatrix));
	_RenderSystem.CreateConstantBuffer(typeid(CB_Time), sizeof(CB_Time));
	_RenderSystem.CreateConstantBuffer(typeid(CB_Campos), sizeof(CB_Campos));
	_RenderSystem.CreateConstantBuffer(typeid(CB_Fog), sizeof(CB_Fog));
	_RenderSystem.CreateConstantBuffer(typeid(CB_LightMatrix), sizeof(CB_LightMatrix));
	m_vp_CubeMap.MinDepth = m_vp_BB.MinDepth = 0.0f;
	m_vp_CubeMap.MaxDepth = m_vp_BB.MaxDepth = 1.0f;
	OnResize(m_iWidth, m_iHeight);
}

void RenderSystem::Frame(float deltatime, float elapsedtime)
{
	std::cout << "Frame : " << "RenderSystem" << " Class" << '\n';
	std::cout << "deltatime : " << deltatime << '\n';
#ifdef _ECS
	Vector3& dir = static_cast<DirectionalLight*>(_LightSystem.GetLight(0))->m_vDirection;
	dir = dir * GetMat_RotYaw(deltatime * 30.0f);

	Vector3& pos = static_cast<PointLight*>(_LightSystem.GetLight(1))->m_vPosition;
	pos = pos * GetMat_RotYaw(deltatime * 50.0f);

	SkyObj->m_vPosition = _CameraSystem.GetCamera(0)->GetPosition();

	for (const auto& iter : objs)
	{
		iter->Frame(deltatime);
	}
#endif // _ECS
}

void RenderSystem::PreRender(float deltatime, float elapsedtime)
{
	std::cout << "PreRender : " << "RenderSystem" << " Class" << '\n';
	//RTV초기화
	ClearRenderViews(0, 0.3f, 0.4f, 1);
	SetRS_Viewport(&m_vp_BB);
}

void RenderSystem::Render(float deltatime, float elapsedtime)
{
	std::cout << "Render : " << "RenderSystem" << " Class" << '\n';
	SetIA_Topology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	CB_DirectionalLight cb_directional;
	{
		size_t lookup = _LightSystem.lookup_directional;
		const auto& lightDir = _ECSSystem.GetComponent<C_Light_Direction>(lookup);
		const auto& c_light = _ECSSystem.GetComponent<C_Light>(lookup);
		cb_directional.mAmbient = c_light.vAmbient;
		cb_directional.mDiffuse = c_light.vDiffuse;
		cb_directional.mSpecular = c_light.vSpecular;
		cb_directional.vDirection = Vector4(Quarternion(lightDir.dir).GetForwardAxis(), c_light.fShiness);
	}
	m_pCCBs[g_hash_cbdirectionalLight]->UpdateBufferData(m_pCDirect3D->GetDeviceContext(), &cb_directional);
	SetPS_ConstantBuffer(m_pCCBs[g_hash_cbdirectionalLight]->GetBuffer(), 0);

	CB_PointLight cb_point;
	{
		size_t lookup = _LightSystem.lookup_point;
		const auto& c_transform = _ECSSystem.GetComponent<C_Transform>(lookup);
		const auto& c_light = _ECSSystem.GetComponent<C_Light>(lookup);
		const auto& c_attenuation = _ECSSystem.GetComponent<C_Light_Attenuation>(lookup);
		cb_point.mAmbient = c_light.vAmbient;
		cb_point.mDiffuse = c_light.vDiffuse;
		cb_point.mSpecular = c_light.vSpecular;
		cb_point.vPosition = Vector4(c_transform.vPosition, c_light.fShiness);
		cb_point.fAttenuations = Vector4(c_attenuation.fAtt_a0, c_attenuation.fAtt_a1, c_attenuation.fAtt_a2, c_attenuation.fRange);
	}
	m_pCCBs[g_hash_cbpointlight]->UpdateBufferData(m_pCDirect3D->GetDeviceContext(), &cb_point);
	SetPS_ConstantBuffer(m_pCCBs[g_hash_cbpointlight]->GetBuffer(), 1);

	CB_SpotLight cb_spot;
	{
		size_t lookup = _LightSystem.lookup_spot;
		const auto& c_transform = _ECSSystem.GetComponent<C_Transform>(lookup);
		const auto& c_light = _ECSSystem.GetComponent<C_Light>(lookup);
		const auto& c_attenuation = _ECSSystem.GetComponent<C_Light_Attenuation>(lookup);
		const auto& c_spot = _ECSSystem.GetComponent<C_Light_Spot>(lookup);
		cb_spot.mAmbient = c_light.vAmbient;
		cb_spot.mDiffuse = c_light.vDiffuse;
		cb_spot.mSpecular = c_light.vSpecular;
		cb_spot.vDirection = c_transform.qRotate.GetForwardAxis();
		cb_spot.vPosition = Vector4(c_transform.vPosition, c_light.fShiness);
		cb_spot.fAttenuations = Vector4(c_attenuation.fAtt_a0, c_attenuation.fAtt_a1, c_attenuation.fAtt_a2, c_attenuation.fRange);
		cb_spot.fSpots = Vector4(c_spot.fSpot, c_spot.fCos_OuterCone, c_spot.fCos_InnerCone, 0.0f);
	}
	m_pCCBs[g_hash_cbspotlight]->UpdateBufferData(m_pCDirect3D->GetDeviceContext(), &cb_spot);
	SetPS_ConstantBuffer(m_pCCBs[g_hash_cbspotlight]->GetBuffer(), 2);

	CB_LightMatrix cb_lightMat;
	{
		size_t lookup = _LightSystem.lookup_directional;
		const auto& lightDir = _ECSSystem.GetComponent<C_Light_Direction>(lookup);
		const auto& c_cam = _ECSSystem.GetComponent<C_Camera>(_CameraSystem.lookup_maincam);
		Vector4 pos = -lightDir.dir * 5000.0f;
		cb_lightMat.matLightView = GetMat_View(pos.ToVector3(), lightDir.dir);

		cb_lightMat.matLightProj = GetMat_Orthographic(c_cam.fScreenWidth, c_cam.fScreenHeight, c_cam.fNear, 10000.0f);
		cb_lightMat.vPos = pos;
	}
	m_pCCBs[g_hash_cblightmat]->UpdateBufferData(m_pCDirect3D->GetDeviceContext(), &cb_lightMat);
	SetVS_ConstantBuffer(m_pCCBs[g_hash_cblightmat]->GetBuffer(), 1);
	//ShadowMap
	{
		RenderShadowMap(cb_lightMat.matLightView, cb_lightMat.matLightProj);
	}

#ifdef _EnviornmentMap
	RenderCubeMap();
#endif // _EnviornmentMap

	size_t lookup_maincam = _CameraSystem.lookup_maincam;
	const auto& c_cam_main = _ECSSystem.GetComponent<C_Camera>(lookup_maincam);
	const auto& c_cam_proj = _ECSSystem.GetComponent<C_Projection>(lookup_maincam);
	const auto& c_cam_ortho = _ECSSystem.GetComponent<C_Orthographic>(lookup_maincam);
	const Matrix4x4& cam_matWorld = c_cam_main.matWorld;
	const Matrix4x4& cam_matView = c_cam_main.matView;
	const Matrix4x4& cam_matProj = c_cam_proj.matProj;
	const Matrix4x4& cam_matOrhto = c_cam_ortho.matOrtho;

	const auto& c_cam_transform = _ECSSystem.GetComponent<C_Transform>(lookup_maincam);
	CB_Campos cb_campos;
	cb_campos.vPosition = c_cam_transform.vPosition;
	m_pCCBs[g_hash_cbcampos]->UpdateBufferData(m_pCDirect3D->GetDeviceContext(), &cb_campos);
	SetPS_ConstantBuffer(m_pCCBs[g_hash_cbcampos]->GetBuffer(), 5);

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

	//Render UI
	RenderUI(cam_matOrhto);
	
#ifdef _ECS
	//gizmo
	std::function<void(const Matrix4x4& matWorld)> drawgizmo = [&](const Matrix4x4& matWorld)->void
		{
			SetIA_Topology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
			SetOM_BlendState(m_pCBlends->GetState(E_BSState::Transparent), NULL);
			TempObj* obj = Gizmo;
			CB_WVPMatrix cc0;
			cc0.matWorld = matWorld;
			cc0.matView = matView;
			cc0.matProj = matProj;
			m_pCCBs[g_hash_cbwvpmat]->UpdateBufferData(m_pCDirect3D->GetDeviceContext(), &cc0);
			SetVS_ConstantBuffer(m_pCCBs[g_hash_cbwvpmat]->GetBuffer(), 3);

			for (UINT j = 0; j < obj->m_Mesh_Material.size(); j++)
			{
				auto& iter = obj->m_Mesh_Material[j];
				Mesh<Vertex_PC>* pMesh = _ResourceSystem.GetResource<Mesh<Vertex_PC>>(iter.hash_mesh);
				SetIA_VertexBuffer(m_pCVBs[pMesh->GetVB()]->GetBuffer(), m_pCVBs[pMesh->GetVB()]->GetVertexSize());
				SetIA_IndexBuffer(m_pCIBs[pMesh->GetIB()]->GetBuffer());

				Material* pMaterial = _ResourceSystem.GetResource<Material>(iter.hash_material);
				SetIA_InputLayout(m_pCILs[pMaterial->GetIL()]->GetInputLayout());
				SetVS(m_pCVSs[pMaterial->GetVS()]->GetShader());
				SetPS(m_pCPSs[pMaterial->GetPS()]->GetShader());

				Draw_Indices(pMesh->GetRendIndices()[j].count, pMesh->GetRendIndices()[j].idx, 0);
			}
			SetIA_Topology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			SetOM_BlendState(m_pCBlends->GetState(E_BSState::Opaque), NULL);
		};

#ifdef _REFLECT
#include "Plane.h"
		{
			SetRS_RasterizerState(m_pCRasterizers->GetState(E_RSState::SOLID_CULLBACK_CCW));
			SetOM_BlendState(m_pCBlends->GetState(E_BSState::Opaque), NULL);
			Plane plane(Vector3(0.0f, 1.0f, 0.0f), ReflectPlane->m_vPosition);
			for (const auto& obj : objs)
			{
				//if (!obj->bRenderable) continue;
				//상수버퍼에 cc0(wvp mat), cc1(시간) 을 세팅한다
				Matrix4x4 mat_scale_obj = GetMat_Scale(obj->m_vScale);
				Matrix4x4 mat_rotate_obj = GetMat_RotateRollPitchYaw(obj->m_vRotate);
				Matrix4x4 mat_translation_obj = GetMat_Translation(obj->m_vPosition);
				Matrix4x4 mat_reflect = GetMat_Reflect(plane.ToVector4());
				CB_WVPITMatrix cc0;
				cc0.matWorld = mat_scale_obj * mat_rotate_obj * mat_translation_obj;
				cc0.matView = mat_reflect * matView;
				cc0.matProj = matProj;
				cc0.matInvTrans = GetMat_InverseTranspose(cc0.matWorld);
				m_pCCBs[g_hash_cbwvpitmat]->UpdateBufferData(m_pCDirect3D->GetDeviceContext(), &cc0);
				SetVS_ConstantBuffer(m_pCCBs[g_hash_cbwvpitmat]->GetBuffer(), 3);

				for (UINT j = 0; j < obj->m_Mesh_Material.size(); j++)
				{
					auto& iter = obj->m_Mesh_Material[j];
					Mesh<Vertex_PTN>* pMesh = _ResourceSystem.GetResource<Mesh<Vertex_PTN>>(iter.hash_mesh);
					SetIA_VertexBuffer(m_pCVBs[pMesh->GetVB()]->GetBuffer(), m_pCVBs[pMesh->GetVB()]->GetVertexSize());
					SetIA_IndexBuffer(m_pCIBs[pMesh->GetIB()]->GetBuffer());

					Material* pMaterial = _ResourceSystem.GetResource<Material>(iter.hash_material);
					SetIA_InputLayout(m_pCILs[pMaterial->GetIL()]->GetInputLayout());
					SetVS(m_pCVSs[pMaterial->GetVS()]->GetShader());
					SetPS(m_pCPSs[pMaterial->GetPS()]->GetShader());

					const std::vector<size_t>* texs = pMaterial->GetTextures();
					int cnt = 0;
					for (int idxTex = 0; idxTex < (UINT)E_Texture::count; idxTex++)
					{
						for (const auto& hashTx : texs[idxTex])
						{
							auto pSrv = m_pCSRVs[_ResourceSystem.GetResource<Texture>(hashTx)->GetSRV()]->GetView();
							SetPS_ShaderResourceView(pSrv, cnt++);
						}
					}
					Draw_Indices(pMesh->GetRendIndices()[j].count, pMesh->GetRendIndices()[j].idx, 0);
				}
				//drawgizmo(GetMat_Scale(obj->m_vScale * 3.0f) * mat_rotate_obj * mat_translation_obj);
			}


			//reflect plane
			{
				SetRS_RasterizerState(m_pCRasterizers->GetState(E_RSState::SOLID_CULLBACK_CW));
				SetOM_BlendState(m_pCBlends->GetState(E_BSState::Transparent), NULL);
				TempObj* obj = ReflectPlane;
				ReflectPlane->bRenderable = true;
				if (obj->bRenderable)
				{
					//상수버퍼에 cc0(wvp mat), cc1(시간) 을 세팅한다
					Matrix4x4 mat_scale_obj = GetMat_Scale(obj->m_vScale);
					Matrix4x4 mat_rotate_obj = GetMat_RotateRollPitchYaw(obj->m_vRotate);
					Matrix4x4 mat_translation_obj = GetMat_Translation(obj->m_vPosition);
					CB_WVPITMatrix cc0;
					cc0.matWorld = mat_scale_obj * mat_rotate_obj * mat_translation_obj;
					cc0.matView = matView;
					cc0.matProj = matProj;
					cc0.matInvTrans = GetMat_InverseTranspose(cc0.matWorld);
					m_pCCBs[g_hash_cbwvpitmat]->UpdateBufferData(m_pCDirect3D->GetDeviceContext(), &cc0);
					SetVS_ConstantBuffer(m_pCCBs[g_hash_cbwvpitmat]->GetBuffer(), 3);

					for (UINT j = 0; j < obj->m_Mesh_Material.size(); j++)
					{
						auto& iter = obj->m_Mesh_Material[j];
						Mesh<Vertex_PTN>* pMesh = _ResourceSystem.GetResource<Mesh<Vertex_PTN>>(iter.hash_mesh);
						SetIA_VertexBuffer(m_pCVBs[pMesh->GetVB()]->GetBuffer(), m_pCVBs[pMesh->GetVB()]->GetVertexSize());
						SetIA_IndexBuffer(m_pCIBs[pMesh->GetIB()]->GetBuffer());

						Material* pMaterial = _ResourceSystem.GetResource<Material>(iter.hash_material);
						SetIA_InputLayout(m_pCILs[pMaterial->GetIL()]->GetInputLayout());
						SetVS(m_pCVSs[pMaterial->GetVS()]->GetShader());
						SetPS(m_pCPSs[pMaterial->GetPS()]->GetShader());

						const std::vector<size_t>* texs = pMaterial->GetTextures();
						int cnt = 0;
						for (int idxTex = 0; idxTex < (UINT)E_Texture::count; idxTex++)
						{
							for (const auto& hashTx : texs[idxTex])
							{
								auto pSrv = m_pCSRVs[_ResourceSystem.GetResource<Texture>(hashTx)->GetSRV()]->GetView();
								SetPS_ShaderResourceView(pSrv, cnt++);
							}
						}
						Draw_Indices(pMesh->GetRendIndices()[j].count, pMesh->GetRendIndices()[j].idx, 0);
					}
					//drawgizmo(GetMat_Scale(obj->m_vScale * 3.0f) * mat_rotate_obj * mat_translation_obj);
				}
				SetOM_BlendState(m_pCBlends->GetState(E_BSState::Opaque), NULL);
			}

		}
#endif // _REFLECT

#ifndef _TESTBLOCK
		for (const auto& obj : objs)
		{
			if (!obj->bRenderable) continue;
			//상수버퍼에 cc0(wvp mat), cc1(시간) 을 세팅한다
			Matrix4x4 mat_scale_obj = GetMat_Scale(obj->m_vScale);
			Matrix4x4 mat_rotate_obj = GetMat_RotateRollPitchYaw(obj->m_vRotate);
			Matrix4x4 mat_translation_obj = GetMat_Translation(obj->m_vPosition);
			CB_WVPITMatrix cc0;
			cc0.matWorld = mat_scale_obj * mat_rotate_obj * mat_translation_obj;
			cc0.matView = matView;
			cc0.matProj = matProj;
			cc0.matInvTrans = GetMat_InverseTranspose(cc0.matWorld);
			m_pCCBs[g_hash_cbwvpitmat]->UpdateBufferData(m_pCDirect3D->GetDeviceContext(), &cc0);
			SetVS_ConstantBuffer(m_pCCBs[g_hash_cbwvpitmat]->GetBuffer(), 3);

			for (UINT j = 0; j < obj->m_Mesh_Material.size(); j++)
			{
				auto& iter = obj->m_Mesh_Material[j];
				Mesh<Vertex_PTN>* pMesh = _ResourceSystem.GetResource<Mesh<Vertex_PTN>>(iter.hash_mesh);
				SetIA_VertexBuffer(m_pCVBs[pMesh->GetVB()]->GetBuffer(), m_pCVBs[pMesh->GetVB()]->GetVertexSize());
				SetIA_IndexBuffer(m_pCIBs[pMesh->GetIB()]->GetBuffer());

				Material* pMaterial = _ResourceSystem.GetResource<Material>(iter.hash_material);
				SetIA_InputLayout(m_pCILs[pMaterial->GetIL()]->GetInputLayout());
				SetVS(m_pCVSs[pMaterial->GetVS()]->GetShader());
				SetPS(m_pCPSs[pMaterial->GetPS()]->GetShader());

				const std::vector<size_t>* texs = pMaterial->GetTextures();
				int cnt = 0;
				for (int idxTex = 0; idxTex < (UINT)E_Texture::count; idxTex++)
				{
					for (const auto& hashTx : texs[idxTex])
					{
						auto pSrv = m_pCSRVs[_ResourceSystem.GetResource<Texture>(hashTx)->GetSRV()]->GetView();
						SetPS_ShaderResourceView(pSrv, cnt++);
					}
				}
				Draw_Indices(pMesh->GetRendIndices()[j].count, pMesh->GetRendIndices()[j].idx, 0);
			}
			drawgizmo(GetMat_Scale(obj->m_vScale * 3.0f) * mat_rotate_obj * mat_translation_obj);
		}
#endif 

#ifdef _TESTBLOCK
		{
#ifdef _SPACESHIP
			{
				auto& obj = SpaceShip;
				if (!obj->bRenderable)
				{
					//상수버퍼에 cc0(wvp mat), cc1(시간) 을 세팅한다
					CB_WVPITMatrix cc0;
					obj->m_vPosition = matWorld[3].ToVector3() + GetAxesUpFromWorld(matWorld) * -100.0f + GetAxesForwardFromWorld(matWorld) * 500.0f;
					Matrix4x4 mat_scale_obj = GetMat_Scale(obj->m_vScale);
					Matrix4x4 mat_rotate_obj = GetMat_RotateRollPitchYaw(obj->m_vRotate) * GetMat_RotFromMatrix(matWorld);
					Matrix4x4 mat_translation_obj = GetMat_Translation(obj->m_vPosition);
					cc0.matWorld = mat_scale_obj * mat_rotate_obj * mat_translation_obj;
					cc0.matView = matView;
					cc0.matProj = matProj;
					cc0.matInvTrans = GetMat_InverseTranspose(cc0.matWorld);
					m_pCCBs[g_hash_cbwvpitmat]->UpdateBufferData(m_pCDirect3D->GetDeviceContext(), &cc0);
					SetVS_ConstantBuffer(m_pCCBs[g_hash_cbwvpitmat]->GetBuffer(), 3);

					for (UINT j = 0; j < obj->m_Mesh_Material.size(); j++)
					{
						auto& iter = obj->m_Mesh_Material[j];
						//지정핸들링필요
						Mesh<Vertex_PTN>* pMesh = _ResourceSystem.GetResource<Mesh<Vertex_PTN>>(iter.hash_mesh);
						SetIA_VertexBuffer(m_pCVBs[pMesh->GetVB()]->GetBuffer(), m_pCVBs[pMesh->GetVB()]->GetVertexSize());
						SetIA_IndexBuffer(m_pCIBs[pMesh->GetIB()]->GetBuffer());

						Material* pMaterial = _ResourceSystem.GetResource<Material>(iter.hash_material);
						SetIA_InputLayout(m_pCILs[pMaterial->GetIL()]->GetInputLayout());
						SetVS(m_pCVSs[pMaterial->GetVS()]->GetShader());
						SetPS(m_pCPSs[pMaterial->GetPS()]->GetShader());

						const std::vector<size_t>* texs = pMaterial->GetTextures();
						int cnt = 0;
						for (int idxTex = 0; idxTex < (UINT)E_Texture::count; idxTex++)
						{
							for (const auto& hashTX : texs[idxTex])
							{
								auto pSrv = m_pCSRVs[_ResourceSystem.GetResource<Texture>(hashTX)->GetSRV()]->GetView();
								SetPS_ShaderResourceView(pSrv, cnt++);
							}
						}
						Draw_Indices(pMesh->GetRendIndices()[j].count, pMesh->GetRendIndices()[j].idx, 0);
					}
					drawgizmo(GetMat_Scale(obj->m_vScale * 5.0f) * GetMat_RotFromMatrix(matWorld) * mat_translation_obj);
				}
			}
#endif // _SPACESHIP

			//objs
			for (int i = 0; i < objs.size(); i++)
			{
				const auto& obj = objs[i];
				if (!obj->bRenderable) continue;

				CB_WVPITMatrix cc0;
				Matrix4x4 mat_scale_obj = GetMat_Scale(obj->m_vScale);
				Matrix4x4 mat_rotate_obj = GetMat_RotateRollPitchYaw(obj->m_vRotate);
				Matrix4x4 mat_translation_obj = GetMat_Translation(obj->m_vPosition);
				cc0.matWorld = mat_scale_obj * mat_rotate_obj * mat_translation_obj;
				cc0.matView = matView;
				cc0.matProj = matProj;
				cc0.matInvTrans = GetMat_InverseTranspose(cc0.matWorld);
				m_pCCBs[g_hash_cbwvpitmat]->UpdateBufferData(m_pCDirect3D->GetDeviceContext(), &cc0);
				SetVS_ConstantBuffer(m_pCCBs[g_hash_cbwvpitmat]->GetBuffer(), 3);

				for (UINT j = 0; j < obj->m_Mesh_Material.size(); j++)
				{
					auto& iter = obj->m_Mesh_Material[j];
					//지정핸들링필요
					Mesh<Vertex_PTNTB>* pMesh = _ResourceSystem.GetResource<Mesh<Vertex_PTNTB>>(iter.hash_mesh);
					SetIA_VertexBuffer(m_pCVBs[pMesh->GetVB()]->GetBuffer(), m_pCVBs[pMesh->GetVB()]->GetVertexSize());
					SetIA_IndexBuffer(m_pCIBs[pMesh->GetIB()]->GetBuffer());

					Material* pMaterial = _ResourceSystem.GetResource<Material>(iter.hash_material);
					SetIA_InputLayout(m_pCILs[pMaterial->GetIL()]->GetInputLayout());
					SetVS(m_pCVSs[pMaterial->GetVS()]->GetShader());
					SetPS(m_pCPSs[pMaterial->GetPS()]->GetShader());

					const std::vector<size_t>* texs = pMaterial->GetTextures();
					int cnt = 0;
					for (int idxTex = 0; idxTex < (UINT)E_Texture::count; idxTex++)
					{
						for (const auto& hashTX : texs[idxTex])
						{
							auto pSrv = m_pCSRVs[_ResourceSystem.GetResource<Texture>(hashTX)->GetSRV()]->GetView();
							SetPS_ShaderResourceView(pSrv, cnt++);
						}
					}
					Draw_Indices(pMesh->GetRendIndices()[j].count, pMesh->GetRendIndices()[j].idx, 0);
				}
				drawgizmo(GetMat_Scale(obj->m_vScale * 5.0f) * mat_rotate_obj * mat_translation_obj);
			}
		}
#endif	
		//UIRender
		{
			//2D객체로 SRV임시체크
			m_pCDirect3D->GetDeviceContext()->OMSetRenderTargets(0, nullptr, nullptr);
			ID3D11RenderTargetView* RTVS[]
			{
				m_pCRTVs[m_hash_RTV_BB]->GetView(),
			};
			FLOAT clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
			m_pCDirect3D->GetDeviceContext()->OMSetRenderTargets(1, RTVS, NULL);
			m_pCDirect3D->GetDeviceContext()->ClearRenderTargetView(m_pCRTVs[m_hash_RTV_BB]->GetView(), clearColor);
			Matrix4x4 matOrtho = _CameraSystem.GetCamera(0)->GetOrthoMatrix();
			{
				//ortho_objs
				for (int i = 0; i < ortho_objs.size(); i++)
				{
					const auto& obj = ortho_objs[i];
					//if (!obj->bRenderable) continue;
					//상수버퍼에 cc0(wvp mat), cc1(시간) 을 세팅한다
					CB_WVPITMatrix cc0;
					cc0.matWorld = GetMat_ConvertGeometryOrtho() * GetMat_World(obj->m_vScale, obj->m_vRotate, obj->m_vPosition);// +Vector3(-(m_iWidth / 2.0f), m_iHeight / 2.0f, 0.0f));
					cc0.matView = GetMat_Identity();
					cc0.matProj = matOrtho;
					cc0.matInvTrans = GetMat_InverseTranspose(cc0.matWorld);
					m_pCCBs[g_hash_cbwvpitmat]->UpdateBufferData(m_pCDirect3D->GetDeviceContext(), &cc0);
					SetVS_ConstantBuffer(m_pCCBs[g_hash_cbwvpitmat]->GetBuffer(), 3);

					for (UINT j = 0; j < obj->m_Mesh_Material.size(); j++)
					{
						auto& iter = obj->m_Mesh_Material[j];
						Mesh<Vertex_PT>* pMesh = _ResourceSystem.GetResource<Mesh<Vertex_PT>>(iter.hash_mesh);
						SetIA_VertexBuffer(m_pCVBs[pMesh->GetVB()]->GetBuffer(), m_pCVBs[pMesh->GetVB()]->GetVertexSize());
						SetIA_IndexBuffer(m_pCIBs[pMesh->GetIB()]->GetBuffer());

						Material* pMaterial = _ResourceSystem.GetResource<Material>(iter.hash_material);
						SetIA_InputLayout(m_pCILs[pMaterial->GetIL()]->GetInputLayout());
						SetVS(m_pCVSs[pMaterial->GetVS()]->GetShader());
						SetPS(m_pCPSs[pMaterial->GetPS()]->GetShader());

						const std::vector<size_t>* texs = pMaterial->GetTextures();
						int cnt = 0;
						for (int idxTex = 0; idxTex < (UINT)E_Texture::count; idxTex++)
						{
							for (const auto& hashTX : texs[idxTex])
							{
								size_t hash_srv = _ResourceSystem.GetResource<Texture>(hashTX)->GetSRV();
								if (i == 0) SetPS_ShaderResourceView(m_pCSRVs[m_hash_RTV_0]->GetView(), cnt++);
								else SetPS_ShaderResourceView(m_pCSRVs[m_hash_DSV_0]->GetView(), cnt++);
							}
						}
						Draw_Indices(pMesh->GetRendIndices()[j].count, pMesh->GetRendIndices()[j].idx, 0);
					}
				}
			}
		}
	}
#endif // _ECS
}

void RenderSystem::PostRender()
{
	std::cout << "PostRender : " << "RenderSystem" << " Class" << '\n';
	SwapchainPresent(false);
}

RenderSystem::~RenderSystem()
{
	CoUninitialize();

	for (auto iter = m_pCVBs.begin(); iter != m_pCVBs.end();)
	{
		delete iter->second;
		iter = m_pCVBs.erase(iter);
	}

	for (auto iter = m_pCIBs.begin(); iter != m_pCIBs.end();)
	{
		delete iter->second;
		iter = m_pCIBs.erase(iter);
	}

	for (auto iter = m_pCILs.begin(); iter != m_pCILs.end();)
	{
		delete iter->second;
		iter = m_pCILs.erase(iter);
	}

	for (auto iter = m_pCVSs.begin(); iter != m_pCVSs.end();)
	{
		delete iter->second;
		iter = m_pCVSs.erase(iter);
	}

	for (auto iter = m_pCGSs.begin(); iter != m_pCGSs.end();)
	{
		delete iter->second;
		iter = m_pCGSs.erase(iter);
	}

	for (auto iter = m_pCPSs.begin(); iter != m_pCPSs.end();)
	{
		delete iter->second;
		iter = m_pCPSs.erase(iter);
	}

	for (auto iter = m_pCCBs.begin(); iter != m_pCCBs.end();)
	{
		delete iter->second;
		iter = m_pCCBs.erase(iter);
	}

	for (auto iter = m_pCSRVs.begin(); iter != m_pCSRVs.end();)
	{
		delete iter->second;
		iter = m_pCSRVs.erase(iter);
	}

	for (auto iter = m_pCDSVs.begin(); iter != m_pCDSVs.end();)
	{
		delete iter->second;
		iter = m_pCDSVs.erase(iter);
	}

	for (auto iter = m_pCRTVs.begin(); iter != m_pCRTVs.end();)
	{
		delete iter->second;
		iter = m_pCRTVs.erase(iter);
	}

	for (auto iter = m_pCRAs.begin(); iter != m_pCRAs.end();)
	{
		delete iter->second;
		iter = m_pCRAs.erase(iter);
	}

	for (auto iter = m_pCCAs.begin(); iter != m_pCCAs.end();)
	{
		delete iter->second;
		iter = m_pCCAs.erase(iter);
	}

	/*if (SkyObj)
		delete SkyObj;

	if (Gizmo)
		delete Gizmo;

	if (SpaceShip)
		delete SpaceShip;

	if (ReflectPlane)
		delete ReflectPlane;

	for (auto iter = objs.begin(); iter != objs.end();)
	{
		delete *iter;
		iter = objs.erase(iter);
	}

	for (auto iter = ortho_objs.begin(); iter != ortho_objs.end();)
	{
		delete* iter;
		iter = ortho_objs.erase(iter);
	}*/

	if (m_pCSamplers)
	{
		delete m_pCSamplers;
		m_pCSamplers = nullptr;
	}

	if (m_pCRasterizers)
	{
		delete m_pCRasterizers;
		m_pCRasterizers = nullptr;
	}

	if (m_pCDepthStencils)
	{
		delete m_pCDepthStencils;
		m_pCDepthStencils = nullptr;
	}

	if (m_pCBlends)
	{
		delete m_pCBlends;
		m_pCBlends = nullptr;
	}

	if (m_pCSwapChain)
	{
		delete m_pCSwapChain;
		m_pCSwapChain = nullptr;
	}
	
	if (m_pCDirect3D)
	{
		delete m_pCDirect3D;
		m_pCDirect3D = nullptr;
	}
}

ID3D11Device* RenderSystem::GetD3DDevice() const
{
	return m_pCDirect3D->GetDevice();
}

ID3D11DeviceContext* RenderSystem::GetD3DDeviceContext() const
{
	return m_pCDirect3D->GetDeviceContext();
}

void RenderSystem::OnResize(UINT width, UINT height)
{
	if (!m_pCDirect3D) return;
	if (width == 0 || height == 0) return;
	m_iWidth = width;
	m_iHeight = height;

	//refCount를 전부 해제시킨다, SwapChain의 ResizeBuffer시 기존버퍼에대한 모든 RefCount가 초기화되어 Comptr객체에서 해제된다
	if (m_pCRTVs.find(m_hash_RTV_BB) != m_pCRTVs.end()) m_pCRTVs[m_hash_RTV_BB]->GetView()->Release();
	if (m_pCRTVs.find(m_hash_RTV_0) != m_pCRTVs.end()) m_pCRTVs[m_hash_RTV_0]->GetView()->Release();
	if (m_pCSRVs.find(m_hash_RTV_0) != m_pCSRVs.end()) m_pCSRVs[m_hash_RTV_0]->GetView()->Release();
	if (m_pCDSVs.find(m_hash_DSV_0) != m_pCDSVs.end()) m_pCDSVs[m_hash_DSV_0]->GetView()->Release();
	if (m_pCSRVs.find(m_hash_DSV_0) != m_pCSRVs.end()) m_pCSRVs[m_hash_DSV_0]->GetView()->Release();

	m_pCSwapChain->GetSwapChain()->ResizeBuffers(1, m_iWidth, m_iHeight, DXGI_FORMAT_UNKNOWN, 0);
	m_hash_RTV_BB = CreateRenderTargetView(L"BackBufferRTV");
	m_hash_RTV_0 = CreateRenderTargetView(L"RTV0", m_iWidth, m_iHeight);
	m_hash_DSV_0 = CreateDepthStencilView(L"DSV0", m_iWidth, m_iHeight);
	//SetViewportSize(m_iWidth, m_iHeight);
	SetViewportSize(&m_vp_BB, m_iWidth, m_iHeight);
}

void RenderSystem::SetViewportSize(UINT iWidth, UINT iHeight)
{
	D3D11_VIEWPORT vp = {};
	vp.Width = (float)iWidth;
	vp.Height = (float)iHeight;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	m_pCDirect3D->GetDeviceContext()->RSSetViewports(1, &vp);
}

void RenderSystem::SetViewportSize(D3D11_VIEWPORT* pViewport, UINT iWidth, UINT iHeight)
{
	pViewport->Width = iWidth;
	pViewport->Height = iHeight;
}

size_t RenderSystem::CreateVertexBuffer(const std::wstring& szName, void* vertices, UINT size_vertex, UINT size_vertices)
{
	size_t hash = Hasing_wstring(szName);
	if (m_pCVBs.find(hash) != m_pCVBs.end()) return hash;
	VertexBuffer* pVertexBuffer = new VertexBuffer(m_pCDirect3D->GetDevice(), vertices, size_vertex, size_vertices);
	_ASEERTION_NULCHK(pVertexBuffer, "VB is nullptr");
	m_pCVBs[hash] = pVertexBuffer;
	return hash;
}

size_t RenderSystem::CreateInputLayout(const std::wstring& szName, D3D11_INPUT_ELEMENT_DESC* pInputElementDescs, UINT size_layout, ID3DBlob* vsBlob)
{
	size_t hash = Hasing_wstring(szName);
	if (m_pCILs.find(hash) != m_pCILs.end()) return hash;
	_ASEERTION_NULCHK(vsBlob, "Blob is nullptr");
	InputLayout* pInputLayout = new InputLayout(m_pCDirect3D->GetDevice(), pInputElementDescs, size_layout, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize());
	_ASEERTION_NULCHK(pInputLayout, "IL is nullptr");
	m_pCILs[hash] = pInputLayout;
	return hash;
}

size_t RenderSystem::CreateIndexBuffer(const std::wstring& szName, void* indices, UINT size_indices)
{
	size_t hash = Hasing_wstring(szName);
	if (m_pCIBs.find(hash) != m_pCIBs.end()) return hash;
	IndexBuffer* pIndexBuffer = new IndexBuffer(m_pCDirect3D->GetDevice(), indices, size_indices);
	_ASEERTION_NULCHK(pIndexBuffer, "IB is nullptr");
	m_pCIBs[hash] = pIndexBuffer;
	return hash;
}

size_t RenderSystem::CreateShaderResourceView(const std::wstring& szName, const ScratchImage* resource)
{
	size_t hash = Hasing_wstring(szName);
	if (m_pCSRVs.find(hash) != m_pCSRVs.end()) return hash;
	_ASEERTION_NULCHK(resource, "scratchImage is nullptr");
	ShaderResourceView* pSRV = new ShaderResourceView(m_pCDirect3D->GetDevice(), resource);
	_ASEERTION_NULCHK(pSRV, "VW is nullptr");
	m_pCSRVs[hash] = pSRV;
	return hash;
}

//백버퍼 렌더타겟뷰
size_t RenderSystem::CreateRenderTargetView(const std::wstring& szName)
{
	size_t hash = Hasing_wstring(szName);
	RenderTargetView* pRTV;
	if (m_pCRTVs.find(hash) != m_pCRTVs.end())
	{
		pRTV = m_pCRTVs[hash];
		pRTV->Resize(m_pCDirect3D->GetDevice(), m_pCSwapChain->GetSwapChain());
	}
	else
	{
		pRTV = new RenderTargetView(m_pCDirect3D->GetDevice(), m_pCSwapChain->GetSwapChain());
		m_pCRTVs[hash] = pRTV;
	}
	return hash;
}

//사이즈지정 렌더타겟뷰
size_t RenderSystem::CreateRenderTargetView(const std::wstring& szName, UINT width, UINT height)
{
	size_t hash = Hasing_wstring(szName);
	HRESULT hResult;
	// 렌더 타겟 텍스처 설명을 초기화합니다.
	ID3D11Texture2D* pBuffer;
	D3D11_TEXTURE2D_DESC desc_tex = GetTex2DDesc(E_ResourcesUsage::RTV, width, height);

	// 렌더 타겟 텍스처를 만듭니다.
	hResult = m_pCDirect3D->GetDevice()->CreateTexture2D(&desc_tex, NULL, &pBuffer);
	_ASEERTION_CREATE(hResult, "Buffer");

	D3D11_RENDER_TARGET_VIEW_DESC desc_rtv;
	desc_rtv.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	desc_rtv.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	desc_rtv.Texture2D.MipSlice = 0;

	RenderTargetView* pRTV;
	if (m_pCRTVs.find(hash) != m_pCRTVs.end())
	{
		pRTV = m_pCRTVs[hash];
		pRTV->Resize(m_pCDirect3D->GetDevice(), pBuffer, desc_rtv);
	}
	else
	{
		pRTV = new RenderTargetView(m_pCDirect3D->GetDevice(), pBuffer, desc_rtv);
		m_pCRTVs[hash] = pRTV;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC desc_srv;
	desc_srv.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	desc_srv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	desc_srv.Texture2D.MostDetailedMip = 0;
	desc_srv.Texture2D.MipLevels = 1;
	ShaderResourceView* pSRV;
	if (m_pCSRVs.find(hash) != m_pCSRVs.end())
	{
		pSRV = m_pCSRVs[hash];
		pSRV->Resize(m_pCDirect3D->GetDevice(), pBuffer, desc_srv);
	}
	else
	{
		pSRV = new ShaderResourceView(m_pCDirect3D->GetDevice(), pBuffer, desc_srv);
		m_pCSRVs[hash] = pSRV;
	}

	pBuffer->Release();	//사용을 끝낸 refCount감소
	return hash;
}

size_t RenderSystem::CreateDepthStencilView(const std::wstring& szName, UINT width, UINT height)
{
	size_t hash = Hasing_wstring(szName);
	//뎁스스텐실뷰에 사용할 텍스쳐버퍼 디스크립션
	HRESULT hResult;
	ID3D11Texture2D* pBuffer;
	D3D11_TEXTURE2D_DESC tex_desc = GetTex2DDesc(E_ResourcesUsage::DSV, width, height);

	hResult = m_pCDirect3D->GetDevice()->CreateTexture2D(&tex_desc, nullptr, &pBuffer);
	_ASEERTION_CREATE(hResult, "DepthBuffer");

	D3D11_DEPTH_STENCIL_VIEW_DESC desc_dsv;
	ZeroMemory(&desc_dsv, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	desc_dsv.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;													//깊이맵은 D24_UNROM_S8_UINT가정석
	desc_dsv.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	desc_dsv.Texture2D.MipSlice = 0;
	DepthStencilView* pDSV;
	if (m_pCDSVs.find(hash) != m_pCDSVs.end())
	{
		pDSV = m_pCDSVs[hash];
		pDSV->Resize(m_pCDirect3D->GetDevice(), pBuffer, desc_dsv);
	}
	else
	{
		pDSV = new DepthStencilView(m_pCDirect3D->GetDevice(), pBuffer, desc_dsv);
		m_pCDSVs[hash] = pDSV;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC desc_srv;
	desc_srv.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	desc_srv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	desc_srv.Texture2D.MostDetailedMip = 0;
	desc_srv.Texture2D.MipLevels = 1;
	ShaderResourceView* pSRV;
	if (m_pCSRVs.find(hash) != m_pCSRVs.end())
	{
		pSRV = m_pCSRVs[hash];
		pSRV->Resize(m_pCDirect3D->GetDevice(), pBuffer, desc_srv);
	}
	else
	{
		pSRV = new ShaderResourceView(m_pCDirect3D->GetDevice(), pBuffer, desc_srv);
		m_pCSRVs[hash] = pSRV;
	}

	pBuffer->Release();	//사용을 끝낸 refCount감소
	return hash;
}

std::vector<size_t>  RenderSystem::CreateCubeMapViews(const int width, const int height)
{
	// 렌더타겟은 백버퍼와다르게해상도가 다르므로 뷰포트를 해당크기에맞게끔지정해 드로우콜에이용해야한다

	//6(RTV), 1(SRV), 1(DSV)
	std::vector<size_t> hashs;
	size_t hash;
	HRESULT hResult;
	
	//큐브맵에 사용할 텍스쳐자원을 생성한다
	ID3D11Texture2D* pBuffer;
	D3D11_TEXTURE2D_DESC tex_desc = GetTex2DDesc(E_ResourcesUsage::RTV_CubeMap, width, height);
	hResult = m_pCDirect3D->GetDevice()->CreateTexture2D(&tex_desc, nullptr, &pBuffer);
	_ASEERTION_CREATE(hResult, "CubeMapTex");

	//RTV를 생성한다(6면)
	D3D11_RENDER_TARGET_VIEW_DESC desc_rtv;
	desc_rtv.Format = tex_desc.Format;
	desc_rtv.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
	desc_rtv.Texture2DArray.ArraySize = 1;
	desc_rtv.Texture2DArray.MipSlice = 0;

	for (int i = 0; i < 6; i++)
	{
		desc_rtv.Texture2DArray.FirstArraySlice = i;
		hash = Hasing_wstring(L"CubeMapRTV" + std::to_wstring(i));
		RenderTargetView* pRTV;
		pRTV = new RenderTargetView(m_pCDirect3D->GetDevice(), pBuffer, desc_rtv);
		m_pCRTVs[hash] = pRTV;
		hashs.push_back(hash);
	}
	
	//렌더링에 사용할 SRV자원을 생성한다
	D3D11_SHADER_RESOURCE_VIEW_DESC desc_srv;
	desc_srv.Format = tex_desc.Format;
	desc_srv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	desc_srv.TextureCube.MostDetailedMip = 0;
	desc_srv.TextureCube.MipLevels = -1;

	hash = Hasing_wstring(L"CubeMapSRV");
	ShaderResourceView* pSRV;
	pSRV = new ShaderResourceView(m_pCDirect3D->GetDevice(), pBuffer, desc_srv);
	m_pCSRVs[hash] = pSRV;
	hashs.push_back(hash);

	pBuffer->Release();	//사용을 끝낸 refCount감소
	
	//큐브맵에 사용할 텍스쳐자원을 생성한다(DSV)
	tex_desc = GetTex2DDesc(E_ResourcesUsage::DSV, width, height);
	hResult = m_pCDirect3D->GetDevice()->CreateTexture2D(&tex_desc, nullptr, &pBuffer);
	_ASEERTION_CREATE(hResult, "CubeMapTex");

	//렌더링에 사용할 DSV자원을 생성한다
	hash = Hasing_wstring(L"CubeMapDSV");
	D3D11_DEPTH_STENCIL_VIEW_DESC desc_dsv;
	ZeroMemory(&desc_dsv, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	desc_dsv.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;													//깊이맵은 D24_UNROM_S8_UINT가정석
	desc_dsv.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	desc_dsv.Texture2D.MipSlice = 0;
	DepthStencilView* pDSV;
	pDSV = new DepthStencilView(m_pCDirect3D->GetDevice(), pBuffer, desc_dsv);
	m_pCDSVs[hash] = pDSV;
	hashs.push_back(hash);

	pBuffer->Release();//사용을 끝낸 refCount감소

	return hashs;
}

//기하셰이더를 이용한 입방체맵의 생성
std::vector<size_t> RenderSystem::CreateCubeMapView(const int width, const int height)
{
	// 렌더타겟은 백버퍼와다르게해상도가 다르므로 뷰포트를 해당크기에맞게끔지정해 드로우콜에이용해야한다
	//1(RTV), 1(SRV), 1(DSV)
	std::vector<size_t> hashs;
	size_t hash;
	HRESULT hResult;

	//큐브맵에 사용할 텍스쳐자원을 생성한다
	ID3D11Texture2D* pBuffer;
	D3D11_TEXTURE2D_DESC tex_desc = GetTex2DDesc(E_ResourcesUsage::RTV_CubeMap, width, height);
	hResult = m_pCDirect3D->GetDevice()->CreateTexture2D(&tex_desc, nullptr, &pBuffer);
	_ASEERTION_CREATE(hResult, "CubeMapRTV,SRV Tex");

	//RTV를 생성한다(6면)
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	rtvDesc.Format = tex_desc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
	rtvDesc.Texture2DArray.FirstArraySlice = 0;
	rtvDesc.Texture2DArray.ArraySize = 6;
	rtvDesc.Texture2DArray.MipSlice = 0;
	hash = Hasing_wstring(L"CubeMapRTV");
	RenderTargetView* pRTV;
	pRTV = new RenderTargetView(m_pCDirect3D->GetDevice(), pBuffer, rtvDesc);
	m_pCRTVs[hash] = pRTV;
	hashs.push_back(hash);

	//렌더링에 사용할 SRV자원을 생성한다
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = tex_desc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.TextureCube.MostDetailedMip = 0;
	srvDesc.TextureCube.MipLevels = -1;

	hash = Hasing_wstring(L"CubeMapSRV");
	ShaderResourceView* pSRV;
	pSRV = new ShaderResourceView(m_pCDirect3D->GetDevice(), pBuffer, srvDesc);
	m_pCSRVs[hash] = pSRV;
	hashs.push_back(hash);

	pBuffer->Release();	//사용을 끝낸 refCount감소

	//큐브맵에 사용할 텍스쳐자원을 생성한다(DSV)
	tex_desc = GetTex2DDesc(E_ResourcesUsage::DSV_CubeMap, width, height);
	hResult = m_pCDirect3D->GetDevice()->CreateTexture2D(&tex_desc, nullptr, &pBuffer);
	_ASEERTION_CREATE(hResult, "CubeMapDSV Tex");

	//렌더링에 사용할 DSV자원을 생성한다
	hash = Hasing_wstring(L"CubeMapDSV");
	D3D11_DEPTH_STENCIL_VIEW_DESC desc_dsv;
	ZeroMemory(&desc_dsv, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	desc_dsv.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;													//깊이맵은 D24_UNROM_S8_UINT가정석
	desc_dsv.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
	desc_dsv.Texture2DArray.ArraySize = 6;
	desc_dsv.Texture2D.MipSlice = 0;
	DepthStencilView* pDSV;
	pDSV = new DepthStencilView(m_pCDirect3D->GetDevice(), pBuffer, desc_dsv);
	m_pCDSVs[hash] = pDSV;
	hashs.push_back(hash);

	pBuffer->Release();//사용을 끝낸 refCount감소

	return hashs;
}

void RenderSystem::RenderSkySphere(const Matrix4x4& matView, const Matrix4x4& matProj)
{
	SetOM_BlendState(m_pCBlends->GetState(E_BSState::Opaque), NULL);
	SetOM_DepthStenilState(m_pCDepthStencils->GetState(E_DSState::SKYBOX));
	SetPS_SamplerState(m_pCSamplers->GetState(E_Sampler::LINEAR_WRAP));
	SetRS_RasterizerState(m_pCRasterizers->GetState(E_RSState::SOLID_CULLBACK_CCW));
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
				m_pCCBs[g_hash_cbwvpitmat]->UpdateBufferData(m_pCDirect3D->GetDeviceContext(), &cb_wvpitmat);
				SetVS_ConstantBuffer(m_pCCBs[g_hash_cbwvpitmat]->GetBuffer(), 0);

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
					SetPS_Shader(m_pCPSs[pMaterial->GetPS()]->GetShader());

					const std::vector<size_t>* texs = pMaterial->GetTextures();
					int cnt = 0;
					for (int idxTex = 0; idxTex < (UINT)E_Texture::count; idxTex++)
					{
						for (const auto& hashTx : texs[idxTex])
						{
							auto pSrv = m_pCSRVs[_ResourceSystem.GetResource<Texture>(hashTx)->GetSRV()]->GetView();
							SetPS_ShaderResourceView(pSrv, cnt++);
						}
					}
					Draw_Indices(pMesh->GetRendIndices()[j].count, pMesh->GetRendIndices()[j].idx, 0);
				}
			}
			st_col = 0;
		}
	}
}

void RenderSystem::RenderGeometry(const Matrix4x4& matView, const Matrix4x4& matProj)
{
	SetOM_BlendState(m_pCBlends->GetState(E_BSState::Opaque), NULL);
	SetOM_DepthStenilState(m_pCDepthStencils->GetState(E_DSState::DEFAULT));
	SetPS_SamplerState(m_pCSamplers->GetState(E_Sampler::LINEAR_WRAP));
	SetPS_SamplerState(m_pCSamplers->GetState(E_Sampler::POINT_CLAMP_COMPARISON), 6);
	SetRS_RasterizerState(m_pCRasterizers->GetState(E_RSState::SOLID_CULLBACK_CW));
	ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Transform, C_Render, T_Render_Geometry>();
	std::vector<Archetype*> queries = _ECSSystem.QueryArchetypes(key);
	UINT renderCnt = 0;
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
				renderCnt++;
				const Vector3& scale = transforms[col].vScale;
				const Quarternion& rotate = transforms[col].qRotate;
				const Vector3& position = transforms[col].vPosition;
				CB_WVPITMatrix cb_wvpitmat;
				cb_wvpitmat.matWorld = GetMat_World(scale, rotate, position);
				cb_wvpitmat.matView = matView;
				cb_wvpitmat.matProj = matProj;
				cb_wvpitmat.matInvTrans = GetMat_InverseTranspose(cb_wvpitmat.matWorld);
				m_pCCBs[g_hash_cbwvpitmat]->UpdateBufferData(m_pCDirect3D->GetDeviceContext(), &cb_wvpitmat);
				SetVS_ConstantBuffer(m_pCCBs[g_hash_cbwvpitmat]->GetBuffer(), 0);

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
					SetPS_Shader(m_pCPSs[pMaterial->GetPS()]->GetShader());

					const std::vector<size_t>* texs = pMaterial->GetTextures();
					int cnt = 0;
					for (int idxTex = 0; idxTex < (UINT)E_Texture::count; idxTex++)
					{
						for (const auto& hashTx : texs[idxTex])
						{
							auto pSrv = m_pCSRVs[_ResourceSystem.GetResource<Texture>(hashTx)->GetSRV()]->GetView();
							SetPS_ShaderResourceView(pSrv, cnt++);
						}
					}
					Draw_Indices(pMesh->GetRendIndices()[j].count, pMesh->GetRendIndices()[j].idx, 0);
				}
			}
			st_col = 0;
		}
	}
	std::cout << "렌더링된객체수 : " << renderCnt << '\n';
}

void RenderSystem::RenderBillboard(const Vector3& campos, const Matrix4x4& matView, const Matrix4x4& matProj)
{
	SetOM_BlendState(m_pCBlends->GetState(E_BSState::Opaque), NULL);
	SetOM_DepthStenilState(m_pCDepthStencils->GetState(E_DSState::DEFAULT));
	SetPS_SamplerState(m_pCSamplers->GetState(E_Sampler::LINEAR_WRAP));
	SetRS_RasterizerState(m_pCRasterizers->GetState(E_RSState::SOLID_CULLBACK_CW));
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
				m_pCCBs[g_hash_cbwvpitmat]->UpdateBufferData(m_pCDirect3D->GetDeviceContext(), &cb_wvpitmat);
				SetVS_ConstantBuffer(m_pCCBs[g_hash_cbwvpitmat]->GetBuffer(), 0);

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
					SetPS_Shader(m_pCPSs[pMaterial->GetPS()]->GetShader());

					const std::vector<size_t>* texs = pMaterial->GetTextures();
					int cnt = 0;
					for (int idxTex = 0; idxTex < (UINT)E_Texture::count; idxTex++)
					{
						for (const auto& hashTx : texs[idxTex])
						{
							auto pSrv = m_pCSRVs[_ResourceSystem.GetResource<Texture>(hashTx)->GetSRV()]->GetView();
							SetPS_ShaderResourceView(pSrv, cnt++);
						}
					}
					Draw_Indices(pMesh->GetRendIndices()[j].count, pMesh->GetRendIndices()[j].idx, 0);
				}
			}
			st_col = 0;
		}
	}
}

void RenderSystem::RenderShadowMap(const Matrix4x4& matView, const Matrix4x4& matProj)
{
	SetOM_BlendState(m_pCBlends->GetState(E_BSState::Opaque), NULL);
	SetOM_DepthStenilState(m_pCDepthStencils->GetState(E_DSState::DEFAULT));
	SetPS_SamplerState(m_pCSamplers->GetState(E_Sampler::LINEAR_WRAP));
	SetRS_RasterizerState(m_pCRasterizers->GetState(E_RSState::SOLID_CULLBACK_CW));

	//그림자맵 텍스쳐를 구성하기위한 타겟(rtv null, dsv만) 세팅

	m_pCDirect3D->GetDeviceContext()->ClearDepthStencilView(m_pCDSVs[m_hash_DSV_ShadowMap]->GetView(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	m_pCDirect3D->GetDeviceContext()->OMSetRenderTargets(0, nullptr, m_pCDSVs[m_hash_DSV_ShadowMap]->GetView());

	//Render Geometry
	{
		ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Transform, C_Render, T_Render_Geometry>();
		std::vector<Archetype*> queries = _ECSSystem.QueryArchetypes(key);
		UINT renderCnt = 0;
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
					renderCnt++;
					const Vector3& scale = transforms[col].vScale;
					const Quarternion& rotate = transforms[col].qRotate;
					const Vector3& position = transforms[col].vPosition;
					CB_WVPITMatrix cb_wvpitmat;
					cb_wvpitmat.matWorld = GetMat_World(scale, rotate, position);
					cb_wvpitmat.matView = matView;
					cb_wvpitmat.matProj = matProj;
					cb_wvpitmat.matInvTrans = GetMat_InverseTranspose(cb_wvpitmat.matWorld);
					m_pCCBs[g_hash_cbwvpitmat]->UpdateBufferData(m_pCDirect3D->GetDeviceContext(), &cb_wvpitmat);
					SetVS_ConstantBuffer(m_pCCBs[g_hash_cbwvpitmat]->GetBuffer(), 0);

					const auto& MeshMats = m_pCRAs[renders[col].hash_ra]->m_hMeshMats;
					for (UINT j = 0; j < MeshMats.size(); j++)
					{
						auto& iter = MeshMats[j];
						BaseMesh* pMesh = _ResourceSystem.GetResource<BaseMesh>(iter.hash_mesh);
						SetIA_VertexBuffer(m_pCVBs[pMesh->GetVB()]->GetBuffer(), m_pCVBs[pMesh->GetVB()]->GetVertexSize());
						SetIA_IndexBuffer(m_pCIBs[pMesh->GetIB()]->GetBuffer());

						Material* pMaterial = _ResourceSystem.GetResource<Material>(m_hash_Mat_ShadowMap);
						SetIA_InputLayout(m_pCILs[pMaterial->GetIL()]->GetInputLayout());
						SetVS_Shader(m_pCVSs[pMaterial->GetVS()]->GetShader());
						SetPS_Shader(m_pCPSs[pMaterial->GetPS()]->GetShader());

						Draw_Indices(pMesh->GetRendIndices()[j].count, pMesh->GetRendIndices()[j].idx, 0);
					}
				}
				st_col = 0;
			}
		}
		std::cout << "렌더링된객체수 : " << renderCnt << '\n';
	}

	//Render RTV_CubeMap
#ifdef  _EnviornmentMap
	{
		ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Transform, C_Render, T_Render_CubeMap>();
		std::vector<Archetype*> queries = _ECSSystem.QueryArchetypes(key);
		//UINT renderCnt = 0;
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
					//renderCnt++;
					const Vector3& scale = transforms[col].vScale;
					const Quarternion& rotate = transforms[col].qRotate;
					const Vector3& position = transforms[col].vPosition;
					CB_WVPITMatrix cb_wvpitmat;
					cb_wvpitmat.matWorld = GetMat_World(scale, rotate, position);
					cb_wvpitmat.matView = matView;
					cb_wvpitmat.matProj = matProj;
					cb_wvpitmat.matInvTrans = GetMat_InverseTranspose(cb_wvpitmat.matWorld);
					m_pCCBs[g_hash_cbwvpitmat]->UpdateBufferData(m_pCDirect3D->GetDeviceContext(), &cb_wvpitmat);
					SetVS_ConstantBuffer(m_pCCBs[g_hash_cbwvpitmat]->GetBuffer(), 3);

					const auto& MeshMats = m_pCRAs[renders[col].hash_ra]->m_hMeshMats;
					for (UINT j = 0; j < MeshMats.size(); j++)
					{
						auto& iter = MeshMats[j];
						BaseMesh* pMesh = _ResourceSystem.GetResource<BaseMesh>(iter.hash_mesh);
						SetIA_VertexBuffer(m_pCVBs[pMesh->GetVB()]->GetBuffer(), m_pCVBs[pMesh->GetVB()]->GetVertexSize());
						SetIA_IndexBuffer(m_pCIBs[pMesh->GetIB()]->GetBuffer());

						Material* pMaterial = _ResourceSystem.GetResource<Material>(m_hash_Mat_ShadowMap);
						SetIA_InputLayout(m_pCILs[pMaterial->GetIL()]->GetInputLayout());
						SetVS_Shader(m_pCVSs[pMaterial->GetVS()]->GetShader());
						if (m_pCGSs.find(pMaterial->GetGS()) != m_pCGSs.end())
							SetGS_Shader(m_pCGSs[pMaterial->GetGS()]->GetShader());
						else
							SetGS_Shader(nullptr);
						SetPS_Shader(m_pCPSs[pMaterial->GetPS()]->GetShader());

						Draw_Indices(pMesh->GetRendIndices()[j].count, pMesh->GetRendIndices()[j].idx, 0);
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
					m_pCCBs[g_hash_cbwvpitmat]->UpdateBufferData(m_pCDirect3D->GetDeviceContext(), &cb_wvpitmat);
					SetVS_ConstantBuffer(m_pCCBs[g_hash_cbwvpitmat]->GetBuffer(), 0);

					const auto& MeshMats = m_pCRAs[renders[col].hash_ra]->m_hMeshMats;
					for (UINT j = 0; j < MeshMats.size(); j++)
					{
						auto& iter = MeshMats[j];
						BaseMesh* pMesh = _ResourceSystem.GetResource<BaseMesh>(iter.hash_mesh);
						SetIA_VertexBuffer(m_pCVBs[pMesh->GetVB()]->GetBuffer(), m_pCVBs[pMesh->GetVB()]->GetVertexSize());
						SetIA_IndexBuffer(m_pCIBs[pMesh->GetIB()]->GetBuffer());

						Material* pMaterial = _ResourceSystem.GetResource<Material>(m_hash_Mat_ShadowMap);
						SetIA_InputLayout(m_pCILs[pMaterial->GetIL()]->GetInputLayout());
						SetVS_Shader(m_pCVSs[pMaterial->GetVS()]->GetShader());
						SetPS_Shader(m_pCPSs[pMaterial->GetPS()]->GetShader());

						Draw_Indices(pMesh->GetRendIndices()[j].count, pMesh->GetRendIndices()[j].idx, 0);
					}
				}
				st_col = 0;
			}
		}
	}

	//원복
	ClearRenderViews(0.0f, 0.3f, 0.4f, 1.0f);

	auto pSrv = m_pCSRVs[m_hash_DSV_ShadowMap]->GetView();
	SetPS_ShaderResourceView(pSrv, 6);
}

void RenderSystem::RenderEnviornmentMap(const Matrix4x4& matView, const Matrix4x4& matProj)
{
	SetOM_BlendState(m_pCBlends->GetState(E_BSState::Opaque), NULL);
	SetOM_DepthStenilState(m_pCDepthStencils->GetState(E_DSState::DEFAULT));
	SetPS_SamplerState(m_pCSamplers->GetState(E_Sampler::ANISOTROPIC_WRAP));
	SetRS_RasterizerState(m_pCRasterizers->GetState(E_RSState::SOLID_CULLBACK_CW));
	ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Transform, C_Render, T_Render_CubeMap>();
	std::vector<Archetype*> queries = _ECSSystem.QueryArchetypes(key);
	//UINT renderCnt = 0;
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
				//renderCnt++;
				const Vector3& scale = transforms[col].vScale;
				const Quarternion& rotate = transforms[col].qRotate;
				const Vector3& position = transforms[col].vPosition;
				CB_WVPITMatrix cb_wvpitmat;
				cb_wvpitmat.matWorld = GetMat_World(scale, rotate, position);
				cb_wvpitmat.matView = matView;
				cb_wvpitmat.matProj = matProj;
				cb_wvpitmat.matInvTrans = GetMat_InverseTranspose(cb_wvpitmat.matWorld);
				m_pCCBs[g_hash_cbwvpitmat]->UpdateBufferData(m_pCDirect3D->GetDeviceContext(), &cb_wvpitmat);
				SetVS_ConstantBuffer(m_pCCBs[g_hash_cbwvpitmat]->GetBuffer(), 0);

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
					SetPS_Shader(m_pCPSs[pMaterial->GetPS()]->GetShader());

					const std::vector<size_t>* texs = pMaterial->GetTextures();
					int cnt = 0;
					for (int idxTex = 0; idxTex < (UINT)E_Texture::count; idxTex++)
					{
						for (const auto& hashTx : texs[idxTex])
						{
							auto pSrv = m_pCSRVs[_ResourceSystem.GetResource<Texture>(hashTx)->GetSRV()]->GetView();
							SetPS_ShaderResourceView(pSrv, cnt++);
						}
					}
					SetPS_ShaderResourceView(m_pCSRVs[_RenderSystem.m_hash_SRV_CubeMap]->GetView(), cnt++);
					Draw_Indices(pMesh->GetRendIndices()[j].count, pMesh->GetRendIndices()[j].idx, 0);
				}
			}
			st_col = 0;
		}
	}
	//std::cout << "렌더링된객체수 : " << renderCnt << '\n';
}

void RenderSystem::RenderCubeMap()
{
	//Render EnvironmentMap
	{
		SetRS_Viewport(&m_vp_CubeMap);
		for (int i = 0; i < 6; i++) 
		{
			RenderCubeMapTexture(i);
		}
		//원복
		ClearRenderViews(0.0f, 0.3f, 0.4f, 1.0f);
		SetRS_Viewport(&m_vp_BB);
		// RTV, SRV에 사용하는 버퍼의 밉맵을 형성한다(앨리어싱처리)
		m_pCDirect3D->GetDeviceContext()->GenerateMips(m_pCSRVs[m_hash_SRV_CubeMap]->GetView());
	}
}

void RenderSystem::RenderCubeMapTexture(UINT cubemapIdx)
{
	FLOAT clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	m_pCDirect3D->GetDeviceContext()->ClearRenderTargetView(m_pCRTVs[m_hash_RTV_CubeMap[cubemapIdx]]->GetView(), clearColor);
	m_pCDirect3D->GetDeviceContext()->ClearDepthStencilView(m_pCDSVs[m_hash_DSV_CubeMap]->GetView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
	ID3D11RenderTargetView* RTVS[] =
	{
		m_pCRTVs[m_hash_RTV_CubeMap[cubemapIdx]]->GetView(),
	};
	m_pCDirect3D->GetDeviceContext()->OMSetRenderTargets(ARRAYSIZE(RTVS), RTVS, m_pCDSVs[m_hash_DSV_CubeMap]->GetView());

	size_t lookup_cam = _CameraSystem.lookup_cubemapcam[cubemapIdx];
	const auto& c_cam_main = _ECSSystem.GetComponent<C_Camera>(lookup_cam);
	const auto& c_cam_proj = _ECSSystem.GetComponent<C_Projection>(lookup_cam);
	const Matrix4x4& cam_matWorld = c_cam_main.matWorld;
	const Matrix4x4& cam_matView = c_cam_main.matView;
	const Matrix4x4& cam_matProj = c_cam_proj.matProj;

	const auto& c_cam_transform = _ECSSystem.GetComponent<C_Transform>(lookup_cam);
	CB_Campos cb_campos;
	cb_campos.vPosition = c_cam_transform.vPosition;
	m_pCCBs[g_hash_cbcampos]->UpdateBufferData(m_pCDirect3D->GetDeviceContext(), &cb_campos);
	SetPS_ConstantBuffer(m_pCCBs[g_hash_cbcampos]->GetBuffer(), 5);

	//Render SkySphere
	RenderSkySphere(cam_matView, cam_matProj);

	//Render Geometry
	RenderGeometry(cam_matView, cam_matProj);

	//Render Billboard
	RenderBillboard(c_cam_transform.vPosition, cam_matView, cam_matProj);
}

void RenderSystem::RenderUI(const Matrix4x4& matOrtho)
{
	SetOM_BlendState(m_pCBlends->GetState(E_BSState::Opaque), NULL);
	SetOM_DepthStenilState(m_pCDepthStencils->GetState(E_DSState::UI));
	SetPS_SamplerState(m_pCSamplers->GetState(E_Sampler::LINEAR_WRAP));
	SetRS_RasterizerState(m_pCRasterizers->GetState(E_RSState::SOLID_CULLBACK_CW));
	//2D객체로 SRV임시체크
	m_pCDirect3D->GetDeviceContext()->OMSetRenderTargets(0, nullptr, nullptr);
	ID3D11RenderTargetView* RTVS[]
	{
		m_pCRTVs[m_hash_RTV_BB]->GetView(),
	};
	FLOAT clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	m_pCDirect3D->GetDeviceContext()->OMSetRenderTargets(1, RTVS, NULL);
	m_pCDirect3D->GetDeviceContext()->ClearRenderTargetView(m_pCRTVs[m_hash_RTV_BB]->GetView(), clearColor);

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
				m_pCCBs[g_hash_cbwvpitmat]->UpdateBufferData(m_pCDirect3D->GetDeviceContext(), &cb_wvpitmat);
				SetVS_ConstantBuffer(m_pCCBs[g_hash_cbwvpitmat]->GetBuffer(), 0);

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
					SetPS_Shader(m_pCPSs[pMaterial->GetPS()]->GetShader());

					const std::vector<size_t>* texs = pMaterial->GetTextures();
					int cnt = 0;
					for (int idxTex = 0; idxTex < (UINT)E_Texture::count; idxTex++)
					{
						for (const auto& hashTx : texs[idxTex])
						{
							auto pSrv = m_pCSRVs[hashTx]->GetView();
							SetPS_ShaderResourceView(m_pCSRVs[hashTx]->GetView(), cnt++);
						}
					}
					Draw_Indices(pMesh->GetRendIndices()[j].count, pMesh->GetRendIndices()[j].idx, 0);
				}
			}
		}
		st_col = 0;
	}
}


void RenderSystem::ClearRenderViews(float red, float green, float blue, float alpha)
{
	FLOAT clearColor[] = { red, green, blue, alpha };

	m_pCDirect3D->GetDeviceContext()->ClearRenderTargetView(m_pCRTVs[m_hash_RTV_0]->GetView(), clearColor);
	m_pCDirect3D->GetDeviceContext()->ClearDepthStencilView(m_pCDSVs[m_hash_DSV_0]->GetView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
	ID3D11RenderTargetView* RTVS[] =
	{
		m_pCRTVs[m_hash_RTV_0]->GetView(),
	};
	m_pCDirect3D->GetDeviceContext()->OMSetRenderTargets(ARRAYSIZE(RTVS), RTVS, m_pCDSVs[m_hash_DSV_0]->GetView());

	/*m_pCDirect3D->GetDeviceContext()->ClearRenderTargetView(m_pCRTVs[m_hash_RTV_BB]->GetView(), clearColor);
	m_pCDirect3D->GetDeviceContext()->ClearDepthStencilView(m_pCDSVs[m_hash_DSV_0]->GetView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
	ID3D11RenderTargetView* RTVS[] =
	{
		m_pCRTVs[m_hash_RTV_BB]->GetView(),
	};
	m_pCDirect3D->GetDeviceContext()->OMSetRenderTargets(ARRAYSIZE(RTVS), RTVS, m_pCDSVs[m_hash_DSV_0]->GetView());*/
}

void RenderSystem::SetIA_Topology(D3D_PRIMITIVE_TOPOLOGY topology)
{
	m_pCDirect3D->GetDeviceContext()->IASetPrimitiveTopology(topology);
}

void RenderSystem::SetIA_InputLayout(ID3D11InputLayout* pInputLayout)
{
	m_pCDirect3D->GetDeviceContext()->IASetInputLayout(pInputLayout);
}

void RenderSystem::SetIA_VertexBuffer(ID3D11Buffer* pBuffer, UINT iSizeVertex, UINT offset)
{
	m_pCDirect3D->GetDeviceContext()->IASetVertexBuffers(0, 1, &pBuffer, &iSizeVertex, &offset);
}

void RenderSystem::SetIA_IndexBuffer(ID3D11Buffer* pBuffer, UINT offset)
{
	m_pCDirect3D->GetDeviceContext()->IASetIndexBuffer(pBuffer, DXGI_FORMAT_R32_UINT, offset); //4바이트, 32비트를의미
}

void RenderSystem::SetVS_Shader(ID3D11VertexShader* pVS)
{
	m_pCDirect3D->GetDeviceContext()->VSSetShader(pVS, nullptr, 0);
}

void RenderSystem::SetVS_ShaderResourceView(ID3D11ShaderResourceView* pSRV, UINT startIdx)
{
	m_pCDirect3D->GetDeviceContext()->VSSetShaderResources(startIdx, 1, &pSRV);
}

void RenderSystem::SetVS_ConstantBuffer(ID3D11Buffer* pBuffer, UINT startIdx)
{
	m_pCDirect3D->GetDeviceContext()->VSSetConstantBuffers(startIdx, 1, &pBuffer);
}

void RenderSystem::SetVS_SamplerState(ID3D11SamplerState* pState, UINT startIdx)
{
	m_pCDirect3D->GetDeviceContext()->VSSetSamplers(startIdx, 1, &pState);
}

void RenderSystem::SetGS_Shader(ID3D11GeometryShader* pGS)
{
	m_pCDirect3D->GetDeviceContext()->GSSetShader(pGS, nullptr, 0);
}

void RenderSystem::SetPS_SamplerState(ID3D11SamplerState* pState, UINT startIdx)
{
	m_pCDirect3D->GetDeviceContext()->PSSetSamplers(startIdx, 1, &pState);
}

void RenderSystem::SetPS_Shader(ID3D11PixelShader* pPS)
{
	m_pCDirect3D->GetDeviceContext()->PSSetShader(pPS, nullptr, 0);
}

void RenderSystem::SetPS_ShaderResourceView(ID3D11ShaderResourceView* pSRV, UINT startIdx)
{
	m_pCDirect3D->GetDeviceContext()->PSSetShaderResources(startIdx, 1, &pSRV);
}

void RenderSystem::SetPS_ConstantBuffer(ID3D11Buffer* pBuffer, UINT startIdx)
{
	m_pCDirect3D->GetDeviceContext()->PSSetConstantBuffers(startIdx, 1, &pBuffer);
}

void RenderSystem::SetRS_RasterizerState(ID3D11RasterizerState* pState)
{
	m_pCDirect3D->GetDeviceContext()->RSSetState(pState);
}

void RenderSystem::SetRS_Viewport(D3D11_VIEWPORT* pViewport)
{
	m_pCDirect3D->GetDeviceContext()->RSSetViewports(1, pViewport);
}

void RenderSystem::SetOM_DepthStenilState(ID3D11DepthStencilState* pState, UINT stencilRef)
{
	m_pCDirect3D->GetDeviceContext()->OMSetDepthStencilState(pState, stencilRef);
}

void RenderSystem::SetOM_BlendState(ID3D11BlendState* pState, const FLOAT* blendFactor, UINT sampleMask)
{
	m_pCDirect3D->GetDeviceContext()->OMSetBlendState(pState, blendFactor, sampleMask);
}

void RenderSystem::Draw_Vertices(UINT vertexCount, UINT startIdx)
{
	m_pCDirect3D->GetDeviceContext()->Draw(vertexCount, startIdx);
}

void RenderSystem::Draw_Indices(UINT indexCount, UINT startIdx, INT vertexOffset)
{
	m_pCDirect3D->GetDeviceContext()->DrawIndexed(indexCount, startIdx, vertexOffset);
}

void RenderSystem::SwapchainPresent(bool vsync)
{
	std::cout << "SwapchainPreset, 백버퍼교체" << '\n';
	m_pCSwapChain->GetSwapChain()->Present(vsync, NULL);
}

size_t RenderSystem::CreateConstantBuffer(const type_info& typeinfo, UINT size_buffer, void* data)
{
	size_t hash = typeinfo.hash_code();
	if (m_pCCBs.find(hash) != m_pCCBs.end()) return hash;
	ConstantBuffer* pConstantBuffer = new ConstantBuffer(m_pCDirect3D->GetDevice(), size_buffer, data);
	_ASEERTION_NULCHK(pConstantBuffer, "CB is nullptr");
	m_pCCBs[hash] = pConstantBuffer;
	return hash;
}

size_t RenderSystem::CreateVertexShader(std::wstring shaderName, std::string entryName, std::string target)
{
	size_t hash = Hasing_wstring(shaderName);
	if (m_pCVSs.find(hash) != m_pCVSs.end()) return hash;

	VertexShader* pVertexShader = new VertexShader(m_pCDirect3D->GetDevice(), CompileShader(shaderName, entryName, target));
	_ASEERTION_NULCHK(pVertexShader, "VS is nullptr");
	m_pCVSs[hash] = pVertexShader;
	return hash;
}

size_t RenderSystem::CreateGeometryShader(std::wstring shaderName, std::string entryName, std::string target)
{
	size_t hash = Hasing_wstring(shaderName);
	if (m_pCGSs.find(hash) != m_pCGSs.end()) return hash;

	GeometryShader* pGeometryShader = new GeometryShader(m_pCDirect3D->GetDevice(), CompileShader(shaderName, entryName, target));
	_ASEERTION_NULCHK(pGeometryShader, "GS is nullptr");
	m_pCGSs[hash] = pGeometryShader;
	return hash;
}

size_t RenderSystem::CreatePixelShader(std::wstring shaderName, std::string entryName, std::string target)
{
	size_t hash = Hasing_wstring(shaderName);
	if (m_pCPSs.find(hash) != m_pCPSs.end()) return hash;

	PixelShader* pPixelShader = new PixelShader(m_pCDirect3D->GetDevice(), CompileShader(shaderName, entryName, target));
	_ASEERTION_NULCHK(pPixelShader, "PS is nullptr");
	m_pCPSs[hash] = pPixelShader;
	return hash;
}

// DirectXTex의 함수를 이용하여 image_data로 리턴, imageData로부터 ID3D11Resource 객체를 생성한다
size_t RenderSystem::CreateTexture(const std::wstring& szFilePath, DirectX::WIC_FLAGS flag)
{
	Texture* pTexture = _ResourceSystem.CreateResourceFromFile<Texture>(szFilePath, flag);
	pTexture->SetSRV(CreateShaderResourceView(szFilePath + L"VW", pTexture->GetImage()));
	return pTexture->GetHash();
}

// DirectXTex의 함수를 이용하여 image_data로 리턴, imageData로부터 ID3D11Resource 객체를 생성한다
size_t RenderSystem::CreateTexture(const std::wstring& szFilePath, DirectX::DDS_FLAGS flag)
{
	Texture* pTexture = _ResourceSystem.CreateResourceFromFile<Texture>(szFilePath, flag);
	pTexture->SetSRV(CreateShaderResourceView(szFilePath + L"VW", pTexture->GetImage()));
	return pTexture->GetHash();
}

template<typename T>
size_t RenderSystem::CreateMeshFromGeometry(const std::wstring szName, std::vector<std::vector<Vector3>>&& points, std::vector<T>&& vertices, std::vector<UINT>&& indices)
{
	std::vector<RenderCounts> countsVertices;
	countsVertices.push_back({ (UINT)vertices.size(), 0 });
	std::vector<RenderCounts> countsIndices;
	countsIndices.push_back({ (UINT)indices.size(), 0 });
	Mesh<T>* pMesh = _ResourceSystem.CreateResourceFromFile<Mesh<T>>(szName, std::move(points), std::move(vertices), std::move(countsVertices), std::move(indices), std::move(countsIndices));
	std::wstring szTypename = _tomw(typeid(T).name());
	pMesh->SetVB(CreateVertexBuffer(szName + szTypename + L"VB", pMesh->GetVertices(), sizeof(T), (UINT)pMesh->GetVerticesSize()));
	pMesh->SetIB(CreateIndexBuffer(szName + szTypename + L"IB", pMesh->GetIndices(), (UINT)pMesh->GetIndicesSize()));
	return pMesh->GetHash();
}

template<typename T>
size_t RenderSystem::CreateMesh(const std::wstring& szFilePath)
{
	std::map<UINT, MTL_TEXTURES> texturesByMaterial;
	Mesh<T>* pMesh = _ResourceSystem.CreateMeshFromFile<T>(szFilePath, texturesByMaterial);
	std::wstring szTypename = _tomw(typeid(T).name());
	pMesh->SetVB(CreateVertexBuffer(szFilePath + szTypename + L"VB", pMesh->GetVertices(), sizeof(T), (UINT)pMesh->GetVerticesSize()));
	pMesh->SetIB(CreateIndexBuffer(szFilePath + szTypename + L"IB", pMesh->GetIndices(), (UINT)pMesh->GetIndicesSize()));
	return pMesh->GetHash();
}

template<typename T>
const std::unordered_set<size_t>& RenderSystem::CreateColliders(size_t hash_mesh, E_Collider collider)
{
	Mesh<T>* pMesh = _ResourceSystem.GetResource<Mesh<T>>(hash_mesh);
	if (pMesh->GetCL().empty())	//Vertex 자료형에 따른 중복정의에서 중복된 컬라이더생성을 방지
	{
		for (UINT idx = 0; idx < pMesh->GetPoints().size(); idx++)
			pMesh->SetCL(_CollisionSystem.CreateCollider(pMesh->GetPath() + std::to_wstring(idx), &pMesh->GetPoints()[idx], collider));
	}
	return pMesh->GetCL();
}

template<typename T>
size_t RenderSystem::CreateMaterial(const std::wstring& szFilePath)
{
	Material* pMaterial = _ResourceSystem.CreateResourceFromFile<Material>(szFilePath);
	return pMaterial->GetHash();
}

/*
template<typename T>
std::vector<size_t> RenderSystem::CreateMaterialsFromFile(const std::wstring& szFilePath)
{
	std::vector<size_t> rets;
	std::vector<Material*> materials = _ResourceSystem.CreateResourcesFromFile<Material>(szFilePath);
	for (const auto& iter : materials)
		rets.push_back(iter->GetHash());
	return rets;
}
*/
void RenderSystem::Material_SetVS(size_t hash_material, const std::wstring& vsName)
{
	Material* pMaterial = _ResourceSystem.GetResource<Material>(hash_material);
	pMaterial->SetVS(CreateVertexShader(vsName, "vsmain", "vs_5_0"));
}

template<typename T>
void RenderSystem::Material_SetIL(size_t hash_material, const std::wstring& vsName)
{
	Material* pMaterial = _ResourceSystem.GetResource<Material>(hash_material);
	pMaterial->SetIL(CreateInputLayout(vsName + L"IL", Traits_InputLayout<T>::GetLayout(), Traits_InputLayout<T>::GetSize(), m_pCVSs[CreateVertexShader(vsName, "vsmain", "vs_5_0")]->GetBlob()));
}

void RenderSystem::Material_SetGS(size_t hash_material, const std::wstring& gsName)
{
	Material* pMaterial = _ResourceSystem.GetResource<Material>(hash_material);
	pMaterial->SetGS(CreateGeometryShader(gsName, "gsmain", "gs_5_0"));
}

void RenderSystem::Material_SetPS(size_t hash_material, const std::wstring& psName)
{
	Material* pMaterial = _ResourceSystem.GetResource<Material>(hash_material);
	pMaterial->SetPS(CreatePixelShader(psName, "psmain", "ps_5_0"));
}

void RenderSystem::Material_SetTextures(size_t hash_material, const std::vector<TX_HASH>& textures)
{
	Material* pMaterial = _ResourceSystem.GetResource<Material>(hash_material);
	for (const auto& iter : textures)
		pMaterial->SetTexture(iter);
}

void RenderSystem::CreateCubeMapTexture(int iSize)
{
	auto hashs = CreateCubeMapViews(iSize, iSize);
	for (int i = 0; i < 6; i++)
		m_hash_RTV_CubeMap[i] = hashs[i];
	m_hash_SRV_CubeMap = hashs[6];
	m_hash_DSV_CubeMap = hashs[7];
}


// 그림자매핑에 필요한 자원들을 구성한다
size_t RenderSystem::CreateShadowMapTexture(const int width, const int height)
{
	size_t hash;
	HRESULT hResult;

	//그림자맵에 사용할 텍스쳐자원을 생성한다
	ID3D11Texture2D* pBuffer;
	D3D11_TEXTURE2D_DESC tex_desc = GetTex2DDesc(E_ResourcesUsage::DSV, width, height);
	hResult = m_pCDirect3D->GetDevice()->CreateTexture2D(&tex_desc, nullptr, &pBuffer);
	_ASEERTION_CREATE(hResult, "ShadowMapTex");

	//렌더링에 사용할 DSV자원을 생성한다
	hash = Hasing_wstring(L"ShadowMap");

	D3D11_DEPTH_STENCIL_VIEW_DESC desc_dsv;
	ZeroMemory(&desc_dsv, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	desc_dsv.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	desc_dsv.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	desc_dsv.Texture2D.MipSlice = 0;
	DepthStencilView* pDSV;
	pDSV = new DepthStencilView(m_pCDirect3D->GetDevice(), pBuffer, desc_dsv);
	m_pCDSVs[hash] = pDSV;

	//렌더링에 사용할 SRV자원을 생성한다
	D3D11_SHADER_RESOURCE_VIEW_DESC desc_srv;
	desc_srv.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	desc_srv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	desc_srv.Texture2D.MostDetailedMip = 0;
	desc_srv.Texture2D.MipLevels = 1;

	ShaderResourceView* pSRV;
	pSRV = new ShaderResourceView(m_pCDirect3D->GetDevice(), pBuffer, desc_srv);
	m_pCSRVs[hash] = pSRV;

	pBuffer->Release();//사용을 끝낸 refCount감소
	return hash;
}

size_t RenderSystem::CreateRenderAsset(const std::wstring& szName, const std::vector<Mesh_Material>& hashs)
{
	size_t hash = Hasing_wstring(szName);
	if (m_pCRAs.find(hash) != m_pCRAs.end()) return hash;

	RenderAsset* pRenderAsset = new RenderAsset(szName);
	_ASEERTION_NULCHK(pRenderAsset, "RA is nullptr");
	pRenderAsset->m_hMeshMats = hashs;
	m_pCRAs[hash] = pRenderAsset;
	return hash;
}

size_t RenderSystem::CreateColliderAsset(const std::wstring& szName, const std::unordered_set<size_t>& hashs)
{
	size_t hash = Hasing_wstring(szName);
	if (m_pCCAs.find(hash) != m_pCCAs.end()) return hash;

	ColliderAsset* pColliderAsset = new ColliderAsset(szName);
	_ASEERTION_NULCHK(pColliderAsset, "RA is nullptr");
	pColliderAsset->m_hColliders = hashs;
	m_pCCAs[hash] = pColliderAsset;
	return hash;
}

const ColliderAsset* RenderSystem::GetColliderAsset(size_t hash)
{
	return m_pCCAs[hash];
}

ID3DBlob* RenderSystem::CompileShader(std::wstring shaderName, std::string entryName, std::string target)
{
	ID3DBlob* pBlob;
	ID3DBlob* errBlob;
	HRESULT hResult;
	DWORD dwShaderFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
	//compile Shader
	//D3D_COMPILE_STANDARD_FILE_INCLUDE, include를 위해 추가
	hResult = D3DCompileFromFile(shaderName.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, entryName.c_str(), target.c_str(), dwShaderFlags, NULL, &pBlob, &errBlob);
	if (FAILED(hResult))
	{
		OutputDebugStringA((char*)errBlob->GetBufferPointer());
		if (errBlob)
			errBlob->Release();
		_ASEERTION_CREATE(hResult, "CompileShader");
	}
	return pBlob;
}

