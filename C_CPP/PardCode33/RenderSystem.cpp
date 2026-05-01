#include "RenderSystem.h"
#include "EngineSystem.h"
//Systems
#include "InputSystem.h"
#include "ResourceSystem.h"
#include "ECSSystem.h"
#include "CameraSystem.h"
#include "CollisionSystem.h"
#include "AnimationSystem.h"

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




RenderSystem::RenderSystem()
{
}

void RenderSystem::Init()
{
	OnResize(g_iWidth, g_iHeight);
}

void RenderSystem::Frame(float deltatime, float elapsedtime)
{
	if (g_fTime_Log >= 1.0f)
		std::cout << "Frame : " << "EngineSystem" << " Class" << '\n';
	const auto& c_cam_main = _ECSSystem.GetComponent<C_Camera>(_CameraSystem.lookup_maincam);
	const Matrix4x4& cam_matWorld = c_cam_main.matWorld;
	//RenderPass
	CollectRenderItem(cam_matWorld[3].ToVector3());
	_EngineSystem.SortRenderItem();
}

void RenderSystem::PreRender(float deltatime, float elapsedtime)
{
	if (g_fTime_Log >= 1.0f)
		std::cout << "PreRender : " << "EngineSystem" << " Class" << '\n';
	
	//RTV초기화
	ClearRenderTargetView(_EngineSystem.m_hash_RTV_BB, 0.0f, 0.0f, 0.0f, 1.0f);
	ClearRenderTargetView(_EngineSystem.m_hash_RTV_0, 0.0f, 0.3f, 0.4f, 1.0f);
	ClearRenderTargetView(_EngineSystem.m_hash_RTV_CubeMap, 0.0f, 0.0f, 0.0f, 1.0f);

	ClearDepthStencilView(_EngineSystem.m_hash_DSV_0);
	ClearDepthStencilView(_EngineSystem.m_hash_DSV_ShadowMap);
	ClearDepthStencilView(_EngineSystem.m_hash_DSV_CubeMap);

	/*SetOM_RenderTargets({ _EngineSystem.m_hash_RTV_0 }, _EngineSystem.m_hash_DSV_0);
	SetRS_Viewport(&_EngineSystem.m_vp_BB);*/
}

void RenderSystem::Render(float deltatime, float elapsedtime)
{
	if (g_fTime_Log >= 1.0f)
		std::cout << "Render : " << "EngineSystem" << " Class" << '\n';

	SetPS_ConstantBuffer(g_hash_cb_directionalLight, 0);
	SetPS_ConstantBuffer(g_hash_cb_pointLight, 1);
	SetPS_ConstantBuffer(g_hash_cb_spotLight, 2);

	SetVS_ConstantBuffer(g_hash_cb_wvpitmat, 0);
	SetDS_ConstantBuffer(g_hash_cb_wvpitmat, 0);	//tesselate sphere 
	SetGS_ConstantBuffer(g_hash_cb_wvpitmat, 0);	//geometry box

	SetVS_ConstantBuffer(g_hash_cb_lightmat, 1);	//shadowmap
	SetGS_ConstantBuffer(g_hash_cb_lightmat, 1);

	SetVS_ConstantBuffer(g_hash_cb_bonemat, 2);		//animation

	SetPS_ConstantBuffer(g_hash_cb_campos, 5);		//campos

	SetGS_ConstantBuffer(g_hash_cb_cubemap, 7);		//cubemap

	SetGS_ConstantBuffer(g_hash_cb_debug_box, 5);	//debug property
	SetHS_ConstantBuffer(g_hash_cb_debug_sphere, 5);
	SetDS_ConstantBuffer(g_hash_cb_debug_sphere, 5);

	SetVS_ConstantBuffer(g_hash_cb_outline_picking, 3); //outline

	CB_DirectionalLight cb_directional;
	{
		size_t lookup = _EngineSystem.m_hash_light_directional;
		const auto& lightDir = _ECSSystem.GetComponent<C_Light_Direction>(lookup);
		const auto& c_light = _ECSSystem.GetComponent<C_Light>(lookup);
		cb_directional.mAmbient = c_light.vAmbient;
		cb_directional.mDiffuse = c_light.vDiffuse;
		cb_directional.mSpecular = c_light.vSpecular;
		cb_directional.vDirection = Vector4(Quaternion(lightDir.dir).GetForwardAxis(), c_light.fShiness);
	}
	_EngineSystem.UpdateConstantBuffer(g_hash_cb_directionalLight, &cb_directional);
	

	CB_PointLight cb_point;
	{
		size_t lookup = _EngineSystem.m_hash_light_point;
		const auto& c_transform = _ECSSystem.GetComponent<C_Transform>(lookup);
		const auto& c_light = _ECSSystem.GetComponent<C_Light>(lookup);
		const auto& c_attenuation = _ECSSystem.GetComponent<C_Light_Attenuation>(lookup);
		cb_point.mAmbient = c_light.vAmbient;
		cb_point.mDiffuse = c_light.vDiffuse;
		cb_point.mSpecular = c_light.vSpecular;
		cb_point.vPosition = Vector4(c_transform.vPosition, c_light.fShiness);
		cb_point.fAttenuations = Vector4(c_attenuation.fAtt_a0, c_attenuation.fAtt_a1, c_attenuation.fAtt_a2, c_attenuation.fRange);
	}
	_EngineSystem.UpdateConstantBuffer(g_hash_cb_pointLight, &cb_point);
	

	CB_SpotLight cb_spot;
	{
		size_t lookup = _EngineSystem.m_hash_light_spot;
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
	_EngineSystem.UpdateConstantBuffer(g_hash_cb_spotLight, &cb_spot);
	

	CB_LightMatrix cb_lightMat;
	{
		size_t lookup = _EngineSystem.m_hash_light_directional;
		const auto& lightDir = _ECSSystem.GetComponent<C_Light_Direction>(lookup);
		const auto& c_cam = _ECSSystem.GetComponent<C_Camera>(_CameraSystem.lookup_maincam);
		Vector4 pos = -lightDir.dir * 5000.0f;
		cb_lightMat.matLightView = GetMat_View(pos.ToVector3(), lightDir.dir.Normalize());
		cb_lightMat.matLightProj = GetMat_Orthographic(c_cam.fScreenWidth, c_cam.fScreenHeight, c_cam.fNear, 10000.0f);
		cb_lightMat.vPos = pos;
	}
	_EngineSystem.UpdateConstantBuffer(g_hash_cb_lightmat, &cb_lightMat);


	//메인카메라
	size_t lookup_maincam			= _CameraSystem.lookup_maincam;
	const auto& c_cam_main			= _ECSSystem.GetComponent<C_Camera>(lookup_maincam);
	const auto& c_cam_proj			= _ECSSystem.GetComponent<C_Projection>(lookup_maincam);
	const auto& c_cam_ortho			= _ECSSystem.GetComponent<C_Orthographic>(lookup_maincam);
	const Matrix4x4& cam_matWorld	= c_cam_main.matWorld;
	const Matrix4x4& cam_matView	= c_cam_main.matView;
	const Matrix4x4& cam_matProj	= c_cam_proj.matProj;
	const Matrix4x4& cam_matOrhto	= c_cam_ortho.matOrtho;
	const Vector3& cam_pos			= _ECSSystem.GetComponent<C_Transform>(lookup_maincam).vPosition;

	//큐브카메라
	size_t lookup_cubecam = _CameraSystem.lookup_cubemapcam;
	const auto& c_cubecam_proj = _ECSSystem.GetComponent<C_Projection>(lookup_cubecam);
	const Matrix4x4& matProj = c_cubecam_proj.matProj;

	//큐브맵 뷰행렬
	const Vector3& cam_pos_cubemap = _ECSSystem.GetComponent<C_Transform>(lookup_cubecam).vPosition;

	//카메라위치세팅
	CB_Campos cb_campos;

	float x = cam_pos_cubemap.GetX(), y = cam_pos_cubemap.GetY(), z = cam_pos_cubemap.GetZ();
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

	//큐브맵행렬 세팅
	CB_CubeMap cb_cubemap;
	for (int i = 0; i < 6; i++)
		cb_cubemap.matViews[i] = GetMat_View(cam_pos_cubemap, targets[i], ups[i]);
	_EngineSystem.UpdateConstantBuffer(g_hash_cb_cubemap, &cb_cubemap);

	CB_Outline_Picking cb_outline;
	Vector3 outlineColor = Vector3(255.0f, 165.0f, 0.0f) / 255.0f;
	cb_outline.color_thickness = Vector4(outlineColor, _EngineSystem.m_fThickness); //rgb, thickness;
	_EngineSystem.UpdateConstantBuffer(g_hash_cb_outline_picking, &cb_outline);
	

	_RPKey keyPrev = _HashNotInitialize;

	//ExecuteRenderPass
	// 1. Pass는 4비트면 충분하므로 uint8_t로도 충분하지만, 가독성을 위해 유지
	// 2. 셰이더 조합 ID (16비트)
	// 3. 상태(RS, BS, DSS) 조합 ID (8비트)
	// 4. 리소스(Material/Texture) ID (16비트)
	// 5. 거리 계산 (20비트) - float 거리를 받아서 20비트 정수로 변환
	// 4 + 16 + 8 + 16 + 20 -> 64비트 hash 비트별구분
	for (UINT idx = 0; idx < _EngineSystem.m_hRP_CommandQueue.size(); idx++)
	{
		const auto& renderItem = _EngineSystem.m_hRP_CommandQueue[idx];
	/*}
	for (const auto& renderItem : _EngineSystem.m_hRP_CommandQueue)
	{*/
		_RPKey keyCur			= renderItem.sortKey;
		Archetype* pArchetype	= renderItem.pArchetype;
		size_t row				= renderItem.entityRow;
		size_t col				= renderItem.entityCol;
		UINT renderCnt			= renderItem.renderCnt;
		UINT startIdx			= renderItem.startIdx;

		//debug entity
		const auto& info = pArchetype->GetComponents<C_Info>(row)[col];
		int a = 0;

		size_t diff = keyCur ^ keyPrev;				//xor, 1비트라도 다르면 시프트후 해당비트에 값이존재
		uint32_t prevPass = (keyPrev >> 60) & 0xF;	
		uint32_t curPass = (keyCur >> 60) & 0xF;	//4비트

		if (diff >> 60) //패스가 바뀌었는지 확인(비트 60~63)
		{
			SetRS_Viewport(&_EngineSystem.m_vp_BB);							//Default	Viewport
			SetPS_SamplerState(E_SMState::LINEAR_WRAP);						//Default	SamplerState
			cb_campos.vPosition = cam_pos;
			_EngineSystem.UpdateConstantBuffer(g_hash_cb_campos, &cb_campos);
			
		
			// RenderTarget 교체 및 패스 준비
			switch (curPass)
			{
				case (uint32_t)E_RenderPass::Sky:
				case (uint32_t)E_RenderPass::Opaque:
				case (uint32_t)E_RenderPass::Outline_Write:
				case (uint32_t)E_RenderPass::Outline_Draw:
				case (uint32_t)E_RenderPass::Debug:
				case (uint32_t)E_RenderPass::Transparent:
				{
					SetOM_RenderTargets({ _EngineSystem.m_hash_RTV_0 }, _EngineSystem.m_hash_DSV_0);
				}break;

				case (uint32_t)E_RenderPass::Shadow:
				{
					SetOM_RenderTargets(std::vector<size_t>(), _EngineSystem.m_hash_DSV_ShadowMap);
				}break;

				case (uint32_t)E_RenderPass::Cubemap:
				{
					//뷰포트 및 렌더타겟 세팅
					SetRS_Viewport(&_EngineSystem.m_vp_CubeMap);
					SetOM_RenderTargets({ _EngineSystem.m_hash_RTV_CubeMap }, _EngineSystem.m_hash_DSV_CubeMap);
				}break;

				case (uint32_t)E_RenderPass::UI:
				{
					//2D객체로 SRV임시체크
					SetOM_RenderTargets({ _EngineSystem.m_hash_RTV_BB }, NULL);
				}break;

				default: break;
			}

			if (prevPass == uint32_t(E_RenderPass::Shadow))
			{
				SetPS_SamplerState(E_SMState::POINT_CLAMP_COMPARISON, 6);	//ShadowMap SamplerState
				SetPS_ShaderResourceView(_EngineSystem.m_hash_DSV_ShadowMap, 6);
			}

			if (prevPass == uint32_t(E_RenderPass::Cubemap))
			{
				//RTV, SRV에 사용하는 버퍼의 밉맵을 형성한다(앨리어싱처리)
				_EngineSystem.GenerateMipMaps(_EngineSystem.m_hash_SRV_CubeMap);
				SetPS_ShaderResourceView(_EngineSystem.m_hash_SRV_CubeMap, 7);
				cb_campos.vPosition = cam_pos_cubemap;
				_EngineSystem.UpdateConstantBuffer(g_hash_cb_campos, &cb_campos);
			}
		}

		if (diff >> 44) // 셰이더가 바뀌었는지 확인 (비트 44~59)
		{
			// VS, PS 등 셰이더 바인딩
			uint32_t IDShaders = (keyCur >> 44) & 0xFFFF;	//16비트
			size_t hashMaterial = _EngineSystem.m_resRP_Shaders[IDShaders];
			Material* pMaterial = _ResourceSystem.GetResource<Material>(hashMaterial);
			SetIA_Topology(pMaterial->GetTopology());
			SetIA_InputLayout(pMaterial->GetIL());
			SetVS_Shader(pMaterial->GetVS());
			SetHS_Shader(pMaterial->GetHS());
			SetDS_Shader(pMaterial->GetDS());
			SetGS_Shader(pMaterial->GetGS());
			if(curPass == (uint32_t)E_RenderPass::Shadow)
				SetPS_Shader(_ResourceSystem.GetResource<Material>(_EngineSystem.m_hash_Mat_ShadowMap)->GetPS());
			else
				SetPS_Shader(pMaterial->GetPS());

			int cnt = 0;
			for (int idxTex = 0; idxTex < (UINT)E_Texture::count; idxTex++)
			{
				for (const auto& hashTx : pMaterial->GetTextures()[idxTex])
				{
					size_t hashSRV = _ResourceSystem.GetResource<Texture>(hashTx)->GetSRV();
					SetPS_ShaderResourceView(hashSRV, cnt++);
				}
			}
		}

		if (diff >> 36) // 상태가 바뀌었는지 확인 (비트 36~43)
		{
			// RS, BS, DSS 바인딩
			uint32_t IDStates = (keyCur >> 36) & 0xFF;		//8비트
			RPStates states = _EngineSystem.m_resRP_States[IDStates];
			SetRS_RasterizerState(states.stateRS);
			SetOM_BlendState(states.stateBS, states.blendFactor, states.blendMask);
			SetOM_DepthStenilState(states.stateDS, states.stencilRef);
		}

		if (diff >> 20)	// 상태가 바뀌었는지 확인 (비트 20~35)
		{
			//버퍼 세팅
			uint32_t IDResources = (keyCur >> 20) & 0xFFFF;		//16비트
			RPMeshCollider MeshCollider = _EngineSystem.m_resRP_Resources[IDResources];

			BaseMesh* pMesh = _ResourceSystem.GetResource<BaseMesh>(MeshCollider.hashMesh);
			SetIA_VertexBuffer(pMesh->GetVB());
			SetIA_IndexBuffer(pMesh->GetIB());

			if (curPass == (uint32_t)E_RenderPass::Debug)
			{
				size_t hash_collider = pMesh->GetCLs()[MeshCollider.idxCollider];
				if (MeshCollider.collider == E_Collider::AABB)
				{
					CB_Debug_Box cb_debug_box;
					_CollisionSystem.SetColliderDebugData(hash_collider, cb_debug_box);
					_EngineSystem.UpdateConstantBuffer(g_hash_cb_debug_box, &cb_debug_box);
			
				}
				else if (MeshCollider.collider == E_Collider::SPHERE)
				{
					CB_Debug_Sphere cb_debug_sphere;
					_CollisionSystem.SetColliderDebugData(hash_collider, cb_debug_sphere);
					_EngineSystem.UpdateConstantBuffer(g_hash_cb_debug_sphere, &cb_debug_sphere);
				}
			}
		}


		// ... 나머지 리소스 바인딩 및 Draw 호출
		if (pArchetype->HasComponents<C_Transform>())
		{
			const auto& transform = pArchetype->GetComponents<C_Transform>(row)[col];
			const Vector3& scale = transform.vScale;
			const Quaternion& quat = transform.qRotate;
			const Vector3& pos = transform.vPosition;

			CB_WVPITMatrix cb_wvpitmat;
			Matrix4x4 matWorld;
			Matrix4x4 matView;
			Matrix4x4 matProj;
			if (pArchetype->HasComponents<T_Render_Billboard>())
			{
				/*
					float dx = c_cam_transform.vPosition.GetX() - position.GetX();
					float dz = c_cam_transform.vPosition.GetZ() - position.GetZ();
					double angle = _RADTODEG(atan2f(dx, dz)) + 180.0f;
				*/
				//yaw빌보드 구현, +z(화면안쪽) 이므로 물체-카메라로 역방향으로 회전해야 같은yaw로 회전해 빌보드로 보이게된다
				Quaternion qRot((pos - cam_pos).Normalize());
				matWorld = GetMat_Scale(scale) * GetMat_RotYaw(qRot.ToRotate().GetY()) * GetMat_Translation(pos);
				matView = cam_matView;
				matProj = cam_matProj;
			}
			else if (pArchetype->HasComponents<T_Render_UI>())
			{
				matWorld = GetMat_ConvertGeometryOrtho() * GetMat_World(scale, quat, pos);
				matView = GetMat_Identity();
				matProj = cam_matOrhto;
			}
			else
			{
				matWorld = GetMat_World(scale, quat, pos);
				matView = cam_matView;
				matProj = cam_matProj;
			}

			if (curPass == (uint32_t)E_RenderPass::Shadow)
			{
				matView = cb_lightMat.matLightView;
				matProj = cb_lightMat.matLightProj;
			}

			cb_wvpitmat.matWorld = matWorld;
			cb_wvpitmat.matView = matView;
			cb_wvpitmat.matProj = matProj;
			cb_wvpitmat.matInvTrans = GetMat_InverseTranspose(matWorld);
			_EngineSystem.UpdateConstantBuffer(g_hash_cb_wvpitmat, &cb_wvpitmat);
		}

		if (pArchetype->HasComponents<C_Animation>())
		{
			const auto& animation = pArchetype->GetComponents<C_Animation>(row)[col];
			_EngineSystem.UpdateConstantBuffer(g_hash_cb_bonemat, (void*)_AnimationSystem.GetAnimbones(animation.hash_animbones).data());
			
		}

		if (curPass != (uint32_t)E_RenderPass::Debug)
			Draw_Indicies(renderCnt, startIdx, 0);
		else
			Draw_Vertices(renderCnt, startIdx);

		//키상태 저장
		keyPrev = keyCur;
	}


//	CB_DirectionalLight cb_directional;
//	{
//		size_t lookup				= _EngineSystem.m_hash_light_directional;
//		const auto& lightDir		= _ECSSystem.GetComponent<C_Light_Direction>(lookup);
//		const auto& c_light			= _ECSSystem.GetComponent<C_Light>(lookup);
//		cb_directional.mAmbient		= c_light.vAmbient;
//		cb_directional.mDiffuse		= c_light.vDiffuse;
//		cb_directional.mSpecular	= c_light.vSpecular;
//		cb_directional.vDirection	= Vector4(Quaternion(lightDir.dir).GetForwardAxis(), c_light.fShiness);
//	}
//	_EngineSystem.UpdateConstantBuffer(g_hash_cb_directionalLight, &cb_directional);
//	SetPS_ConstantBuffer(g_hash_cb_directionalLight, 0);
//
//	CB_PointLight cb_point;
//	{
//		size_t lookup				= _EngineSystem.m_hash_light_point;
//		const auto& c_transform		= _ECSSystem.GetComponent<C_Transform>(lookup);
//		const auto& c_light			= _ECSSystem.GetComponent<C_Light>(lookup);
//		const auto& c_attenuation	= _ECSSystem.GetComponent<C_Light_Attenuation>(lookup);
//		cb_point.mAmbient			= c_light.vAmbient;
//		cb_point.mDiffuse			= c_light.vDiffuse;
//		cb_point.mSpecular			= c_light.vSpecular;
//		cb_point.vPosition			= Vector4(c_transform.vPosition, c_light.fShiness);
//		cb_point.fAttenuations		= Vector4(c_attenuation.fAtt_a0, c_attenuation.fAtt_a1, c_attenuation.fAtt_a2, c_attenuation.fRange);
//	}
//	_EngineSystem.UpdateConstantBuffer(g_hash_cb_pointLight, &cb_point);
//	SetPS_ConstantBuffer(g_hash_cb_pointLight, 1);
//
//	CB_SpotLight cb_spot;
//	{
//		size_t lookup				= _EngineSystem.m_hash_light_spot;
//		const auto& c_transform		= _ECSSystem.GetComponent<C_Transform>(lookup);
//		const auto& c_light			= _ECSSystem.GetComponent<C_Light>(lookup);
//		const auto& c_attenuation	= _ECSSystem.GetComponent<C_Light_Attenuation>(lookup);
//		const auto& c_spot			= _ECSSystem.GetComponent<C_Light_Spot>(lookup);
//		cb_spot.mAmbient			= c_light.vAmbient;
//		cb_spot.mDiffuse			= c_light.vDiffuse;
//		cb_spot.mSpecular			= c_light.vSpecular;
//		cb_spot.vDirection			= c_transform.qRotate.GetForwardAxis();
//		cb_spot.vPosition			= Vector4(c_transform.vPosition, c_light.fShiness);
//		cb_spot.fAttenuations		= Vector4(c_attenuation.fAtt_a0, c_attenuation.fAtt_a1, c_attenuation.fAtt_a2, c_attenuation.fRange);
//		cb_spot.fSpots				= Vector4(c_spot.fSpot, c_spot.fCos_OuterCone, c_spot.fCos_InnerCone, 0.0f);
//	}
//	_EngineSystem.UpdateConstantBuffer(g_hash_cb_spotLight, &cb_spot);
//	SetPS_ConstantBuffer(g_hash_cb_spotLight, 2);
//
//	CB_LightMatrix cb_lightMat;
//	{
//		size_t lookup				= _EngineSystem.m_hash_light_directional;
//		const auto& lightDir		= _ECSSystem.GetComponent<C_Light_Direction>(lookup);
//		const auto& c_cam			= _ECSSystem.GetComponent<C_Camera>(_CameraSystem.lookup_maincam);
//		Vector4 pos					= -lightDir.dir * 5000.0f;
//		cb_lightMat.matLightView	= GetMat_View(pos.ToVector3(), lightDir.dir);
//		cb_lightMat.matLightProj	= GetMat_Orthographic(c_cam.fScreenWidth, c_cam.fScreenHeight, c_cam.fNear, 10000.0f);
//		cb_lightMat.vPos			= pos;
//	}
//	_EngineSystem.UpdateConstantBuffer(g_hash_cb_lightmat, &cb_lightMat);
//	SetVS_ConstantBuffer(g_hash_cb_lightmat, 1);
//	SetGS_ConstantBuffer(g_hash_cb_lightmat, 1);
//
//	size_t lookup_maincam			= _CameraSystem.lookup_maincam;
//	const auto& c_cam_main			= _ECSSystem.GetComponent<C_Camera>(lookup_maincam);
//	const auto& c_cam_proj			= _ECSSystem.GetComponent<C_Projection>(lookup_maincam);
//	const auto& c_cam_ortho			= _ECSSystem.GetComponent<C_Orthographic>(lookup_maincam);
//	const Matrix4x4& cam_matWorld	= c_cam_main.matWorld;
//	const Matrix4x4& cam_matView	= c_cam_main.matView;
//	const Matrix4x4& cam_matProj	= c_cam_proj.matProj;
//	const Matrix4x4& cam_matOrhto	= c_cam_ortho.matOrtho;
//
//	//ShadowMap
//	RenderShadowMap(cb_lightMat.matLightView, cb_lightMat.matLightProj);
//
//#ifdef  _EnviornmentMap
//	//CubeMap
//	RenderCubeMap();
//#endif //  _EnviornmentMap
//
//	const auto& c_cam_transform = _ECSSystem.GetComponent<C_Transform>(lookup_maincam);
//	CB_Campos cb_campos;
//	cb_campos.vPosition = c_cam_transform.vPosition;
//	_EngineSystem.UpdateConstantBuffer(g_hash_cb_campos, &cb_campos);
//	SetVS_ConstantBuffer(g_hash_cb_campos, 5);
//
//	//Render SkySphere
//	RenderSkySphere(cam_matView, cam_matProj);
//	
//	//Render Geometry
//	RenderGeometry(cam_matView, cam_matProj);
//
//	//Render CubeMap
//	//RenderEnviornmentMap(cam_matView, cam_matProj);
//
//	//Render Billboard
//	RenderBillboard(c_cam_transform.vPosition, cam_matView, cam_matProj);
//
//	//Render Pikcing
//	if (_EngineSystem.m_hash_pickingLookup != _HashNotInitialize)
//	{
//		RenderGeometry_PickingOutline(cam_matView, cam_matProj);
//		RenderGeometry_PickingTriangle(cam_matView, cam_matProj);
//	}
//
//	//Render DebugGeometry
//	if (_InputSystem.IsDebugRender())
//		RenderGeometry_Debug(cam_matView, cam_matProj);
//
//	//Render UI
//	RenderUI(cam_matOrhto);
}

void RenderSystem::PostRender()
{
	if (g_fTime_Log >= 1.0f)
		std::cout << "PostRender : " << "EngineSystem" << " Class" << '\n';
	SwapchainPresent(false);
	_EngineSystem.ClearRenderItem();
}

RenderSystem::~RenderSystem()
{

}

/////////////////////////////
//API Usage
/////////////////////////////
void RenderSystem::OnResize(UINT width, UINT height)
{
	if (!_EngineSystem.GetD3DDevice()) return;
	if (width == 0 || height == 0) return;
	g_iWidth = width;
	g_iHeight = height;

	//refCount를 전부 해제시킨다, SwapChain의 ResizeBuffer시 기존버퍼에대한 모든 RefCount가 초기화되어 Comptr객체에서 해제된다
	if (_EngineSystem.GetRTV(_EngineSystem.m_hash_RTV_BB)) _EngineSystem.GetRTV(_EngineSystem.m_hash_RTV_BB)->ReleaseView();
	if (_EngineSystem.GetRTV(_EngineSystem.m_hash_RTV_0)) _EngineSystem.GetRTV(_EngineSystem.m_hash_RTV_0)->ReleaseView();
	if (_EngineSystem.GetSRV(_EngineSystem.m_hash_RTV_0)) _EngineSystem.GetSRV(_EngineSystem.m_hash_RTV_0)->ReleaseView();
	if (_EngineSystem.GetDSV(_EngineSystem.m_hash_DSV_0)) _EngineSystem.GetDSV(_EngineSystem.m_hash_DSV_0)->ReleaseView();
	if (_EngineSystem.GetSRV(_EngineSystem.m_hash_DSV_0)) _EngineSystem.GetSRV(_EngineSystem.m_hash_DSV_0)->ReleaseView();

	_EngineSystem.GetSwapChain()->ResizeBuffers(1, g_iWidth, g_iHeight, DXGI_FORMAT_UNKNOWN, 0);
	//백버퍼
	HRESULT hResult;
	ID3D11Texture2D* pBuffer;
	hResult = _EngineSystem.GetSwapChain()->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBuffer);
	_ASEERTION_CREATE(hResult, "BackBuffer");
	_EngineSystem.m_hash_RTV_BB = _EngineSystem.CreateRenderTargetView(Hasing_wstring(L"RTV_BB"), pBuffer, _BB);
	pBuffer->Release();

	//RTT
	_EngineSystem.m_hash_RTV_0 = _EngineSystem.CreateViews(L"RTV_BB_UI", _Target_ResourceView, g_iWidth, g_iHeight);
	_EngineSystem.m_hash_DSV_0 = _EngineSystem.CreateViews(L"DSV_BB_UI", _Target_DepthView, g_iWidth, g_iHeight);
	SetViewportSize(&_EngineSystem.m_vp_BB, g_iWidth, g_iHeight);
}

void RenderSystem::SetViewportSize(D3D11_VIEWPORT* pViewport, UINT iWidth, UINT iHeight)
{
	pViewport->Width = iWidth;
	pViewport->Height = iHeight;
}

void RenderSystem::ClearRenderTargetView(size_t hashRTV, float r, float g, float b, float a)
{
	auto pView = _EngineSystem.GetRTV(hashRTV);
	_ASEERTION_NULCHK(pView, "NotExist");
	FLOAT clearColor[] = { r, g, b, a };
	_EngineSystem.GetD3DDeviceContext()->ClearRenderTargetView(pView->GetView(), clearColor);
}

void RenderSystem::ClearDepthStencilView(size_t hashDSV)
{
	auto pView = _EngineSystem.GetDSV(hashDSV);
	_ASEERTION_NULCHK(pView, "NotExist");
	_EngineSystem.GetD3DDeviceContext()->ClearDepthStencilView(pView->GetView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void RenderSystem::SetIA_Topology(D3D_PRIMITIVE_TOPOLOGY topology)
{
	_EngineSystem.GetD3DDeviceContext()->IASetPrimitiveTopology(topology);
}

void RenderSystem::SetIA_InputLayout(size_t hashIL)
{
	auto pIL = _EngineSystem.GetIL(hashIL);
	auto pInputLayout = pIL ? pIL->GetInputLayout() : nullptr;
	_EngineSystem.GetD3DDeviceContext()->IASetInputLayout(pInputLayout);
}

void RenderSystem::SetIA_VertexBuffer(size_t hashVB, UINT offset)
{
	auto pBuffer = _EngineSystem.GetVB(hashVB);
	auto ptr = pBuffer ? pBuffer->GetBuffer() : nullptr;
	UINT stride = pBuffer ? pBuffer->GetVertexSize() : 0;
	_EngineSystem.GetD3DDeviceContext()->IASetVertexBuffers(0, 1, &ptr, &stride, &offset);
}

void RenderSystem::SetIA_IndexBuffer(size_t hashIB, UINT offset)
{
	auto pBuffer = _EngineSystem.GetIB(hashIB);
	auto ptr = pBuffer ? pBuffer->GetBuffer() : nullptr;
	_EngineSystem.GetD3DDeviceContext()->IASetIndexBuffer(ptr, DXGI_FORMAT_R32_UINT, offset); //4바이트, 32비트를의미
}

void RenderSystem::SetVS_Shader(size_t hashVS)
{
	auto pShader = _EngineSystem.GetVS(hashVS);
	auto ptr = pShader ? pShader->GetShader() : nullptr;
	_EngineSystem.GetD3DDeviceContext()->VSSetShader(ptr, nullptr, 0);
}

void RenderSystem::SetVS_ShaderResourceView(size_t hashSRV, UINT startIdx)
{
	auto pView = _EngineSystem.GetSRV(hashSRV);
	auto ptr = pView ? pView->GetView() : nullptr;
	int isize = pView ? 1 : 0;
	_EngineSystem.GetD3DDeviceContext()->VSSetShaderResources(startIdx, isize, &ptr);
}

void RenderSystem::SetVS_ConstantBuffer(size_t hashCB, UINT startIdx)
{
	auto pBuffer = _EngineSystem.GetCB(hashCB);
	auto ptr = pBuffer ? pBuffer->GetBuffer() : nullptr;
	int isize = pBuffer ? 1 : 0;
	_EngineSystem.GetD3DDeviceContext()->VSSetConstantBuffers(startIdx, isize, &ptr);
}

void RenderSystem::SetVS_SamplerState(E_SMState eSampler, UINT startIdx)
{
	auto pState = _EngineSystem.GetState_SS()->GetState(eSampler);
	_ASEERTION_NULCHK(pState, "NotExist");
	_EngineSystem.GetD3DDeviceContext()->VSSetSamplers(startIdx, 1, &pState);
}

void RenderSystem::SetHS_Shader(size_t hashHS)
{
	auto pShader = _EngineSystem.GetHS(hashHS);
	auto ptr = pShader ? pShader->GetShader() : nullptr;
	_EngineSystem.GetD3DDeviceContext()->HSSetShader(ptr, nullptr, 0);
}

void RenderSystem::SetHS_ConstantBuffer(size_t hashCB, UINT startIdx)
{
	auto pBuffer = _EngineSystem.GetCB(hashCB);
	auto ptr = pBuffer ? pBuffer->GetBuffer() : nullptr;
	int isize = pBuffer ? 1 : 0;
	_EngineSystem.GetD3DDeviceContext()->HSSetConstantBuffers(startIdx, isize, &ptr);
}

void RenderSystem::SetDS_Shader(size_t hashDS)
{
	auto pShader = _EngineSystem.GetDS(hashDS);
	auto ptr = pShader ? pShader->GetShader() : nullptr;
	_EngineSystem.GetD3DDeviceContext()->DSSetShader(ptr, nullptr, 0);
}

void RenderSystem::SetDS_ConstantBuffer(size_t hashCB, UINT startIdx)
{
	auto pBuffer = _EngineSystem.GetCB(hashCB);
	auto ptr = pBuffer ? pBuffer->GetBuffer() : nullptr;
	int isize = pBuffer ? 1 : 0;
	_EngineSystem.GetD3DDeviceContext()->DSSetConstantBuffers(startIdx, isize, &ptr);
}

void RenderSystem::SetGS_Shader(size_t hashGS)
{
	auto pShader = _EngineSystem.GetGS(hashGS);
	auto ptr = pShader ? pShader->GetShader() : nullptr;
	_EngineSystem.GetD3DDeviceContext()->GSSetShader(ptr, nullptr, 0);
}

void RenderSystem::SetGS_ConstantBuffer(size_t hashCB, UINT startIdx)
{
	auto pBuffer = _EngineSystem.GetCB(hashCB);
	auto ptr = pBuffer ? pBuffer->GetBuffer() : nullptr;
	int isize = pBuffer ? 1 : 0;
	_EngineSystem.GetD3DDeviceContext()->GSSetConstantBuffers(startIdx, isize, &ptr);
}

void RenderSystem::SetPS_SamplerState(E_SMState eSampler, UINT startIdx)
{
	auto pState = _EngineSystem.GetState_SS()->GetState(eSampler);
	_ASEERTION_NULCHK(pState, "NotExist");
	_EngineSystem.GetD3DDeviceContext()->PSSetSamplers(startIdx, 1, &pState);
}

void RenderSystem::SetPS_Shader(size_t hashPS)
{
	auto pShader = _EngineSystem.GetPS(hashPS);
	auto ptr = pShader ? pShader->GetShader() : nullptr;
	_EngineSystem.GetD3DDeviceContext()->PSSetShader(ptr, nullptr, 0);
}

void RenderSystem::SetPS_ShaderResourceView(size_t hashSRV, UINT startIdx)
{
	auto pView = _EngineSystem.GetSRV(hashSRV);
	auto ptr = pView ? pView->GetView() : nullptr;
	int isize = pView ? 1 : 0;
	_EngineSystem.GetD3DDeviceContext()->PSSetShaderResources(startIdx, isize, &ptr);
}

void RenderSystem::SetPS_ConstantBuffer(size_t hashCB, UINT startIdx)
{
	auto pBuffer = _EngineSystem.GetCB(hashCB);
	auto ptr = pBuffer ? pBuffer->GetBuffer() : nullptr;
	int isize = pBuffer ? 1 : 0;
	_EngineSystem.GetD3DDeviceContext()->PSSetConstantBuffers(startIdx, isize, &ptr);
}

void RenderSystem::SetRS_RasterizerState(E_RSState eRSState)
{
	auto pState = _EngineSystem.GetState_RS()->GetState(eRSState);
	_ASEERTION_NULCHK(pState, "NotExist");
	_EngineSystem.GetD3DDeviceContext()->RSSetState(pState);
}

void RenderSystem::SetRS_Viewport(D3D11_VIEWPORT* pViewport)
{
	_EngineSystem.GetD3DDeviceContext()->RSSetViewports(1, pViewport);
}

void RenderSystem::SetOM_RenderTargets(std::vector<size_t> hashRtvs, size_t hashDSV)
{
	std::vector<ID3D11RenderTargetView*> rtvs;
	for (UINT i = 0; i < hashRtvs.size(); i++)
	{
		auto pRTV = _EngineSystem.GetRTV(hashRtvs[i]);
		_ASEERTION_NULCHK(pRTV, "NotExist");
		rtvs.push_back(pRTV->GetView());
	}
	auto pDSV = _EngineSystem.GetDSV(hashDSV);
	auto dsv = pDSV ? pDSV->GetView() : nullptr;
	_EngineSystem.GetD3DDeviceContext()->OMSetRenderTargets(rtvs.size(), rtvs.empty() ? nullptr : rtvs.data(), dsv);
}

void RenderSystem::SetOM_DepthStenilState(E_DSState eDSState, UINT stencilRef)
{
	auto pState = _EngineSystem.GetState_DS()->GetState(eDSState);
	_ASEERTION_NULCHK(pState, "NotExist");
	_EngineSystem.GetD3DDeviceContext()->OMSetDepthStencilState(pState, stencilRef);
}

void RenderSystem::SetOM_BlendState(E_BSState eBSState, const FLOAT* blendFactor, UINT sampleMask)
{
	auto pState = _EngineSystem.GetState_BS()->GetState(eBSState);
	_ASEERTION_NULCHK(pState, "NotExist");
	_EngineSystem.GetD3DDeviceContext()->OMSetBlendState(pState, blendFactor, sampleMask);
}

void RenderSystem::Draw_Vertices(UINT vertexCount, UINT startIdx)
{
	_EngineSystem.GetD3DDeviceContext()->Draw(vertexCount, startIdx);
}

void RenderSystem::Draw_Indicies(UINT indexCount, UINT startIdx, INT vertexOffset)
{
	_EngineSystem.GetD3DDeviceContext()->DrawIndexed(indexCount, startIdx, vertexOffset);
}

void RenderSystem::SwapchainPresent(bool vsync)
{
	_EngineSystem.GetSwapChain()->Present(vsync, NULL);
}

/*
*******************************
* RenderPasses
*******************************
*/
size_t RenderSystem::GetHashMat_Debug(E_Collider eType)
{
	switch (eType)
	{

		case E_Collider::AABB :
		case E_Collider::OBB:
			return _EngineSystem.m_hash_Mat_Debug_Box;

		case E_Collider::SPHERE:
			return _EngineSystem.m_hash_Mat_Debug_Sphere;

		default:
			return _EngineSystem.m_hash_Mat_Debug_Box;
	}
}

size_t RenderSystem::GetHashMat_Cubemap(E_VerticesType eType)
{
	switch (eType)
	{
		case E_VerticesType::Vertex_PTN:
			return _EngineSystem.m_hash_Mat_CubeMap_PTN;
		case E_VerticesType::Vertex_PTN_Skinned:
			return _EngineSystem.m_hash_Mat_CubeMap_PTN_Skinned;
		case E_VerticesType::Vertex_PTNTB:
			return _EngineSystem.m_hash_Mat_CubeMap_PTNTB;
		case E_VerticesType::Vertex_PTNTB_Skinned:
			return _EngineSystem.m_hash_Mat_CubeMap_PTNTB_Skinned;
		default:
			return _EngineSystem.m_hash_Mat_CubeMap_PTN;
	}
}

size_t RenderSystem::GetHashMat_Outline(E_VerticesType eType)
{
	switch (eType)
	{
		case E_VerticesType::Vertex_PTN:
			return _EngineSystem.m_hash_Mat_Outline_PTN;
		case E_VerticesType::Vertex_PTN_Skinned:
			return _EngineSystem.m_hash_Mat_Outline_PTN_Skinned;
		case E_VerticesType::Vertex_PTNTB:
			return _EngineSystem.m_hash_Mat_Outline_PTNTB;
		case E_VerticesType::Vertex_PTNTB_Skinned:
			return _EngineSystem.m_hash_Mat_Outline_PTNTB_Skinned;
		default:
			return _EngineSystem.m_hash_Mat_Outline_PTN;
	}
}

void RenderSystem::CollectRenderItem(const Vector3& posCam)
{
	ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Transform, C_Render>();
	std::vector<Archetype*> queries = _ECSSystem.QueryArchetypes(key);
	for (auto& archetype : queries)
	{
		size_t st_row = 0;
		size_t st_col = 0;
		for (size_t row = st_row; row < archetype->GetCount_Chunks(); row++)
		{
			auto& transforms = archetype->GetComponents<C_Transform>(row);
			auto& renders = archetype->GetComponents<C_Render>(row);
			std::vector<C_Collider>* pColliders = nullptr;
			if(archetype->HasComponents<C_Collider>())
				pColliders = &archetype->GetComponents<C_Collider>(row);
			for (size_t col = st_col; col < archetype->GetCount_Chunk(row); col++)
			{
				if (!renders[col].bRenderable) continue;
				size_t passMasks = renders[col].passMasks;

				float distToCam = (transforms[col].vPosition - posCam).Length();
				uint32_t hashDist = _EngineSystem.GetRenderPassKey_DistToCamera(distToCam);

				const auto& pRenderAsset = _ResourceSystem.GetResource<RenderAsset>(renders[col].hash_asset_Render);
				size_t hashMesh = pRenderAsset->m_hMeshMats.hash_mesh;
				BaseMesh* pMesh = _ResourceSystem.GetResource<BaseMesh>(hashMesh);

				E_Collider eCollider = pColliders ? (*pColliders)[col].type : E_Collider::AABB;

				//Collect Draw MeshMaterial
				for (UINT matIdx = 0; matIdx < pRenderAsset->m_hMeshMats.hash_mats.size(); matIdx++)
				{
					size_t hashMaterial = pRenderAsset->m_hMeshMats.hash_mats[matIdx];
					const auto& pMaterial = _ResourceSystem.GetResource<Material>(hashMaterial);
					if (!(passMasks & (1 << pMaterial->GetHashPass()))) continue;
					uint32_t hashPass = pMaterial->GetHashPass();
					uint32_t hashShader = pMaterial->GetHashShaders();
					uint32_t hashStates = pMaterial->GetHashStates();
					uint32_t hashResources = _EngineSystem.GetRenderPassKey_Resources(hashMesh, eCollider);
					_EngineSystem.EnqueueRenderItem(_EngineSystem.GenerateRenderPassHash(hashPass, hashShader, hashStates, hashResources, hashDist), archetype, row, col, pMesh->GetRendIndices()[matIdx].count, pMesh->GetRendIndices()[matIdx].idx);
				}

				//Collect Draw Shadow
				if (passMasks & E_RenderPass::Shadow)
				{
					const auto& shadowMaterial = _ResourceSystem.GetResource<Material>(_EngineSystem.m_hash_Mat_ShadowMap);
					uint32_t hashPassShadow = shadowMaterial->GetHashPass();
					for (UINT matIdx = 0; matIdx < pRenderAsset->m_hMeshMats.hash_mats.size(); matIdx++)
					{
						size_t hashMaterial = pRenderAsset->m_hMeshMats.hash_mats[matIdx];
						const auto& pMaterial = _ResourceSystem.GetResource<Material>(hashMaterial);
						uint32_t hashPass = hashPassShadow;
						uint32_t hashShader = pMaterial->GetHashShaders();
						uint32_t hashStates = pMaterial->GetHashStates();
						uint32_t hashResources = _EngineSystem.GetRenderPassKey_Resources(hashMesh, eCollider);
						_EngineSystem.EnqueueRenderItem(_EngineSystem.GenerateRenderPassHash(hashPass, hashShader, hashStates, hashResources, hashDist), archetype, row, col, pMesh->GetRendIndices()[matIdx].count, pMesh->GetRendIndices()[matIdx].idx);
					}
				}

				//Collect Draw Cubemap
				if (passMasks & E_RenderPass::Cubemap)
				{
					size_t hashMateiral = GetHashMat_Cubemap(pMesh->GetVerticesType());
					const auto& pMaterial = _ResourceSystem.GetResource<Material>(hashMateiral);
					uint32_t hashPass = pMaterial->GetHashPass();
					uint32_t hashShader = pMaterial->GetHashShaders();
					uint32_t hashStates = pMaterial->GetHashStates();
					uint32_t hashResources = _EngineSystem.GetRenderPassKey_Resources(hashMesh, eCollider);
					for (UINT matIdx = 0; matIdx < pRenderAsset->m_hMeshMats.hash_mats.size(); matIdx++)
						_EngineSystem.EnqueueRenderItem(_EngineSystem.GenerateRenderPassHash(hashPass, hashShader, hashStates, hashResources, hashDist), archetype, row, col, pMesh->GetRendIndices()[matIdx].count, pMesh->GetRendIndices()[matIdx].idx);
				}

				//Collect Draw Debug
				if (_InputSystem.IsDebugRender() && (passMasks & E_RenderPass::Debug))
				{
					size_t hashMateiral = GetHashMat_Debug(eCollider);
					const auto& pMaterial = _ResourceSystem.GetResource<Material>(hashMateiral);
					uint32_t hashPass = pMaterial->GetHashPass();
					uint32_t hashShader = pMaterial->GetHashShaders();
					uint32_t hashStates = pMaterial->GetHashStates();
					for (UINT colliderIdx = 0; colliderIdx < pMesh->GetCLs().size(); colliderIdx++)
					{
						uint32_t hashResources = _EngineSystem.GetRenderPassKey_Resources(hashMesh, eCollider, colliderIdx);
						UINT renderCnt = (eCollider == E_Collider::AABB) ? 1 : 60;
						_EngineSystem.EnqueueRenderItem(_EngineSystem.GenerateRenderPassHash(hashPass, hashShader, hashStates, hashResources, hashDist), archetype, row, col, renderCnt, 0);
					}
				}
			}
			st_col = 0;
		}
	}

	if (_EngineSystem.m_hash_pickingLookup != _HashNotInitialize)
	{
		size_t lookup			= _EngineSystem.m_hash_pickingLookup;
		const auto& info		= _ECSSystem.GetComponent<C_Info>(lookup);
		const auto& transform	= _ECSSystem.GetComponent<C_Transform>(lookup);
		const auto& render		= _ECSSystem.GetComponent<C_Render>(lookup);
		const auto& collider	= _ECSSystem.GetComponent<C_Collider>(lookup);
		const auto& entity		= _ECSSystem.GetEntity(lookup);
		Archetype* archetype	= _ECSSystem.QueryArchetype(entity->m_Key);
		size_t row				= entity->m_IdxRow;
		size_t col				= entity->m_IdxCol;

		if (!render.bRenderable) return;
		float distToCam = (transform.vPosition - posCam).Length();
		uint32_t hashDist = _EngineSystem.GetRenderPassKey_DistToCamera(distToCam);

		const auto& pRenderAsset = _ResourceSystem.GetResource<RenderAsset>(render.hash_asset_Render);
		size_t hashMesh = pRenderAsset->m_hMeshMats.hash_mesh;
		BaseMesh* pMesh = _ResourceSystem.GetResource<BaseMesh>(hashMesh);

		E_Collider eCollider = collider.type;

		//Collect Picking Triangle
		{

		}

		//Collect Write Outline
		{
			for (UINT matIdx = 0; matIdx < pRenderAsset->m_hMeshMats.hash_mats.size(); matIdx++)
			{
				size_t hashMaterial = pRenderAsset->m_hMeshMats.hash_mats[matIdx];
				const auto& pMaterial = _ResourceSystem.GetResource<Material>(hashMaterial);
				uint32_t hashPass = pMaterial->GetHashPass();
				uint32_t hashShader = pMaterial->GetHashShaders();
				uint32_t hashStates = _EngineSystem.GetRenderPassKey_States(E_RSState::SOLID_CULLBACK_CW, E_DSState::Outline_Write, E_BSState::Outline_Write, 1);
				uint32_t hashResources = _EngineSystem.GetRenderPassKey_Resources(hashMesh, eCollider);
				_EngineSystem.EnqueueRenderItem(_EngineSystem.GenerateRenderPassHash(hashPass, hashShader, hashStates, hashResources, hashDist), archetype, row, col, pMesh->GetRendIndices()[matIdx].count, pMesh->GetRendIndices()[matIdx].idx);
			}
		}

		//Collect Draw Outline
		{
			size_t hashMateiral = GetHashMat_Outline(pMesh->GetVerticesType());
			const auto& pMaterial = _ResourceSystem.GetResource<Material>(hashMateiral);
			uint32_t hashPass = pMaterial->GetHashPass();
			uint32_t hashShader = pMaterial->GetHashShaders();
			uint32_t hashStates = pMaterial->GetHashStates();
			uint32_t hashResources = _EngineSystem.GetRenderPassKey_Resources(hashMesh, eCollider);
			for (UINT matIdx = 0; matIdx < pRenderAsset->m_hMeshMats.hash_mats.size(); matIdx++)
				_EngineSystem.EnqueueRenderItem(_EngineSystem.GenerateRenderPassHash(hashPass, hashShader, hashStates, hashResources, hashDist), archetype, row, col, pMesh->GetRendIndices()[matIdx].count, pMesh->GetRendIndices()[matIdx].idx);
		}
	}
}

//void RenderSystem::RenderSkySphere(const Matrix4x4& matView, const Matrix4x4& matProj)
//{
//	SetOM_BlendState(E_BSState::Opaque, NULL);
//	SetOM_DepthStenilState(E_DSState::SkyBox);
//	SetPS_SamplerState(E_SMState::LINEAR_WRAP);
//	SetRS_RasterizerState(E_RSState::SOLID_CULLBACK_CCW);
//	ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Transform, C_Render, T_Render_Sky>();
//	std::vector<Archetype*> queries = _ECSSystem.QueryArchetypes(key);
//	for (auto& archetype : queries)
//	{
//		size_t st_row = 0;
//		size_t st_col = 0;
//		for (size_t row = st_row; row < archetype->GetCount_Chunks(); row++)
//		{
//			auto& transforms = archetype->GetComponents<C_Transform>(row);
//			auto& renders = archetype->GetComponents<C_Render>(row);
//			for (size_t col = st_col; col < archetype->GetCount_Chunk(row); col++)
//			{
//				const Vector3& scale = transforms[col].vScale;
//				const Quaternion& rotate = transforms[col].qRotate;
//				const Vector3& position = transforms[col].vPosition;
//				CB_WVPITMatrix cb_wvpitmat;
//				cb_wvpitmat.matWorld = GetMat_World(scale, rotate, position);
//				cb_wvpitmat.matView = matView;
//				cb_wvpitmat.matProj = matProj;
//				cb_wvpitmat.matInvTrans = GetMat_InverseTranspose(cb_wvpitmat.matWorld);
//				_EngineSystem.UpdateConstantBuffer(g_hash_cb_wvpitmat, &cb_wvpitmat);
//				SetVS_ConstantBuffer(g_hash_cb_wvpitmat, 0);
//
//				RenderAsset* pRenderAsset = _ResourceSystem.GetResource<RenderAsset>(renders[col].hash_asset_Render);
//				BaseMesh* pMesh = _ResourceSystem.GetResource<BaseMesh>(pRenderAsset->m_hMeshMats.hash_mesh);
//				SetIA_VertexBuffer(pMesh->GetVB());
//				SetIA_IndexBuffer(pMesh->GetIB());
//
//				for (UINT j = 0; j < pRenderAsset->m_hMeshMats.hash_mats.size(); j++)
//				{
//					Material* pMaterial = _ResourceSystem.GetResource<Material>(pRenderAsset->m_hMeshMats.hash_mats[j]);
//					SetIA_InputLayout(pMaterial->GetIL());
//					SetVS_Shader(pMaterial->GetVS());
//					SetGS_Shader(pMaterial->GetGS());
//					SetPS_Shader(pMaterial->GetPS());
//
//					const std::vector<size_t>* texs = pMaterial->GetTextures();
//					int cnt = 0;
//					for (int idxTex = 0; idxTex < (UINT)E_Texture::count; idxTex++)
//					{
//						for (const auto& hashTx : texs[idxTex])
//						{
//							size_t hashSRV = _ResourceSystem.GetResource<Texture>(hashTx)->GetSRV();
//							SetPS_ShaderResourceView(hashSRV, cnt++);
//						}
//					}
//					Draw_Indicies(pMesh->GetRendIndices()[j].count, pMesh->GetRendIndices()[j].idx, 0);
//				}
//			}
//			st_col = 0;
//		}
//	}
//}
//
//void RenderSystem::RenderGeometry(const Matrix4x4& matView, const Matrix4x4& matProj)
//{
//	SetOM_BlendState(E_BSState::Opaque, NULL);
//	SetOM_DepthStenilState(E_DSState::Default);
//	SetPS_SamplerState(E_SMState::LINEAR_WRAP);
//	SetPS_SamplerState(E_SMState::POINT_CLAMP_COMPARISON, 6);
//	SetRS_RasterizerState(E_RSState::SOLID_CULLBACK_CW);
//	
//	//static
//	{
//		ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Transform, C_Render, T_Render_Geometry_Static>();
//		std::vector<Archetype*> queries = _ECSSystem.QueryArchetypes(key);
//		for (auto& archetype : queries)
//		{
//			size_t st_row = 0;
//			size_t st_col = 0;
//			for (size_t row = st_row; row < archetype->GetCount_Chunks(); row++)
//			{
//				auto& transforms = archetype->GetComponents<C_Transform>(row);
//				auto& renders = archetype->GetComponents<C_Render>(row);
//				for (size_t col = st_col; col < archetype->GetCount_Chunk(row); col++)
//				{
//					if (!renders[col].bRenderable) continue;
//					const Vector3& scale = transforms[col].vScale;
//					const Quaternion& rotate = transforms[col].qRotate;
//					const Vector3& position = transforms[col].vPosition;
//					CB_WVPITMatrix cb_wvpitmat;
//					cb_wvpitmat.matWorld = GetMat_World(scale, rotate, position);
//					cb_wvpitmat.matView = matView;
//					cb_wvpitmat.matProj = matProj;
//					cb_wvpitmat.matInvTrans = GetMat_InverseTranspose(cb_wvpitmat.matWorld);
//					_EngineSystem.UpdateConstantBuffer(g_hash_cb_wvpitmat, &cb_wvpitmat);
//					SetVS_ConstantBuffer(g_hash_cb_wvpitmat, 0);
//
//					const auto& MeshMats = _ResourceSystem.GetResource<RenderAsset>(renders[col].hash_asset_Render)->m_hMeshMats;
//					BaseMesh* pMesh = _ResourceSystem.GetResource<BaseMesh>(MeshMats.hash_mesh);
//					SetIA_VertexBuffer(pMesh->GetVB());
//					SetIA_IndexBuffer(pMesh->GetIB());
//					for (UINT j = 0; j < MeshMats.hash_mats.size(); j++)
//					{
//						auto& hashMaterial = MeshMats.hash_mats[j];
//						Material* pMaterial = _ResourceSystem.GetResource<Material>(hashMaterial);
//						SetIA_InputLayout(pMaterial->GetIL());
//						SetVS_Shader(pMaterial->GetVS());
//						SetGS_Shader(pMaterial->GetGS());
//						SetPS_Shader(pMaterial->GetPS());
//
//						const std::vector<size_t>* texs = pMaterial->GetTextures();
//						int cnt = 0;
//						for (int idxTex = 0; idxTex < (UINT)E_Texture::count; idxTex++)
//						{
//							for (const auto& hashTx : texs[idxTex])
//							{
//								size_t hashSRV = _ResourceSystem.GetResource<Texture>(hashTx)->GetSRV();
//								SetPS_ShaderResourceView(hashSRV, cnt++);
//							}
//						}
//						Draw_Indicies(pMesh->GetRendIndices()[j].count, pMesh->GetRendIndices()[j].idx, 0);
//					}
//				}
//				st_col = 0;
//			}
//		}
//	}
//
//	//Skeletal
//	{
//		ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Transform, C_Render, C_Animation, T_Render_Geometry_Skeletal>();
//		std::vector<Archetype*> queries = _ECSSystem.QueryArchetypes(key);
//		for (auto& archetype : queries)
//		{
//			size_t st_row = 0;
//			size_t st_col = 0;
//			for (size_t row = st_row; row < archetype->GetCount_Chunks(); row++)
//			{
//				auto& transforms = archetype->GetComponents<C_Transform>(row);
//				auto& renders = archetype->GetComponents<C_Render>(row);
//				auto& animations = archetype->GetComponents<C_Animation>(row);
//				for (size_t col = st_col; col < archetype->GetCount_Chunk(row); col++)
//				{
//					if (!renders[col].bRenderable) continue;
//					const Vector3& scale = transforms[col].vScale;
//					const Quaternion& rotate = transforms[col].qRotate;
//					const Vector3& position = transforms[col].vPosition;
//
//					CB_WVPITMatrix cb_wvpitmat;
//					cb_wvpitmat.matWorld = GetMat_World(scale, rotate, position);
//					cb_wvpitmat.matView = matView;
//					cb_wvpitmat.matProj = matProj;
//					cb_wvpitmat.matInvTrans = GetMat_InverseTranspose(cb_wvpitmat.matWorld);
//					_EngineSystem.UpdateConstantBuffer(g_hash_cb_wvpitmat, &cb_wvpitmat);
//					SetVS_ConstantBuffer(g_hash_cb_wvpitmat, 0);
//
//					_EngineSystem.UpdateConstantBuffer(g_hash_cb_bonemat, (void*)_AnimationSystem.GetAnimbones(animations[col].hash_animbones).data());
//					SetVS_ConstantBuffer(g_hash_cb_bonemat, 2);
//
//					const auto& MeshMats = _ResourceSystem.GetResource<RenderAsset>(renders[col].hash_asset_Render)->m_hMeshMats;
//					BaseMesh* pMesh = _ResourceSystem.GetResource<BaseMesh>(MeshMats.hash_mesh);
//					SetIA_VertexBuffer(pMesh->GetVB());
//					SetIA_IndexBuffer(pMesh->GetIB());
//					for (UINT j = 0; j < MeshMats.hash_mats.size(); j++)
//					{
//						auto& hashMaterial = MeshMats.hash_mats[j];
//						Material* pMaterial = _ResourceSystem.GetResource<Material>(hashMaterial);
//						SetIA_InputLayout(pMaterial->GetIL());
//						SetVS_Shader(pMaterial->GetVS());
//						SetGS_Shader(pMaterial->GetGS());
//						SetPS_Shader(pMaterial->GetPS());
//
//						const std::vector<size_t>* texs = pMaterial->GetTextures();
//						int cnt = 0;
//						for (int idxTex = 0; idxTex < (UINT)E_Texture::count; idxTex++)
//						{
//							for (const auto& hashTx : texs[idxTex])
//							{
//								size_t hashSRV = _ResourceSystem.GetResource<Texture>(hashTx)->GetSRV();
//								SetPS_ShaderResourceView(hashSRV, cnt++);
//							}
//						}
//						Draw_Indicies(pMesh->GetRendIndices()[j].count, pMesh->GetRendIndices()[j].idx, 0);
//					}
//				}
//				st_col = 0;
//			}
//		}
//	}
//}
//
//void RenderSystem::RenderBillboard(const Vector3& campos, const Matrix4x4& matView, const Matrix4x4& matProj)
//{
//	SetOM_BlendState(E_BSState::Opaque, NULL);
//	SetOM_DepthStenilState(E_DSState::Default);
//	SetPS_SamplerState(E_SMState::LINEAR_WRAP);
//	SetRS_RasterizerState(E_RSState::SOLID_CULLBACK_CW);
//	ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Transform, C_Render, T_Render_Billboard>();
//	std::vector<Archetype*> queries = _ECSSystem.QueryArchetypes(key);
//	for (auto& archetype : queries)
//	{
//		size_t st_row = 0;
//		size_t st_col = 0;
//		for (size_t row = st_row; row < archetype->GetCount_Chunks(); row++)
//		{
//			auto& transforms = archetype->GetComponents<C_Transform>(row);
//			auto& renders = archetype->GetComponents<C_Render>(row);
//			for (size_t col = st_col; col < archetype->GetCount_Chunk(row); col++)
//			{
//				if (!renders[col].bRenderable) continue;
//				const Vector3& scale = transforms[col].vScale;
//				const Quaternion& rotate = transforms[col].qRotate;
//				const Vector3& position = transforms[col].vPosition;
//				CB_WVPITMatrix cb_wvpitmat;
//				/*
//				float dx = c_cam_transform.vPosition.GetX() - position.GetX();
//				float dz = c_cam_transform.vPosition.GetZ() - position.GetZ();
//				double angle = _RADTODEG(atan2f(dx, dz)) + 180.0f;
//				*/
//				//yaw빌보드 구현, +z(화면안쪽) 이므로 물체-카메라로 역방향으로 회전해야 같은yaw로 회전해 빌보드로 보이게된다
//
//				Quaternion qRot((position - campos).Normalize());
//				cb_wvpitmat.matWorld = GetMat_Scale(scale) * GetMat_RotYaw(qRot.ToRotate().GetY()) * GetMat_Translation(position);
//				cb_wvpitmat.matView = matView;
//				cb_wvpitmat.matProj = matProj;
//				cb_wvpitmat.matInvTrans = GetMat_InverseTranspose(cb_wvpitmat.matWorld);
//				_EngineSystem.UpdateConstantBuffer(g_hash_cb_wvpitmat, &cb_wvpitmat);
//				SetVS_ConstantBuffer(g_hash_cb_wvpitmat, 0);
//
//				const auto& MeshMats = _ResourceSystem.GetResource<RenderAsset>(renders[col].hash_asset_Render)->m_hMeshMats;
//				BaseMesh* pMesh = _ResourceSystem.GetResource<BaseMesh>(MeshMats.hash_mesh);
//				SetIA_VertexBuffer(pMesh->GetVB());
//				SetIA_IndexBuffer(pMesh->GetIB());
//				for (UINT j = 0; j < MeshMats.hash_mats.size(); j++)
//				{
//					auto& hashMaterial = MeshMats.hash_mats[j];
//					Material* pMaterial = _ResourceSystem.GetResource<Material>(hashMaterial);
//					SetIA_InputLayout(pMaterial->GetIL());
//					SetVS_Shader(pMaterial->GetVS());
//					SetGS_Shader(pMaterial->GetGS());
//					SetPS_Shader(pMaterial->GetPS());
//
//					const std::vector<size_t>* texs = pMaterial->GetTextures();
//					int cnt = 0;
//					for (int idxTex = 0; idxTex < (UINT)E_Texture::count; idxTex++)
//					{
//						for (const auto& hashTx : texs[idxTex])
//						{
//							size_t hashSRV = _ResourceSystem.GetResource<Texture>(hashTx)->GetSRV();
//							SetPS_ShaderResourceView(hashSRV, cnt++);
//						}
//					}
//					Draw_Indicies(pMesh->GetRendIndices()[j].count, pMesh->GetRendIndices()[j].idx, 0);
//				}
//			}
//			st_col = 0;
//		}
//	}
//}
//
//void RenderSystem::RenderShadowMap(const Matrix4x4& matView, const Matrix4x4& matProj)
//{
//	SetOM_BlendState(E_BSState::Opaque, NULL);
//	SetOM_DepthStenilState(E_DSState::Default);
//	SetPS_SamplerState(E_SMState::LINEAR_WRAP);
//	SetRS_RasterizerState(E_RSState::SOLID_CULLBACK_CW);
//
//	//그림자맵 텍스쳐를 구성하기위한 타겟(rtv null, dsv만) 세팅
//	ClearDepthStencilView(_EngineSystem.m_hash_DSV_ShadowMap);
//	SetOM_RenderTargets(std::vector<size_t>(), _EngineSystem.m_hash_DSV_ShadowMap);
//
//	//Render Geometry
//	{
//		ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Transform, C_Render, T_Render_Geometry_Static>();
//		std::vector<Archetype*> queries = _ECSSystem.QueryArchetypes(key);
//		for (auto& archetype : queries)
//		{
//			size_t st_row = 0;
//			size_t st_col = 0;
//			for (size_t row = st_row; row < archetype->GetCount_Chunks(); row++)
//			{
//				auto& transforms = archetype->GetComponents<C_Transform>(row);
//				auto& renders = archetype->GetComponents<C_Render>(row);
//				for (size_t col = st_col; col < archetype->GetCount_Chunk(row); col++)
//				{
//					if (!renders[col].bRenderable) continue;
//					const Vector3& scale = transforms[col].vScale;
//					const Quaternion& rotate = transforms[col].qRotate;
//					const Vector3& position = transforms[col].vPosition;
//					CB_WVPITMatrix cb_wvpitmat;
//					cb_wvpitmat.matWorld = GetMat_World(scale, rotate, position);
//					cb_wvpitmat.matView = matView;
//					cb_wvpitmat.matProj = matProj;
//					cb_wvpitmat.matInvTrans = GetMat_InverseTranspose(cb_wvpitmat.matWorld);
//					_EngineSystem.UpdateConstantBuffer(g_hash_cb_wvpitmat, &cb_wvpitmat);
//					SetVS_ConstantBuffer(g_hash_cb_wvpitmat, 0);
//
//					const auto& MeshMats = _ResourceSystem.GetResource<RenderAsset>(renders[col].hash_asset_Render)->m_hMeshMats;
//					BaseMesh* pMesh = _ResourceSystem.GetResource<BaseMesh>(MeshMats.hash_mesh);
//					SetIA_VertexBuffer(pMesh->GetVB());
//					SetIA_IndexBuffer(pMesh->GetIB());
//					for (UINT j = 0; j < MeshMats.hash_mats.size(); j++)
//					{
//						auto& hashMaterial = MeshMats.hash_mats[j];
//						Material* pMaterial = _ResourceSystem.GetResource<Material>(hashMaterial);
//						SetIA_InputLayout(pMaterial->GetIL());
//						SetVS_Shader(pMaterial->GetVS());
//
//						Material* pShadowMapMaterial = _ResourceSystem.GetResource<Material>(_EngineSystem.m_hash_Mat_ShadowMap);
//						SetPS_Shader(pShadowMapMaterial->GetPS());
//
//						Draw_Indicies(pMesh->GetRendIndices()[j].count, pMesh->GetRendIndices()[j].idx, 0);
//					}
//				}
//				st_col = 0;
//			}
//		}
//	}
//
//	//Skeletal
//	{
//		ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Transform, C_Render, C_Animation, T_Render_Geometry_Skeletal>();
//		std::vector<Archetype*> queries = _ECSSystem.QueryArchetypes(key);
//		for (auto& archetype : queries)
//		{
//			size_t st_row = 0;
//			size_t st_col = 0;
//			for (size_t row = st_row; row < archetype->GetCount_Chunks(); row++)
//			{
//				auto& transforms = archetype->GetComponents<C_Transform>(row);
//				auto& renders = archetype->GetComponents<C_Render>(row);
//				auto& animations = archetype->GetComponents<C_Animation>(row);
//				for (size_t col = st_col; col < archetype->GetCount_Chunk(row); col++)
//				{
//					if (!renders[col].bRenderable) continue;
//					const Vector3& scale = transforms[col].vScale;
//					const Quaternion& rotate = transforms[col].qRotate;
//					const Vector3& position = transforms[col].vPosition;
//
//					CB_WVPITMatrix cb_wvpitmat;
//					cb_wvpitmat.matWorld = GetMat_World(scale, rotate, position);
//					cb_wvpitmat.matView = matView;
//					cb_wvpitmat.matProj = matProj;
//					cb_wvpitmat.matInvTrans = GetMat_InverseTranspose(cb_wvpitmat.matWorld);
//					_EngineSystem.UpdateConstantBuffer(g_hash_cb_wvpitmat, &cb_wvpitmat);
//					SetVS_ConstantBuffer(g_hash_cb_wvpitmat, 0);
//					
//					_EngineSystem.UpdateConstantBuffer(g_hash_cb_bonemat, (void*)_AnimationSystem.GetAnimbones(animations[col].hash_animbones).data());
//					SetVS_ConstantBuffer(g_hash_cb_bonemat, 2);
//
//					const auto& MeshMats = _ResourceSystem.GetResource<RenderAsset>(renders[col].hash_asset_Render)->m_hMeshMats;
//					BaseMesh* pMesh = _ResourceSystem.GetResource<BaseMesh>(MeshMats.hash_mesh);
//					SetIA_VertexBuffer(pMesh->GetVB());
//					SetIA_IndexBuffer(pMesh->GetIB());
//					for (UINT j = 0; j < MeshMats.hash_mats.size(); j++)
//					{
//						auto& hashMaterial = MeshMats.hash_mats[j];
//						Material* pMaterial = _ResourceSystem.GetResource<Material>(hashMaterial);
//						SetIA_InputLayout(pMaterial->GetIL());
//						SetVS_Shader(pMaterial->GetVS());
//
//						Material* pShadowMapMaterial = _ResourceSystem.GetResource<Material>(_EngineSystem.m_hash_Mat_ShadowMap);
//						SetPS_Shader(pShadowMapMaterial->GetPS());
//
//						const std::vector<size_t>* texs = pMaterial->GetTextures();
//						int cnt = 0;
//						for (int idxTex = 0; idxTex < (UINT)E_Texture::count; idxTex++)
//						{
//							for (const auto& hashTx : texs[idxTex])
//							{
//								size_t hashSRV = _ResourceSystem.GetResource<Texture>(hashTx)->GetSRV();
//								SetPS_ShaderResourceView(hashSRV, cnt++);
//							}
//						}
//						Draw_Indicies(pMesh->GetRendIndices()[j].count, pMesh->GetRendIndices()[j].idx, 0);
//					}
//				}
//				st_col = 0;
//			}
//		}
//	}
//
//	const auto& c_cam_transform = _ECSSystem.GetComponent<C_Transform>(_CameraSystem.lookup_maincam);
//	//Render Billboard
//	{
//		ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Transform, C_Render, T_Render_Billboard>();
//		std::vector<Archetype*> queries = _ECSSystem.QueryArchetypes(key);
//		for (auto& archetype : queries)
//		{
//			size_t st_row = 0;
//			size_t st_col = 0;
//			for (size_t row = st_row; row < archetype->GetCount_Chunks(); row++)
//			{
//				auto& transforms = archetype->GetComponents<C_Transform>(row);
//				auto& renders = archetype->GetComponents<C_Render>(row);
//				for (size_t col = st_col; col < archetype->GetCount_Chunk(row); col++)
//				{
//					if (!renders[col].bRenderable) continue;
//					const Vector3& scale = transforms[col].vScale;
//					const Quaternion& rotate = transforms[col].qRotate;
//					const Vector3& position = transforms[col].vPosition;
//					CB_WVPITMatrix cb_wvpitmat;
//					/*
//					float dx = c_cam_transform.vPosition.GetX() - position.GetX();
//					float dz = c_cam_transform.vPosition.GetZ() - position.GetZ();
//					double angle = _RADTODEG(atan2f(dx, dz)) + 180.0f;
//					*/
//					//yaw빌보드 구현, +z(화면안쪽) 이므로 물체-카메라로 역방향으로 회전해야 같은yaw로 회전해 빌보드로 보이게된다
//
//					Quaternion qRot((position - c_cam_transform.vPosition).Normalize());
//					cb_wvpitmat.matWorld = GetMat_Scale(scale) * GetMat_RotYaw(qRot.ToRotate().GetY()) * GetMat_Translation(position);
//					cb_wvpitmat.matView = matView;
//					cb_wvpitmat.matProj = matProj;
//					cb_wvpitmat.matInvTrans = GetMat_InverseTranspose(cb_wvpitmat.matWorld);
//					_EngineSystem.UpdateConstantBuffer(g_hash_cb_wvpitmat, &cb_wvpitmat);
//					SetVS_ConstantBuffer(g_hash_cb_wvpitmat, 0);
//
//
//					const auto& MeshMats = _ResourceSystem.GetResource<RenderAsset>(renders[col].hash_asset_Render)->m_hMeshMats;
//					BaseMesh* pMesh = _ResourceSystem.GetResource<BaseMesh>(MeshMats.hash_mesh);
//					SetIA_VertexBuffer(pMesh->GetVB());
//					SetIA_IndexBuffer(pMesh->GetIB());
//					for (UINT j = 0; j < MeshMats.hash_mats.size(); j++)
//					{
//						auto& hashMaterial = MeshMats.hash_mats[j];
//						Material* pMaterial = _ResourceSystem.GetResource<Material>(hashMaterial);
//						SetIA_InputLayout(pMaterial->GetIL());
//						SetVS_Shader(pMaterial->GetVS());
//
//						Material* pShadowMapMaterial = _ResourceSystem.GetResource<Material>(_EngineSystem.m_hash_Mat_ShadowMap);
//						SetPS_Shader(pShadowMapMaterial->GetPS());
//
//						Draw_Indicies(pMesh->GetRendIndices()[j].count, pMesh->GetRendIndices()[j].idx, 0);
//					}
//				}
//				st_col = 0;
//			}
//		}
//	}
//
//	//원복
//	ClearRenderTargetView(_EngineSystem.m_hash_RTV_0, 0.0f, 0.3f, 0.4f, 1.0f);
//	ClearDepthStencilView(_EngineSystem.m_hash_DSV_0);
//	SetOM_RenderTargets({ _EngineSystem.m_hash_RTV_0 }, _EngineSystem.m_hash_DSV_0);
//	
//	SetPS_ShaderResourceView(_EngineSystem.m_hash_DSV_ShadowMap, 6);
//}
//
//void RenderSystem::RenderCubeMap()
//{
//	//뷰포트 및 렌더타겟 세팅
//	FLOAT clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
//	ClearRenderTargetView(_EngineSystem.m_hash_RTV_CubeMap, 0.0f, 0.0f, 0.0f, 1.0f);
//	ClearDepthStencilView(_EngineSystem.m_hash_DSV_CubeMap);
//	SetRS_Viewport(&_EngineSystem.m_vp_CubeMap);
//	SetOM_RenderTargets({ _EngineSystem.m_hash_RTV_CubeMap }, _EngineSystem.m_hash_DSV_CubeMap);
//	
//	//카메라
//	size_t lookup_cam = _CameraSystem.lookup_cubemapcam;
//	const auto& c_cam_main = _ECSSystem.GetComponent<C_Camera>(lookup_cam);
//	const auto& c_cam_proj = _ECSSystem.GetComponent<C_Projection>(lookup_cam);
//	const Matrix4x4& matProj = c_cam_proj.matProj;
//
//	//큐브맵 뷰행렬
//	Vector3 pos = _ECSSystem.GetComponent<C_Transform>(lookup_cam).vPosition;
//	
//	//카메라위치세팅
//	CB_Campos cb_campos;
//	cb_campos.vPosition = pos;
//	_EngineSystem.UpdateConstantBuffer(g_hash_cb_campos, &cb_campos);
//	SetPS_ConstantBuffer(g_hash_cb_campos, 5);
//
//	float x = pos.GetX(), y = pos.GetY(), z = pos.GetZ();
//	Vector3 targets[6] =
//	{
//		{ x + 1.0f, y, z }, // +X
//		{ x - 1.0f, y, z }, // -X
//		{ x, y + 1.0f, z }, // +Y
//		{ x, y - 1.0f, z }, // -Y
//		{ x, y, z + 1.0f }, // +Z
//		{ x, y, z - 1.0f }  // -Z
//	};
//
//	Vector3 ups[6] =
//	{
//		{ 0.0f,  1.0f,  0.0f}, // +X: Up is Y
//		{ 0.0f,  1.0f,  0.0f}, // -X: Up is Y
//		{ 0.0f,  0.0f, -1.0f}, // +Y: Up is -Z (중요)
//		{ 0.0f,  0.0f,  1.0f}, // -Y: Up is +Z (중요)
//		{ 0.0f,  1.0f,  0.0f}, // +Z: Up is Y
//		{ 0.0f,  1.0f,  0.0f}  // -Z: Up is Y
//	};
//
//	//큐브맵행렬 세팅
//	CB_CubeMap cb_cubemap;
//	for (int i = 0; i < 6; i++)
//		cb_cubemap.matViews[i] = GetMat_View(pos, targets[i], ups[i]);
//	_EngineSystem.UpdateConstantBuffer(g_hash_cb_cubemap, &cb_cubemap);
//	SetGS_ConstantBuffer(g_hash_cb_cubemap, 7);
//
//	SetOM_BlendState(E_BSState::Opaque, NULL);
//	SetOM_DepthStenilState(E_DSState::Default);
//	SetPS_SamplerState(E_SMState::LINEAR_WRAP);
//	SetPS_SamplerState(E_SMState::POINT_CLAMP_COMPARISON, 6);
//	SetRS_RasterizerState(E_RSState::SOLID_CULLBACK_CW);
//
//	std::function<size_t(E_VerticesType)> GetHash_CubemapMat = [&](E_VerticesType eType)->size_t
//		{
//			switch (eType)
//			{
//				case E_VerticesType::Vertex_PTN :
//					return _EngineSystem.m_hash_Mat_CubeMap_PTN;
//				case E_VerticesType::Vertex_PTN_Skinned:
//					return _EngineSystem.m_hash_Mat_CubeMap_PTN_Skinned;
//				case E_VerticesType::Vertex_PTNTB :
//					return _EngineSystem.m_hash_Mat_CubeMap_PTNTB;
//				case E_VerticesType::Vertex_PTNTB_Skinned :
//					return _EngineSystem.m_hash_Mat_CubeMap_PTNTB_Skinned;
//				default :
//					return _EngineSystem.m_hash_Mat_CubeMap_PTN;
//			}
//		};
//	//RenderSky
//	{
//		SetOM_DepthStenilState(E_DSState::SkyBox);
//		SetRS_RasterizerState(E_RSState::SOLID_CULLBACK_CCW);
//		ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Transform, C_Render, T_Render_Sky>();
//		std::vector<Archetype*> queries = _ECSSystem.QueryArchetypes(key);
//		for (auto& archetype : queries)
//		{
//			if (archetype->HasComponents<T_Render_CubeMap>()) continue;
//			size_t st_row = 0;
//			size_t st_col = 0;
//			for (size_t row = st_row; row < archetype->GetCount_Chunks(); row++)
//			{
//				auto& transforms = archetype->GetComponents<C_Transform>(row);
//				auto& renders = archetype->GetComponents<C_Render>(row);
//				
//				for (size_t col = st_col; col < archetype->GetCount_Chunk(row); col++)
//				{
//					const Vector3& scale = transforms[col].vScale;
//					const Quaternion& rotate = transforms[col].qRotate;
//					const Vector3& position = transforms[col].vPosition;
//					CB_WVPITMatrix cb_wvpitmat;
//					cb_wvpitmat.matWorld = GetMat_World(scale, rotate, position);
//					cb_wvpitmat.matView = GetMat_Identity();
//					cb_wvpitmat.matProj = matProj;
//					cb_wvpitmat.matInvTrans = GetMat_InverseTranspose(cb_wvpitmat.matWorld);
//					_EngineSystem.UpdateConstantBuffer(g_hash_cb_wvpitmat, &cb_wvpitmat);
//					SetGS_ConstantBuffer(g_hash_cb_wvpitmat, 0);
//
//					
//					const auto& MeshMats = _ResourceSystem.GetResource<RenderAsset>(renders[col].hash_asset_Render)->m_hMeshMats;
//					BaseMesh* pMesh = _ResourceSystem.GetResource<BaseMesh>(MeshMats.hash_mesh);
//					SetIA_VertexBuffer(pMesh->GetVB());
//					SetIA_IndexBuffer(pMesh->GetIB());
//					for (UINT j = 0; j < MeshMats.hash_mats.size(); j++)
//					{
//						Material* pMaterial_Cubemap = _ResourceSystem.GetResource<Material>(GetHash_CubemapMat(pMesh->GetVerticesType()));
//						SetVS_Shader(pMaterial_Cubemap->GetVS());
//						SetIA_InputLayout(pMaterial_Cubemap->GetIL());
//						SetGS_Shader(pMaterial_Cubemap->GetGS());
//						SetPS_Shader(pMaterial_Cubemap->GetPS());
//
//						auto& hashMaterial = MeshMats.hash_mats[j];
//						Material* pMaterial = _ResourceSystem.GetResource<Material>(hashMaterial);
//						const std::vector<size_t>* texs = pMaterial->GetTextures();
//						int cnt = 0;
//						for (int idxTex = 0; idxTex < (UINT)E_Texture::count; idxTex++)
//						{
//							for (const auto& hashTx : texs[idxTex])
//							{
//								size_t hashSRV = _ResourceSystem.GetResource<Texture>(hashTx)->GetSRV();
//								SetPS_ShaderResourceView(hashSRV, cnt++);
//							}
//						}
//						Draw_Indicies(pMesh->GetRendIndices()[j].count, pMesh->GetRendIndices()[j].idx, 0);
//					}
//				}
//				st_col = 0;
//			}
//		}
//	}
//
//	//RendrGeometry
//	{
//		SetOM_DepthStenilState(E_DSState::Default);
//		SetRS_RasterizerState(E_RSState::SOLID_CULLBACK_CW);
//		//static
//		{
//			ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Transform, C_Render, T_Render_Geometry_Static>();
//			std::vector<Archetype*> queries = _ECSSystem.QueryArchetypes(key);
//			for (auto& archetype : queries)
//			{
//				if (archetype->HasComponents<T_Render_CubeMap>()) continue;
//				size_t st_row = 0;
//				size_t st_col = 0;
//				for (size_t row = st_row; row < archetype->GetCount_Chunks(); row++)
//				{
//					auto& transforms = archetype->GetComponents<C_Transform>(row);
//					auto& renders = archetype->GetComponents<C_Render>(row);
//					for (size_t col = st_col; col < archetype->GetCount_Chunk(row); col++)
//					{
//						if (!renders[col].bRenderable) continue;
//						const Vector3& scale = transforms[col].vScale;
//						const Quaternion& rotate = transforms[col].qRotate;
//						const Vector3& position = transforms[col].vPosition;
//						CB_WVPITMatrix cb_wvpitmat;
//						cb_wvpitmat.matWorld = GetMat_World(scale, rotate, position);
//						cb_wvpitmat.matView = GetMat_Identity();
//						cb_wvpitmat.matProj = matProj;
//						cb_wvpitmat.matInvTrans = GetMat_InverseTranspose(cb_wvpitmat.matWorld);
//						_EngineSystem.UpdateConstantBuffer(g_hash_cb_wvpitmat, &cb_wvpitmat);
//						SetGS_ConstantBuffer(g_hash_cb_wvpitmat, 0);
//
//						const auto& MeshMats = _ResourceSystem.GetResource<RenderAsset>(renders[col].hash_asset_Render)->m_hMeshMats;
//						BaseMesh* pMesh = _ResourceSystem.GetResource<BaseMesh>(MeshMats.hash_mesh);
//						SetIA_VertexBuffer(pMesh->GetVB());
//						SetIA_IndexBuffer(pMesh->GetIB());
//
//						for (UINT j = 0; j < MeshMats.hash_mats.size(); j++)
//						{
//							Material* pMaterial_Cubemap = _ResourceSystem.GetResource<Material>(GetHash_CubemapMat(pMesh->GetVerticesType()));
//							SetVS_Shader(pMaterial_Cubemap->GetVS());
//							SetIA_InputLayout(pMaterial_Cubemap->GetIL());
//							SetGS_Shader(pMaterial_Cubemap->GetGS());
//							SetPS_Shader(pMaterial_Cubemap->GetPS());
//
//							size_t hashMaterial = MeshMats.hash_mats[j];
//							Material* pMaterial = _ResourceSystem.GetResource<Material>(hashMaterial);
//
//							const std::vector<size_t>* texs = pMaterial->GetTextures();
//							int cnt = 0;
//							for (int idxTex = 0; idxTex < (UINT)E_Texture::count; idxTex++)
//							{
//								for (const auto& hashTx : texs[idxTex])
//								{
//									size_t hashSRV = _ResourceSystem.GetResource<Texture>(hashTx)->GetSRV();
//									SetPS_ShaderResourceView(hashSRV, cnt++);
//								}
//							}
//							Draw_Indicies(pMesh->GetRendIndices()[j].count, pMesh->GetRendIndices()[j].idx, 0);
//						}
//					}
//					st_col = 0;
//				}
//			}
//		}
//
//		//Skeletal
//		{
//			ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Transform, C_Render, C_Animation, T_Render_Geometry_Skeletal>();
//			std::vector<Archetype*> queries = _ECSSystem.QueryArchetypes(key);
//			for (auto& archetype : queries)
//			{
//				if (archetype->HasComponents<T_Render_CubeMap>()) continue;
//				size_t st_row = 0;
//				size_t st_col = 0;
//				for (size_t row = st_row; row < archetype->GetCount_Chunks(); row++)
//				{
//					auto& transforms = archetype->GetComponents<C_Transform>(row);
//					auto& renders = archetype->GetComponents<C_Render>(row);
//					auto& animations = archetype->GetComponents<C_Animation>(row);
//					for (size_t col = st_col; col < archetype->GetCount_Chunk(row); col++)
//					{
//						if (!renders[col].bRenderable) continue;
//						const Vector3& scale = transforms[col].vScale;
//						const Quaternion& rotate = transforms[col].qRotate;
//						const Vector3& position = transforms[col].vPosition;
//
//						CB_WVPITMatrix cb_wvpitmat;
//						cb_wvpitmat.matWorld = GetMat_World(scale, rotate, position);
//						cb_wvpitmat.matView = GetMat_Identity();
//						cb_wvpitmat.matProj = matProj;
//						cb_wvpitmat.matInvTrans = GetMat_InverseTranspose(cb_wvpitmat.matWorld);
//						_EngineSystem.UpdateConstantBuffer(g_hash_cb_wvpitmat, &cb_wvpitmat);
//						SetGS_ConstantBuffer(g_hash_cb_wvpitmat, 0);
//
//						_EngineSystem.UpdateConstantBuffer(g_hash_cb_bonemat, (void*)_AnimationSystem.GetAnimbones(animations[col].hash_animbones).data());
//						SetVS_ConstantBuffer(g_hash_cb_bonemat, 2);
//
//						const auto& MeshMats = _ResourceSystem.GetResource<RenderAsset>(renders[col].hash_asset_Render)->m_hMeshMats;
//						BaseMesh* pMesh = _ResourceSystem.GetResource<BaseMesh>(MeshMats.hash_mesh);
//						SetIA_VertexBuffer(pMesh->GetVB());
//						SetIA_IndexBuffer(pMesh->GetIB());
//						for (UINT j = 0; j < MeshMats.hash_mats.size(); j++)
//						{
//							
//							Material* pMaterial_Cubemap = _ResourceSystem.GetResource<Material>(GetHash_CubemapMat(pMesh->GetVerticesType()));
//							SetVS_Shader(pMaterial_Cubemap->GetVS());
//							SetIA_InputLayout(pMaterial_Cubemap->GetIL());
//							SetGS_Shader(pMaterial_Cubemap->GetGS());
//							SetPS_Shader(pMaterial_Cubemap->GetPS());
//
//							size_t hashMaterial = MeshMats.hash_mats[j];
//							Material* pMaterial = _ResourceSystem.GetResource<Material>(hashMaterial);
//
//							const std::vector<size_t>* texs = pMaterial->GetTextures();
//							int cnt = 0;
//							for (int idxTex = 0; idxTex < (UINT)E_Texture::count; idxTex++)
//							{
//								for (const auto& hashTx : texs[idxTex])
//								{
//									size_t hashSRV = _ResourceSystem.GetResource<Texture>(hashTx)->GetSRV();
//									SetPS_ShaderResourceView(hashSRV, cnt++);
//								}
//							}
//							Draw_Indicies(pMesh->GetRendIndices()[j].count, pMesh->GetRendIndices()[j].idx, 0);
//						}
//					}
//					st_col = 0;
//				}
//			}
//		}
//	}
//
//	//RTV, SRV에 사용하는 버퍼의 밉맵을 형성한다(앨리어싱처리)
//	_EngineSystem.GenerateMipMaps(_EngineSystem.m_hash_SRV_CubeMap);
//	
//	//원복
//	SetOM_RenderTargets({ _EngineSystem.m_hash_RTV_0 }, _EngineSystem.m_hash_DSV_0);
//	SetRS_Viewport(&_EngineSystem.m_vp_BB);
//	SetPS_ShaderResourceView(_EngineSystem.m_hash_SRV_CubeMap, 7);
//}
//
//void RenderSystem::RenderUI(const Matrix4x4& matOrtho)
//{
//	SetIA_Topology(D3D_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//	SetOM_BlendState(E_BSState::Opaque, NULL);
//	SetOM_DepthStenilState(E_DSState::UI);
//	SetPS_SamplerState(E_SMState::LINEAR_WRAP);
//	SetRS_RasterizerState(E_RSState::SOLID_CULLBACK_CW);
//
//	//2D객체로 SRV임시체크
//	SetOM_RenderTargets({}, NULL);
//	ClearRenderTargetView(_EngineSystem.m_hash_RTV_BB, 0.0f, 0.0f, 0.0f, 1.0f);
//	SetOM_RenderTargets({ _EngineSystem.m_hash_RTV_BB }, NULL);
//
//	ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Transform, C_Render, T_Render_UI>();
//	std::vector<Archetype*> queries = _ECSSystem.QueryArchetypes(key);
//	for (auto& archetype : queries)
//	{
//		size_t st_row = 0;
//		size_t st_col = 0;
//		for (size_t row = st_row; row < archetype->GetCount_Chunks(); row++)
//		{
//			auto& transforms = archetype->GetComponents<C_Transform>(row);
//			auto& renders = archetype->GetComponents<C_Render>(row);
//			for (size_t col = st_col; col < archetype->GetCount_Chunk(row); col++)
//			{
//				const Vector3& scale = transforms[col].vScale;
//				const Quaternion& rotate = transforms[col].qRotate;
//				const Vector3& position = transforms[col].vPosition;
//				CB_WVPITMatrix cb_wvpitmat;
//				cb_wvpitmat.matWorld = GetMat_ConvertGeometryOrtho() * GetMat_World(scale, rotate, position);
//				cb_wvpitmat.matView = GetMat_Identity();
//				cb_wvpitmat.matProj = matOrtho;
//				_EngineSystem.UpdateConstantBuffer(g_hash_cb_wvpitmat, &cb_wvpitmat);
//				SetVS_ConstantBuffer(g_hash_cb_wvpitmat, 0);
//
//				const auto& MeshMats = _ResourceSystem.GetResource<RenderAsset>(renders[col].hash_asset_Render)->m_hMeshMats;
//				BaseMesh* pMesh = _ResourceSystem.GetResource<BaseMesh>(MeshMats.hash_mesh);
//				SetIA_VertexBuffer(pMesh->GetVB());
//				SetIA_IndexBuffer(pMesh->GetIB());
//				for (UINT j = 0; j < MeshMats.hash_mats.size(); j++)
//				{
//					size_t hashMaterial = MeshMats.hash_mats[j];
//					Material* pMaterial = _ResourceSystem.GetResource<Material>(hashMaterial);
//					SetIA_InputLayout(pMaterial->GetIL());
//					SetVS_Shader(pMaterial->GetVS());
//					SetPS_Shader(pMaterial->GetPS());
//					SetGS_Shader(pMaterial->GetGS());
//
//					const std::vector<size_t>* texs = pMaterial->GetTextures();
//					int cnt = 0;
//					for (int idxTex = 0; idxTex < (UINT)E_Texture::count; idxTex++)
//					{
//						for (const auto& hashTx : texs[idxTex])
//						{
//							size_t hashSRV = hashTx;
//							SetPS_ShaderResourceView(hashSRV, cnt++);
//						}
//					}
//					Draw_Indicies(pMesh->GetRendIndices()[j].count, pMesh->GetRendIndices()[j].idx, 0);
//				}
//			}
//		}
//		st_col = 0;
//	}
//}
//
//void RenderSystem::RenderGeometry_Debug(const Matrix4x4& matView, const Matrix4x4& matProj)
//{
//	SetOM_BlendState(E_BSState::Opaque, NULL);
//	SetOM_DepthStenilState(E_DSState::Default);
//	SetPS_SamplerState(E_SMState::LINEAR_WRAP);
//	SetRS_RasterizerState(E_RSState::WIRE_CULLBACK_CW);
//	//DebugRender는 PS를 통일
//	SetPS_Shader(g_hash_PS_Debug_PC);
//	//Static
//	{
//		ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Transform, C_Render, C_Collider, T_Render_Geometry_Static>();
//		std::vector<Archetype*> queries = _ECSSystem.QueryArchetypes(key);
//		for (auto& archetype : queries)
//		{
//			size_t st_row = 0;
//			size_t st_col = 0;
//			for (size_t row = st_row; row < archetype->GetCount_Chunks(); row++)
//			{
//				auto& transforms = archetype->GetComponents<C_Transform>(row);
//				auto& renders = archetype->GetComponents<C_Render>(row);
//				auto& colliders = archetype->GetComponents<C_Collider>(row);
//
//				for (size_t col = st_col; col < archetype->GetCount_Chunk(row); col++)
//				{
//					if (!renders[col].bRenderable) continue;
//					const Vector3& scale = transforms[col].vScale;
//					const Quaternion& rotate = transforms[col].qRotate;
//					const Vector3& position = transforms[col].vPosition;
//					CB_WVPITMatrix cb_wvpitmat;
//					cb_wvpitmat.matWorld = GetMat_World(scale, rotate, position);
//					cb_wvpitmat.matView = matView;
//					cb_wvpitmat.matProj = matProj;
//					cb_wvpitmat.matInvTrans = GetMat_InverseTranspose(cb_wvpitmat.matWorld);
//					_EngineSystem.UpdateConstantBuffer(g_hash_cb_wvpitmat, &cb_wvpitmat);
//
//					SetIA_VertexBuffer(NULL);
//					SetIA_IndexBuffer(NULL);
//					SetIA_InputLayout(NULL);
//
//					if (colliders[col].type == E_Collider::AABB)
//					{
//						SetIA_Topology(D3D_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
//						SetVS_Shader(g_hash_VS_Debug);
//						SetHS_Shader(NULL);
//						SetDS_Shader(NULL);
//						SetGS_Shader(g_hash_GS_Debug_Box);
//						SetGS_ConstantBuffer(g_hash_cb_wvpitmat, 0);
//					}
//					else if (colliders[col].type == E_Collider::SPHERE)
//					{
//						SetIA_Topology(D3D_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
//						SetVS_Shader(g_hash_VS_Debug_Sphere);
//						SetHS_Shader(g_hash_HS_Debug_Sphere);
//						SetDS_Shader(g_hash_DS_Debug_Sphere);
//						SetGS_Shader(NULL);
//						SetDS_ConstantBuffer(g_hash_cb_wvpitmat, 0);
//					}
//
//					const auto& MeshMats = _ResourceSystem.GetResource<RenderAsset>(renders[col].hash_asset_Render)->m_hMeshMats;
//					for (UINT j = 0; j < MeshMats.size(); j++)
//					{
//						auto& iter = MeshMats[j];
//						BaseMesh* pMesh = _ResourceSystem.GetResource<BaseMesh>(iter.hash_mesh);
//
//						for (const auto& hash_collider : pMesh->GetCLs())
//						{
//							UINT drawCount = 0;
//							if (colliders[col].type == E_Collider::AABB)
//							{
//								CB_Debug_Box cb_debug_box;
//								_CollisionSystem.SetColliderDebugData(hash_collider, cb_debug_box);
//								_EngineSystem.UpdateConstantBuffer(g_hash_cb_debug_box, &cb_debug_box);
//								SetGS_ConstantBuffer(g_hash_cb_debug_box, 5);
//								drawCount = 1;
//							}
//							else if (colliders[col].type == E_Collider::SPHERE)
//							{
//								CB_Debug_Sphere cb_debug_sphere;
//								_CollisionSystem.SetColliderDebugData(hash_collider, cb_debug_sphere);
//								_EngineSystem.UpdateConstantBuffer(g_hash_cb_debug_sphere, &cb_debug_sphere);
//								SetHS_ConstantBuffer(g_hash_cb_debug_sphere, 5);
//								SetDS_ConstantBuffer(g_hash_cb_debug_sphere, 5);
//								drawCount = 60;
//							}
//
//							Draw_Vertices(drawCount, 0);
//						}
//					}
//				}
//				st_col = 0;
//			}
//		}
//	}
//
//	//Skeletal
//	{
//		ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Transform, C_Render, C_Animation, C_Collider, T_Render_Geometry_Skeletal>();
//		std::vector<Archetype*> queries = _ECSSystem.QueryArchetypes(key);
//		for (auto& archetype : queries)
//		{
//			size_t st_row = 0;
//			size_t st_col = 0;
//			for (size_t row = st_row; row < archetype->GetCount_Chunks(); row++)
//			{
//				auto& transforms = archetype->GetComponents<C_Transform>(row);
//				auto& renders = archetype->GetComponents<C_Render>(row);
//				auto& animations = archetype->GetComponents<C_Animation>(row);
//				auto& colliders = archetype->GetComponents<C_Collider>(row);
//
//				for (size_t col = st_col; col < archetype->GetCount_Chunk(row); col++)
//				{
//					if (!renders[col].bRenderable) continue;
//
//					const Vector3& scale = transforms[col].vScale;
//					const Quaternion& rotate = transforms[col].qRotate;
//					const Vector3& position = transforms[col].vPosition;
//					CB_WVPITMatrix cb_wvpitmat;
//					cb_wvpitmat.matView = matView;
//					cb_wvpitmat.matProj = matProj;
//					cb_wvpitmat.matInvTrans = GetMat_InverseTranspose(cb_wvpitmat.matWorld);
//
//					SetIA_VertexBuffer(NULL, 0);
//					SetIA_IndexBuffer(NULL);
//					SetIA_InputLayout(NULL);
//
//					if (colliders[col].type == E_Collider::AABB)
//					{
//						SetIA_Topology(D3D_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
//						SetVS_Shader(g_hash_VS_Debug);
//						SetHS_Shader(NULL);
//						SetDS_Shader(NULL);
//						SetGS_Shader(g_hash_GS_Debug_Box);
//					}
//					else if (colliders[col].type == E_Collider::SPHERE)
//					{
//						SetIA_Topology(D3D_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
//						SetVS_Shader(g_hash_VS_Debug_Sphere);
//						SetHS_Shader(g_hash_HS_Debug_Sphere);
//						SetDS_Shader(g_hash_DS_Debug_Sphere);
//						SetGS_Shader(NULL);
//					}
//
//					const auto& MeshMats = _ResourceSystem.GetResource<RenderAsset>(renders[col].hash_asset_Render)->m_hMeshMats;
//					for (UINT j = 0; j < MeshMats.size(); j++)
//					{
//						auto& iter = MeshMats[j];
//						BaseMesh* pMesh = _ResourceSystem.GetResource<BaseMesh>(iter.hash_mesh);
//						for (int idx = 0; idx < pMesh->GetCLs().size(); idx++)
//						{
//							cb_wvpitmat.matWorld = _AnimationSystem.GetAnimbones(animations[col].hash_animbones)[idx] * GetMat_World(scale, rotate, position);
//							_EngineSystem.UpdateConstantBuffer(g_hash_cb_wvpitmat, &cb_wvpitmat);
//
//							size_t hash_collider = pMesh->GetCLs()[idx];
//
//							UINT drawCount = 0;
//							if (colliders[col].type == E_Collider::AABB)
//							{
//								SetGS_ConstantBuffer(g_hash_cb_wvpitmat, 0);
//
//								CB_Debug_Box cb_debug_box;
//								_CollisionSystem.SetColliderDebugData(hash_collider, cb_debug_box);
//								_EngineSystem.UpdateConstantBuffer(g_hash_cb_debug_box, &cb_debug_box);
//								SetGS_ConstantBuffer(g_hash_cb_debug_box, 1);
//								drawCount = 1;
//							}
//							else if (colliders[col].type == E_Collider::SPHERE)
//							{
//								SetDS_ConstantBuffer(g_hash_cb_wvpitmat, 0);
//
//
//								CB_Debug_Sphere cb_debug_sphere;
//								_CollisionSystem.SetColliderDebugData(hash_collider, cb_debug_sphere);
//								_EngineSystem.UpdateConstantBuffer(g_hash_cb_debug_sphere, &cb_debug_sphere);
//								SetHS_ConstantBuffer(g_hash_cb_debug_sphere, 1);
//								SetDS_ConstantBuffer(g_hash_cb_debug_sphere, 1);
//								drawCount = 60;
//							}
//
//							Draw_Vertices(drawCount, 0);
//						}
//					}
//				}
//				st_col = 0;
//			}
//		}
//	}
//
//	SetVS_Shader(NULL);
//	SetHS_Shader(NULL);
//	SetDS_Shader(NULL);
//	SetGS_Shader(NULL);
//	SetPS_Shader(NULL);
//}
//
//void RenderSystem::RenderGeometry_PickingTriangle(const Matrix4x4& matView, const Matrix4x4& matProj)
//{
//	if (_EngineSystem.m_hash_pickingLookup == _HashNotInitialize)
//		return;
//	SetOM_BlendState(E_BSState::Opaque, NULL);
//	SetOM_DepthStenilState(E_DSState::Default);
//	SetPS_SamplerState(E_SMState::LINEAR_WRAP);
//	SetPS_SamplerState(E_SMState::POINT_CLAMP_COMPARISON, 6);
//	SetRS_RasterizerState(E_RSState::SOLID_CULLBACK_CW);
//
//	//static
//	{
//		ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Transform, C_Render, C_Collider, T_Render_Geometry_Static>();
//		std::vector<Archetype*> queries = _ECSSystem.QueryArchetypes(key);
//		for (auto& archetype : queries)
//		{
//			size_t st_row = 0;
//			size_t st_col = 0;
//			for (size_t row = st_row; row < archetype->GetCount_Chunks(); row++)
//			{
//				auto& transforms = archetype->GetComponents<C_Transform>(row);
//				auto& renders = archetype->GetComponents<C_Render>(row);
//				auto& colliders = archetype->GetComponents<C_Collider>(row);
//				for (size_t col = st_col; col < archetype->GetCount_Chunk(row); col++)
//				{
//					if (!renders[col].bRenderable) continue;
//					if (!colliders[col].bPicking) continue;
//					const Vector3& scale = transforms[col].vScale;
//					const Quaternion& rotate = transforms[col].qRotate;
//					const Vector3& position = transforms[col].vPosition;
//					CB_WVPITMatrix cb_wvpitmat;
//					cb_wvpitmat.matWorld = GetMat_World(scale, rotate, position);
//					cb_wvpitmat.matView = matView;
//					cb_wvpitmat.matProj = matProj;
//					cb_wvpitmat.matInvTrans = GetMat_InverseTranspose(cb_wvpitmat.matWorld);
//					_EngineSystem.UpdateConstantBuffer(g_hash_cb_wvpitmat, &cb_wvpitmat);
//					SetVS_ConstantBuffer(g_hash_cb_wvpitmat, 0);
//
//					const auto& MeshMats = _ResourceSystem.GetResource<RenderAsset>(renders[col].hash_asset_Render)->m_hMeshMats;
//					for (UINT j = 0; j < MeshMats.size(); j++)
//					{
//						auto& iter = MeshMats[j];
//						BaseMesh* pMesh = _ResourceSystem.GetResource<BaseMesh>(iter.hash_mesh);
//						SetIA_VertexBuffer(pMesh->GetVB());
//						SetIA_IndexBuffer(pMesh->GetIB());
//
//						Material* pMaterial = _ResourceSystem.GetResource<Material>(iter.hash_material);
//						SetIA_InputLayout(pMaterial->GetIL());
//						SetVS_Shader(pMaterial->GetVS());
//						SetHS_Shader(NULL);
//						SetDS_Shader(NULL);
//						SetGS_Shader(NULL);
//						SetPS_Shader(g_hash_PS_Picking);
//
//						Draw_Indicies(3, colliders[col].pickingIdx, 0);
//					}
//				}
//				st_col = 0;
//			}
//		}
//	}
//
//	//Skeletal
//	{
//		ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Transform, C_Render, C_Animation, C_Collider, T_Render_Geometry_Skeletal>();
//		std::vector<Archetype*> queries = _ECSSystem.QueryArchetypes(key);
//		for (auto& archetype : queries)
//		{
//			size_t st_row = 0;
//			size_t st_col = 0;
//			for (size_t row = st_row; row < archetype->GetCount_Chunks(); row++)
//			{
//				auto& transforms = archetype->GetComponents<C_Transform>(row);
//				auto& renders = archetype->GetComponents<C_Render>(row);
//				auto& animations = archetype->GetComponents<C_Animation>(row);
//				auto& colliders = archetype->GetComponents<C_Collider>(row);
//				for (size_t col = st_col; col < archetype->GetCount_Chunk(row); col++)
//				{
//					if (!renders[col].bRenderable) continue;
//					if (!colliders[col].bPicking) continue;
//					const Vector3& scale = transforms[col].vScale;
//					const Quaternion& rotate = transforms[col].qRotate;
//					const Vector3& position = transforms[col].vPosition;
//
//					CB_WVPITMatrix cb_wvpitmat;
//					cb_wvpitmat.matWorld = GetMat_World(scale, rotate, position);
//					cb_wvpitmat.matView = matView;
//					cb_wvpitmat.matProj = matProj;
//					cb_wvpitmat.matInvTrans = GetMat_InverseTranspose(cb_wvpitmat.matWorld);
//					_EngineSystem.UpdateConstantBuffer(g_hash_cb_wvpitmat, &cb_wvpitmat);
//					SetVS_ConstantBuffer(g_hash_cb_wvpitmat, 0);
//
//					CB_BoneMatrix cb_bonemat;
//					std::memcpy(cb_bonemat.bones, animations[col].matAnims, sizeof(cb_bonemat.bones));
//					_EngineSystem.UpdateConstantBuffer(g_hash_cb_bonemat, &cb_bonemat);
//					SetVS_ConstantBuffer(g_hash_cb_bonemat, 2);
//
//					const auto& MeshMats = _ResourceSystem.GetResource<RenderAsset>(renders[col].hash_asset_Render)->m_hMeshMats;
//					for (UINT j = 0; j < MeshMats.size(); j++)
//					{
//						auto& iter = MeshMats[j];
//						BaseMesh* pMesh = _ResourceSystem.GetResource<BaseMesh>(iter.hash_mesh);
//						SetIA_VertexBuffer(pMesh->GetVB());
//						SetIA_IndexBuffer(pMesh->GetIB());
//
//						Material* pMaterial = _ResourceSystem.GetResource<Material>(iter.hash_material);
//						SetIA_InputLayout(pMaterial->GetIL());
//						SetVS_Shader(pMaterial->GetVS());
//						SetHS_Shader(NULL);
//						SetDS_Shader(NULL);
//						SetGS_Shader(NULL);
//						SetPS_Shader(g_hash_PS_Picking);
//
//						Draw_Indicies(3, colliders[col].pickingIdx, 0);
//					}
//				}
//				st_col = 0;
//			}
//		}
//	}
//
//	size_t lookup			= _EngineSystem.m_hash_pickingLookup;
//	auto& info				= _ECSSystem.GetComponent<C_Info>(lookup);
//	auto& transform			= _ECSSystem.GetComponent<C_Transform>(lookup);
//	auto& render			= _ECSSystem.GetComponent<C_Render>(lookup);
//	auto& collider			= _ECSSystem.GetComponent<C_Collider>(lookup);
//
//	if (!render.bRenderable) return;
//
//	if (g_fTime_Log >= 1.0f)
//		std::cout << '\n' << "Picking Entity : " << _towm(info.szName) << '\n' << '\n';
//
//	const Vector3& scale = transform.vScale;
//	const Quaternion& rotate = transform.qRotate;
//	const Vector3& position = transform.vPosition;
//	CB_WVPITMatrix cb_wvpitmat;
//	cb_wvpitmat.matWorld = GetMat_World(scale, rotate, position);
//	cb_wvpitmat.matView = matView;
//	cb_wvpitmat.matProj = matProj;
//	cb_wvpitmat.matInvTrans = GetMat_InverseTranspose(cb_wvpitmat.matWorld);
//	_EngineSystem.UpdateConstantBuffer(g_hash_cb_wvpitmat, &cb_wvpitmat);
//	SetVS_ConstantBuffer(g_hash_cb_wvpitmat, 0);
//
//	if (_ECSSystem.HasComponent<C_Animation>(lookup))
//	{
//		auto& animation = _ECSSystem.GetComponent<C_Animation>(lookup);
//		_EngineSystem.UpdateConstantBuffer(g_hash_cb_bonemat, (void*)_AnimationSystem.GetAnimbones(animation.hash_animbones).data());
//		SetVS_ConstantBuffer(g_hash_cb_bonemat, 2);
//	}
//
//	const auto& MeshMats = _ResourceSystem.GetResource<RenderAsset>(render.hash_asset_Render)->m_hMeshMats;
//	for (UINT j = 0; j < MeshMats.size(); j++)
//	{
//		auto& iter = MeshMats[j];
//		BaseMesh* pMesh = _ResourceSystem.GetResource<BaseMesh>(iter.hash_mesh);
//		SetIA_VertexBuffer(pMesh->GetVB());
//		SetIA_IndexBuffer(pMesh->GetIB());
//
//		Material* pMaterial = _ResourceSystem.GetResource<Material>(iter.hash_material);
//		SetIA_InputLayout(pMaterial->GetIL());
//		SetVS_Shader(pMaterial->GetVS());
//		SetHS_Shader(NULL);
//		SetDS_Shader(NULL);
//		SetGS_Shader(NULL);
//		SetPS_Shader(g_hash_PS_Picking);
//
//		Draw_Indicies(3, collider.pickingIdx, 0);
//	}
//}
//
//void RenderSystem::RenderGeometry_PickingOutline(const Matrix4x4& matView, const Matrix4x4& matProj)
//{
//	if (_EngineSystem.m_hash_pickingLookup == _HashNotInitialize)
//		return;
//
//	std::function<size_t(E_VerticesType)> GetHash_OutlineMat = [&](E_VerticesType eType)->size_t
//		{
//			switch (eType)
//			{
//				case E_VerticesType::Vertex_PTN:
//					return _EngineSystem.m_hash_Mat_Outline_PTN;
//				case E_VerticesType::Vertex_PTN_Skinned:
//					return _EngineSystem.m_hash_Mat_Outline_PTN_Skinned;
//				case E_VerticesType::Vertex_PTNTB:
//					return _EngineSystem.m_hash_Mat_Outline_PTNTB;
//				case E_VerticesType::Vertex_PTNTB_Skinned:
//					return _EngineSystem.m_hash_Mat_Outline_PTNTB_Skinned;
//				default:
//					return _EngineSystem.m_hash_Mat_Outline_PTN;
//			}
//		};
//
//	SetPS_SamplerState(E_SMState::LINEAR_WRAP);
//	SetPS_SamplerState(E_SMState::POINT_CLAMP_COMPARISON, 6);
//	SetRS_RasterizerState(E_RSState::SOLID_CULLBACK_CW);
//
//	size_t lookup			= _EngineSystem.m_hash_pickingLookup;
//	auto& info				= _ECSSystem.GetComponent<C_Info>(lookup);
//	auto& transform			= _ECSSystem.GetComponent<C_Transform>(lookup);
//	auto& render			= _ECSSystem.GetComponent<C_Render>(lookup);
//	auto& collider			= _ECSSystem.GetComponent<C_Collider>(lookup);
//
//	if (!render.bRenderable) return;
//
//	const Vector3& scale = transform.vScale;
//	const Quaternion& rotate = transform.qRotate;
//	const Vector3& position = transform.vPosition;
//	CB_WVPITMatrix cb_wvpitmat;
//	cb_wvpitmat.matWorld = GetMat_World(scale, rotate, position);
//	cb_wvpitmat.matView = matView;
//	cb_wvpitmat.matProj = matProj;
//	cb_wvpitmat.matInvTrans = GetMat_InverseTranspose(cb_wvpitmat.matWorld);
//	_EngineSystem.UpdateConstantBuffer(g_hash_cb_wvpitmat, &cb_wvpitmat);
//	SetVS_ConstantBuffer(g_hash_cb_wvpitmat, 0);
//
//	if (_ECSSystem.HasComponent<C_Animation>(lookup))
//	{
//		auto& animation = _ECSSystem.GetComponent<C_Animation>(lookup);
//		_EngineSystem.UpdateConstantBuffer(g_hash_cb_bonemat, (void*)_AnimationSystem.GetAnimbones(animation.hash_animbones).data());
//		SetVS_ConstantBuffer(g_hash_cb_bonemat, 2);
//	}
//
//	CB_Outline_Picking cb_outline;
//	Vector3 outlineColor = Vector3(255.0f, 165.0f, 0.0f) / 255.0f;
//	cb_outline.color_thickness = Vector4(outlineColor, _EngineSystem.m_fThickness); //rgb, thickness;
//	_EngineSystem.UpdateConstantBuffer(g_hash_cb_outline_picking, &cb_outline);
//	SetVS_ConstantBuffer(g_hash_cb_outline_picking, 3);
//
//	const auto& MeshMats = _ResourceSystem.GetResource<RenderAsset>(render.hash_asset_Render)->m_hMeshMats;
//
//	//Outline_Write
//	SetOM_BlendState(E_BSState::Outline_Write, NULL);
//	SetOM_DepthStenilState(E_DSState::Outline_Write, 1);
//	for (UINT j = 0; j < MeshMats.size(); j++)
//	{
//		auto& iter = MeshMats[j];
//		BaseMesh* pMesh = _ResourceSystem.GetResource<BaseMesh>(iter.hash_mesh);
//		SetIA_VertexBuffer(pMesh->GetVB());
//		SetIA_IndexBuffer(pMesh->GetIB());
//
//		Material* pMaterial = _ResourceSystem.GetResource<Material>(iter.hash_material);
//		SetIA_InputLayout(pMaterial->GetIL());
//		SetVS_Shader(pMaterial->GetVS());
//		SetGS_Shader(pMaterial->GetGS());
//		SetPS_Shader(pMaterial->GetPS());
//
//		const std::vector<size_t>* texs = pMaterial->GetTextures();
//		int cnt = 0;
//		for (int idxTex = 0; idxTex < (UINT)E_Texture::count; idxTex++)
//		{
//			for (const auto& hashTx : texs[idxTex])
//			{
//				size_t hashSRV = _ResourceSystem.GetResource<Texture>(hashTx)->GetSRV();
//				SetPS_ShaderResourceView(hashSRV, cnt++);
//			}
//		}
//		Draw_Indicies(pMesh->GetRendIndices()[j].count, pMesh->GetRendIndices()[j].idx, 0);
//	}
//
//	//Outline_Draw
//	SetOM_BlendState(E_BSState::Outline_Draw, NULL);
//	SetOM_DepthStenilState(E_DSState::Outline_Draw, 1);
//	for (UINT j = 0; j < MeshMats.size(); j++)
//	{
//		auto& iter = MeshMats[j];
//		BaseMesh* pMesh = _ResourceSystem.GetResource<BaseMesh>(iter.hash_mesh);
//		SetIA_VertexBuffer(pMesh->GetVB());
//		SetIA_IndexBuffer(pMesh->GetIB());
//
//		Material* pMaterial = _ResourceSystem.GetResource<Material>(GetHash_OutlineMat(pMesh->GetVerticesType()));
//		SetIA_InputLayout(pMaterial->GetIL());
//		SetVS_Shader(pMaterial->GetVS());
//		SetGS_Shader(pMaterial->GetGS());
//		SetPS_Shader(pMaterial->GetPS());
//
//		const std::vector<size_t>* texs = pMaterial->GetTextures();
//		int cnt = 0;
//		for (int idxTex = 0; idxTex < (UINT)E_Texture::count; idxTex++)
//		{
//			for (const auto& hashTx : texs[idxTex])
//			{
//				size_t hashSRV = _ResourceSystem.GetResource<Texture>(hashTx)->GetSRV();
//				SetPS_ShaderResourceView(hashSRV, cnt++);
//			}
//		}
//		Draw_Indicies(pMesh->GetRendIndices()[j].count, pMesh->GetRendIndices()[j].idx, 0);
//	}
//}
