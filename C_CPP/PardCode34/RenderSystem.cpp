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
	//렌더패스
	m_hRP_CommandQueue.reserve(10'000);
}

void RenderSystem::Frame(float deltatime, float elapsedtime)
{
	if (g_fTime_Log >= 1.0f)
		std::cout << "Frame : " << "RenderSystem" << " Class" << '\n';
	//RenderPass
	CollectRenderItem(_ECSSystem.GetComponent<C_Transform>(_CameraSystem.lookup_maincam).vPosition);
	SortRenderItem();
}

void RenderSystem::PreRender(float deltatime, float elapsedtime)
{
	if (g_fTime_Log >= 1.0f)
		std::cout << "PreRender : " << "RenderSystem" << " Class" << '\n';
	
	//RTV초기화
	ClearRenderTargetView(_EngineSystem.m_hash_SRView_Quad, 0.0f, 0.3f, 0.4f, 1.0f);
	ClearRenderTargetView(_EngineSystem.m_hash_SRView_BB, 0.0f, 0.0f, 0.0f, 1.0f);
	ClearRenderTargetView(_EngineSystem.m_hash_RTV_CubeMap, 0.0f, 0.0f, 0.0f, 1.0f);

	ClearDepthStencilView(_EngineSystem.m_hash_DSView_Quad);
	ClearDepthStencilView(_EngineSystem.m_hash_DSV_ShadowMap);
	ClearDepthStencilView(_EngineSystem.m_hash_DSV_CubeMap);
	
	//Gbuffer 초기화
	ClearRenderTargetView(_EngineSystem.m_hash_Gbuffer_Position, 0.0f, 0.0f, 0.0f, 0.0f);
	ClearRenderTargetView(_EngineSystem.m_hash_Gbuffer_Normal, 0.0f, 0.0f, 0.0f, 0.0f);
	ClearRenderTargetView(_EngineSystem.m_hash_Gbuffer_Albedo, 0.0f, 0.0f, 0.0f, 0.0f);
	ClearRenderTargetView(_EngineSystem.m_hash_Gbuffer_Specular, 0.0f, 0.0f, 0.0f, 0.0f);

	//파이프라인포인터
	m_Inputlayout			= nullptr;
	m_VB					= nullptr;
	m_IB					= nullptr;
	m_VS					= nullptr;
	m_HS					= nullptr;
	m_DS					= nullptr;
	m_GS					= nullptr;
	m_PS					= nullptr;
	m_BlendState			= nullptr;
	m_DepthStencilState		= nullptr;
	m_RasterizerState		= nullptr;
	for (UINT i = 0; i < _ConvPipeline(E_Pipelines::COUNT); i++)
		for (UINT j = 0; j < _MAXSamplers; j++)
			m_SamplerStates[i][j] = nullptr;

	for (UINT i = 0; i < _ConvPipeline(E_Pipelines::COUNT); i++)
		for (UINT j = 0; j < _MAXSRVs; j++)
			m_SRVs[i][j] = nullptr;

	for (UINT i = 0; i < _ConvPipeline(E_Pipelines::COUNT); i++)
		for (UINT j = 0; j < _MAXCBs; j++)
			m_CBs[i][j] = nullptr;
}

void RenderSystem::Render(float deltatime, float elapsedtime)
{
	if (g_fTime_Log >= 1.0f)
		std::cout << "Render : " << "RenderSystem" << " Class" << '\n';

	SetPS_ConstantBuffer(g_hash_cb_directionalLight, 0);
	SetPS_ConstantBuffer(g_hash_cb_pointLight, 1);
	SetPS_ConstantBuffer(g_hash_cb_spotLight, 2);

	SetVS_ConstantBuffer(g_hash_cb_wvpitmat, 0);
	SetDS_ConstantBuffer(g_hash_cb_wvpitmat, 0);		//tesselate sphere 
	SetGS_ConstantBuffer(g_hash_cb_wvpitmat, 0);		//geometry box

	SetVS_ConstantBuffer(g_hash_cb_lightmat, 1);		//shadowmap
	SetGS_ConstantBuffer(g_hash_cb_lightmat, 1);		
	SetPS_ConstantBuffer(g_hash_cb_lightmat, 6);

	SetVS_ConstantBuffer(g_hash_cb_bonemat, 2);			//animation

	SetPS_ConstantBuffer(g_hash_cb_campos, 5);			//campos

	SetGS_ConstantBuffer(g_hash_cb_cubemap, 7);			//cubemap

	SetGS_ConstantBuffer(g_hash_cb_debug_box, 5);		//debug property
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

	//카메라위치
	CB_Campos cb_campos;

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

	//큐브맵 카메라
	size_t lookup_cubecam = _CameraSystem.lookup_cubemapcam;
	const auto& c_cubecam_main = _ECSSystem.GetComponent<C_Camera>(lookup_cubecam);
	const auto& c_cubecam_proj = _ECSSystem.GetComponent<C_Projection>(lookup_cubecam);
	const auto& cubecam_matProj = c_cubecam_proj.matProj;

	//큐브맵 카메라 위치
	const Vector3& cam_pos_cubemap = _ECSSystem.GetComponent<C_Transform>(lookup_cubecam).vPosition;

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

	//피킹 아웃라인
	CB_Outline_Picking cb_outline;
	Vector3 outlineColor = Vector3(255.0f, 165.0f, 0.0f) / 255.0f;
	cb_outline.color_thickness = Vector4(outlineColor, _EngineSystem.m_fThickness); //rgb, thickness;
	_EngineSystem.UpdateConstantBuffer(g_hash_cb_outline_picking, &cb_outline);
	
	_RPKey keyPrev = _HashNotInitialize;

	//ExecuteRenderPass
	// 1. Pass는 4비트면 충분하므로 uint8_t로도 충분하지만, 가독성을 위해 유지
	// 2. 셰이더 조합 ID (16비트)
	// 3. 상태(RS, BS, DSS) 조합 ID (8비트)
	// 4. 리소스(Mesh, Material, Collider) ID (16비트)
	// 5. 거리 계산 (20비트) - float 거리를 받아서 20비트 정수로 변환
	// 4 + 16 + 8 + 16 + 20 -> 64비트 hash 비트별구분

	for (const auto& renderItem : m_hRP_CommandQueue)
	{
		_RPKey keyCur			= renderItem.sortKey;
		Archetype* pArchetype	= renderItem.pArchetype;
		size_t row				= renderItem.entityRow;
		size_t col				= renderItem.entityCol;
		UINT renderCnt			= renderItem.renderCnt;
		UINT startIdx			= renderItem.startIdx;

		//debug entity
		const auto& info = pArchetype->GetComponents<C_Info>(row)[col];

		//WVPMat
		Matrix4x4 matWorld;
		Matrix4x4 matView;
		Matrix4x4 matProj;

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
				case (uint32_t)E_RenderPass::Shadow:
				{
					SetOM_RenderTargets(std::vector<size_t>(), _EngineSystem.m_hash_DSV_ShadowMap);
				}break;

				case (uint32_t)E_RenderPass::Cubemap:
				{
					//뷰포트 및 렌더타겟 세팅
					SetRS_Viewport(&_EngineSystem.m_vp_CubeMap);
					SetOM_RenderTargets({ _EngineSystem.m_hash_RTV_CubeMap }, _EngineSystem.m_hash_DSV_CubeMap);
					cb_campos.vPosition = cam_pos_cubemap;
					_EngineSystem.UpdateConstantBuffer(g_hash_cb_campos, &cb_campos);
				}break;

				case (uint32_t)E_RenderPass::Opaque:
				{
					SetOM_RenderTargets
					(
						{ 
							_EngineSystem.m_hash_Gbuffer_Position,  
							_EngineSystem.m_hash_Gbuffer_Normal,
							_EngineSystem.m_hash_Gbuffer_Albedo,
							_EngineSystem.m_hash_Gbuffer_Specular,
						}, 
						_EngineSystem.m_hash_DSView_Quad
					);
				}break;


				case (uint32_t)E_RenderPass::DefferedLighting:
				case (uint32_t)E_RenderPass::Sky:
				case (uint32_t)E_RenderPass::Outline_Write:
				case (uint32_t)E_RenderPass::Outline_Draw:
				case (uint32_t)E_RenderPass::Picking_Triangle:
				case (uint32_t)E_RenderPass::Debug:
				case (uint32_t)E_RenderPass::Transparent:
				{
					SetOM_RenderTargets({ _EngineSystem.m_hash_SRView_Quad }, _EngineSystem.m_hash_DSView_Quad);
				}break;


				case (uint32_t)E_RenderPass::UI:
				{
					//2D객체로 SRV임시체크
					//SetOM_RenderTargets({ }, NULL);
					SetOM_RenderTargets({ _EngineSystem.m_hash_SRView_BB }, _HashNotInitialize);
				}break;

				default: break;
			}

			if (prevPass == (uint32_t)E_RenderPass::Shadow)
			{
				SetPS_SamplerState(E_SMState::POINT_CLAMP_COMPARISON, 6);	//ShadowMap SamplerState
				SetPS_ShaderResourceView(_EngineSystem.m_hash_DSV_ShadowMap, 6);
			}

			if (prevPass == (uint32_t)E_RenderPass::Cubemap)
			{
				//RTV, SRV에 사용하는 버퍼의 밉맵을 형성한다(앨리어싱처리)
				_EngineSystem.GenerateMipMaps(_EngineSystem.m_hash_SRV_CubeMap);
				SetPS_ShaderResourceView(_EngineSystem.m_hash_SRV_CubeMap, 7);
			}

			if (prevPass == (uint32_t)E_RenderPass::Opaque)
			{
				SetPS_ShaderResourceView(_EngineSystem.m_hash_Gbuffer_Position, 10);
				SetPS_ShaderResourceView(_EngineSystem.m_hash_Gbuffer_Normal, 11);
				SetPS_ShaderResourceView(_EngineSystem.m_hash_Gbuffer_Albedo, 12);
				SetPS_ShaderResourceView(_EngineSystem.m_hash_Gbuffer_Specular, 13);
			}
		}

		if (diff >> 44) // 셰이더가 바뀌었는지 확인 (비트 44~59)
		{
			// VS, PS 등 셰이더 바인딩
			uint32_t IDShaders = (keyCur >> 44) & 0xFFFF;	//16비트
			size_t hashMaterial = m_resRP_Shaders[IDShaders];
			Material* pMaterial = _ResourceSystem.GetResource<Material>(hashMaterial);
			SetIA_Topology(pMaterial->GetTopology());
			SetIA_InputLayout(pMaterial->GetIL());
			SetVS_Shader(pMaterial->GetVS());
			SetHS_Shader(pMaterial->GetHS());
			SetDS_Shader(pMaterial->GetDS());
			SetGS_Shader(pMaterial->GetGS());
			if(curPass == (uint32_t)E_RenderPass::Shadow)
				SetPS_Shader(_ResourceSystem.GetResource<Material>(_EngineSystem.m_hash_Mat_ShadowMap)->GetPS());
			else if(curPass == (uint32_t)E_RenderPass::Picking_Triangle)
				SetPS_Shader(_ResourceSystem.GetResource<Material>(_EngineSystem.m_hash_Mat_Picking)->GetPS());
			else
				SetPS_Shader(pMaterial->GetPS());
		}

		if (diff >> 36) // 상태가 바뀌었는지 확인 (비트 36~43)
		{
			// RS, BS, DSS 바인딩
			uint32_t IDStates = (keyCur >> 36) & 0xFF;		//8비트
			RPStates states = m_resRP_States[IDStates];
			SetRS_RasterizerState(states.stateRS);
			SetOM_BlendState(states.stateBS, states.blendFactor, states.blendMask);
			SetOM_DepthStenilState(states.stateDS, states.stencilRef);
		}

		if (diff >> 20)	// 상태가 바뀌었는지 확인 (비트 20~35)
		{
			//버퍼 세팅
			uint32_t IDResources = (keyCur >> 20) & 0xFFFF;		//16비트
			RPResources resources = m_resRP_Resources[IDResources];
			
			BaseMesh* pMesh = _ResourceSystem.GetResource<BaseMesh>(resources.hashMesh);
			SetIA_VertexBuffer(pMesh->GetVB());
			SetIA_IndexBuffer(pMesh->GetIB());
			
			//텍스쳐
			Material* pMaterial = _ResourceSystem.GetResource<Material>(resources.hashMat);
			int cnt = 0;
			for (int idxTex = 0; idxTex < (UINT)E_Texture::count; idxTex++)
			{
				for (const auto& hashTx : pMaterial->GetTextures()[idxTex])
				{
					size_t hashSRV = _ResourceSystem.GetResource<Texture>(hashTx)->GetSRV();
					SetPS_ShaderResourceView(hashSRV, cnt++);
				}
			}

			//상수버퍼
			if (curPass == (uint32_t)E_RenderPass::Debug)
			{
				size_t hash_collider = pMesh->GetCLs()[resources.idxCollider];
				if (resources.collider == E_Collider::BOX)
				{
					CB_Debug_Box cb_debug_box;
					_CollisionSystem.SetColliderDebugData(hash_collider, cb_debug_box);
					_EngineSystem.UpdateConstantBuffer(g_hash_cb_debug_box, &cb_debug_box);
				}
				else if (resources.collider == E_Collider::SPHERE)
				{
					CB_Debug_Sphere cb_debug_sphere;
					_CollisionSystem.SetColliderDebugData(hash_collider, cb_debug_sphere);
					_EngineSystem.UpdateConstantBuffer(g_hash_cb_debug_sphere, &cb_debug_sphere);
				}
				if (pArchetype->HasComponents<C_Animation>())
				{
					const auto& animation = pArchetype->GetComponents<C_Animation>(row)[col];
					matWorld = matWorld * _AnimationSystem.GetAnimbones(animation.hash_animbones)[resources.idxCollider];
				}
			}
		}
		if (pArchetype->HasComponents<C_Animation>())
		{
			const auto& animation = pArchetype->GetComponents<C_Animation>(row)[col];
			_EngineSystem.UpdateConstantBuffer(g_hash_cb_bonemat, (void*)_AnimationSystem.GetAnimbones(animation.hash_animbones).data());
		}

		// ... 나머지 리소스 바인딩 및 Draw 호출
		if (pArchetype->HasComponents<C_Transform>())
		{
			const auto& transform = pArchetype->GetComponents<C_Transform>(row)[col];
			const Vector3& scale = transform.vScale;
			const Quaternion& quat = transform.qRotate;
			const Vector3& pos = transform.vPosition;

			if (pArchetype->HasComponents<T_Render_UI>())
			{
				matWorld = GetMat_ConvertGeometryOrtho() * GetMat_World(scale, quat, pos);
				matView = GetMat_Identity();
				matProj = cam_matOrhto;
			}
			else
			{
				matWorld = matWorld * GetMat_World(scale, quat, pos);
				matView = cam_matView;
				matProj = cam_matProj;
			}

			if (curPass == (uint32_t)E_RenderPass::Shadow)
			{
				matView = cb_lightMat.matLightView;
				matProj = cb_lightMat.matLightProj;
			}
			else if (curPass == (uint32_t)E_RenderPass::Cubemap)
			{
				matView = GetMat_Identity();
				matProj = cubecam_matProj;
			}

			CB_WVPITMatrix cb_wvpitmat;
			cb_wvpitmat.matWorld = matWorld;
			cb_wvpitmat.matView = matView;
			cb_wvpitmat.matProj = matProj;
			cb_wvpitmat.matInvTrans = GetMat_InverseTranspose(matWorld);
			_EngineSystem.UpdateConstantBuffer(g_hash_cb_wvpitmat, &cb_wvpitmat);
		}

		if (curPass != (uint32_t)E_RenderPass::Debug)
			Draw_Indicies(renderCnt, startIdx, 0);
		else
			Draw_Vertices(renderCnt, startIdx);

		//키상태 저장
		keyPrev = keyCur;
	}
}

void RenderSystem::PostRender()
{
	if (g_fTime_Log >= 1.0f)
		std::cout << "PostRender : " << "RenderSystem" << " Class" << '\n';
	SwapchainPresent(false);
	ClearRenderItem();
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
	if (_EngineSystem.GetRTV(_EngineSystem.m_hash_SRView_BB)) _EngineSystem.GetRTV(_EngineSystem.m_hash_SRView_BB)->ReleaseView();

	if (_EngineSystem.GetRTV(_EngineSystem.m_hash_SRView_Quad)) _EngineSystem.GetRTV(_EngineSystem.m_hash_SRView_Quad)->ReleaseView();
	if (_EngineSystem.GetSRV(_EngineSystem.m_hash_SRView_Quad)) _EngineSystem.GetSRV(_EngineSystem.m_hash_SRView_Quad)->ReleaseView();

	if (_EngineSystem.GetDSV(_EngineSystem.m_hash_DSView_Quad)) _EngineSystem.GetDSV(_EngineSystem.m_hash_DSView_Quad)->ReleaseView();
	if (_EngineSystem.GetSRV(_EngineSystem.m_hash_DSView_Quad)) _EngineSystem.GetSRV(_EngineSystem.m_hash_DSView_Quad)->ReleaseView();

	//Gbuffer
	if (_EngineSystem.GetRTV(_EngineSystem.m_hash_Gbuffer_Position)) _EngineSystem.GetRTV(_EngineSystem.m_hash_Gbuffer_Position)->ReleaseView();
	if (_EngineSystem.GetSRV(_EngineSystem.m_hash_Gbuffer_Position)) _EngineSystem.GetSRV(_EngineSystem.m_hash_Gbuffer_Position)->ReleaseView();

	if (_EngineSystem.GetRTV(_EngineSystem.m_hash_Gbuffer_Normal)) _EngineSystem.GetRTV(_EngineSystem.m_hash_Gbuffer_Normal)->ReleaseView();
	if (_EngineSystem.GetSRV(_EngineSystem.m_hash_Gbuffer_Normal)) _EngineSystem.GetSRV(_EngineSystem.m_hash_Gbuffer_Normal)->ReleaseView();

	if (_EngineSystem.GetRTV(_EngineSystem.m_hash_Gbuffer_Albedo)) _EngineSystem.GetRTV(_EngineSystem.m_hash_Gbuffer_Albedo)->ReleaseView();
	if (_EngineSystem.GetSRV(_EngineSystem.m_hash_Gbuffer_Albedo)) _EngineSystem.GetSRV(_EngineSystem.m_hash_Gbuffer_Albedo)->ReleaseView();

	if (_EngineSystem.GetRTV(_EngineSystem.m_hash_Gbuffer_Specular)) _EngineSystem.GetRTV(_EngineSystem.m_hash_Gbuffer_Specular)->ReleaseView();
	if (_EngineSystem.GetSRV(_EngineSystem.m_hash_Gbuffer_Specular)) _EngineSystem.GetSRV(_EngineSystem.m_hash_Gbuffer_Specular)->ReleaseView();

	_EngineSystem.GetSwapChain()->ResizeBuffers(1, g_iWidth, g_iHeight, DXGI_FORMAT_UNKNOWN, 0);
	//백버퍼
	HRESULT hResult;
	ID3D11Texture2D* pBuffer;
	hResult = _EngineSystem.GetSwapChain()->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBuffer);
	_ASEERTION_CREATE(hResult, "BackBuffer");
	_EngineSystem.m_hash_SRView_BB			= _EngineSystem.CreateRenderTargetView(Hasing_wstring(L"SRView_BB"), pBuffer, _BB);
	pBuffer->Release();

	//RTT
	_EngineSystem.m_hash_SRView_Quad		= _EngineSystem.CreateViews(L"SRView_Quad", _Target_ResourceView, g_iWidth, g_iHeight);
	_EngineSystem.m_hash_DSView_Quad		= _EngineSystem.CreateViews(L"DSView_Quad", _Target_DepthView, g_iWidth, g_iHeight);
	SetViewportSize(&_EngineSystem.m_vp_BB, g_iWidth, g_iHeight);

	//Gbuffer
	_EngineSystem.m_hash_Gbuffer_Position	= _EngineSystem.CreateViews(L"Gbuffer_Position", _Target_ResourceView, g_iWidth, g_iHeight);
	_EngineSystem.m_hash_Gbuffer_Normal		= _EngineSystem.CreateViews(L"Gbuffer_Normal", _Target_ResourceView, g_iWidth, g_iHeight);
	_EngineSystem.m_hash_Gbuffer_Albedo		= _EngineSystem.CreateViews(L"Gbuffer_Albedo", _Target_ResourceView, g_iWidth, g_iHeight);
	_EngineSystem.m_hash_Gbuffer_Specular	= _EngineSystem.CreateViews(L"Gbuffer_Specular", _Target_ResourceView, g_iWidth, g_iHeight);
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
	auto ptr = pIL ? pIL->GetInputLayout() : nullptr;
	if (m_Inputlayout == ptr) return;
	m_Inputlayout = ptr;
	_EngineSystem.GetD3DDeviceContext()->IASetInputLayout(ptr);
}

void RenderSystem::SetIA_VertexBuffer(size_t hashVB, UINT offset)
{
	auto pBuffer = _EngineSystem.GetVB(hashVB);
	auto ptr = pBuffer ? pBuffer->GetBuffer() : nullptr;
	if (m_VB == ptr) return;
	m_VB = ptr;
	UINT stride = pBuffer ? pBuffer->GetVertexSize() : 0;
	_EngineSystem.GetD3DDeviceContext()->IASetVertexBuffers(0, 1, &ptr, &stride, &offset);
}

void RenderSystem::SetIA_IndexBuffer(size_t hashIB, UINT offset)
{
	auto pBuffer = _EngineSystem.GetIB(hashIB);
	auto ptr = pBuffer ? pBuffer->GetBuffer() : nullptr;
	if (m_IB == ptr) return;
	m_IB = ptr;
	_EngineSystem.GetD3DDeviceContext()->IASetIndexBuffer(ptr, DXGI_FORMAT_R32_UINT, offset); //4바이트, 32비트를의미
}

void RenderSystem::SetVS_Shader(size_t hashVS)
{
	auto pShader = _EngineSystem.GetVS(hashVS);
	auto ptr = pShader ? pShader->GetShader() : nullptr;
	if (m_VS == ptr) return;
	m_VS = ptr;
	_EngineSystem.GetD3DDeviceContext()->VSSetShader(ptr, nullptr, 0);
}

void RenderSystem::SetVS_ConstantBuffer(size_t hashCB, UINT startIdx)
{
	auto pBuffer = _EngineSystem.GetCB(hashCB);
	auto ptr = pBuffer ? pBuffer->GetBuffer() : nullptr;
	int isize = pBuffer ? 1 : 0;
	if (m_CBs[_ConvPipeline(E_Pipelines::VS)][startIdx] == ptr) return;
	m_CBs[_ConvPipeline(E_Pipelines::VS)][startIdx] = ptr;
	_EngineSystem.GetD3DDeviceContext()->VSSetConstantBuffers(startIdx, isize, &ptr);
}

void RenderSystem::SetHS_Shader(size_t hashHS)
{
	auto pShader = _EngineSystem.GetHS(hashHS);
	auto ptr = pShader ? pShader->GetShader() : nullptr;
	if (m_HS == ptr) return;
	m_HS = ptr;
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
	if (m_DS == ptr) return;
	m_DS = ptr;
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
	if (m_GS == ptr) return;
	m_GS = ptr;
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
	if (m_SamplerStates[_ConvPipeline(E_Pipelines::PS)][startIdx] == pState) return;
	m_SamplerStates[_ConvPipeline(E_Pipelines::PS)][startIdx] = pState;
	_EngineSystem.GetD3DDeviceContext()->PSSetSamplers(startIdx, 1, &pState);
}

void RenderSystem::SetPS_Shader(size_t hashPS)
{
	auto pShader = _EngineSystem.GetPS(hashPS);
	auto ptr = pShader ? pShader->GetShader() : nullptr;
	if (m_PS == ptr) return;
	m_PS = ptr;
	_EngineSystem.GetD3DDeviceContext()->PSSetShader(ptr, nullptr, 0);
}

void RenderSystem::SetPS_ShaderResourceView(size_t hashSRV, UINT startIdx)
{
	auto pView = _EngineSystem.GetSRV(hashSRV);
	auto ptr = pView ? pView->GetView() : nullptr;
	int isize = pView ? 1 : 0;
	if (m_SRVs[_ConvPipeline(E_Pipelines::PS)][startIdx] == ptr) return;
	m_SRVs[_ConvPipeline(E_Pipelines::PS)][startIdx] = ptr;
	_EngineSystem.GetD3DDeviceContext()->PSSetShaderResources(startIdx, isize, &ptr);
}

void RenderSystem::SetPS_ConstantBuffer(size_t hashCB, UINT startIdx)
{
	auto pBuffer = _EngineSystem.GetCB(hashCB);
	auto ptr = pBuffer ? pBuffer->GetBuffer() : nullptr;
	int isize = pBuffer ? 1 : 0;
	if (m_CBs[_ConvPipeline(E_Pipelines::PS)][startIdx] == ptr) return;
	m_CBs[_ConvPipeline(E_Pipelines::PS)][startIdx] = ptr;
	_EngineSystem.GetD3DDeviceContext()->PSSetConstantBuffers(startIdx, isize, &ptr);
}

void RenderSystem::SetRS_RasterizerState(E_RSState eRSState)
{
	auto pState = _EngineSystem.GetState_RS()->GetState(eRSState);
	_ASEERTION_NULCHK(pState, "NotExist");
	if (m_RasterizerState == pState) return;
	m_RasterizerState = pState;
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
	auto dsv = pDSV ? pDSV->GetView() : NULL;
	_EngineSystem.GetD3DDeviceContext()->OMSetRenderTargets(rtvs.size(), rtvs.empty() ? NULL : rtvs.data(), dsv);
}

void RenderSystem::SetOM_DepthStenilState(E_DSState eDSState, UINT stencilRef)
{
	auto pState = _EngineSystem.GetState_DS()->GetState(eDSState);
	_ASEERTION_NULCHK(pState, "NotExist");
	if (m_DepthStencilState == pState) return;
	m_DepthStencilState = pState;
	_EngineSystem.GetD3DDeviceContext()->OMSetDepthStencilState(pState, stencilRef);
}

void RenderSystem::SetOM_BlendState(E_BSState eBSState, const FLOAT* blendFactor, UINT sampleMask)
{
	auto pState = _EngineSystem.GetState_BS()->GetState(eBSState);
	_ASEERTION_NULCHK(pState, "NotExist");
	if (m_BlendState == pState) return;
	m_BlendState = pState;
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
		case E_Collider::BOX :
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

/*
	렌더 패스 (Render Pass): 그림자와 불투명, 투명 객체는 렌더 타겟(RT) 자체가 다르거나 블렌딩 연산 방식이 완전히 다르므로 가장 먼저 분리해야 합니다.
	셰이더 종류 (Shader): 셰이더 교체는 GPU 파이프라인의 프로그램을 통째로 바꾸는 작업이라 가장 비쌉니다.
	상태 종류 (State): Rasterizer(Cull 모드), Blend, DepthStencil 상태 변경입니다. 셰이더보다는 가볍지만 여전히 비용이 듭니다.
	리소스 종류 (Texture/Buffer): 텍스처나 상수 버퍼 바인딩입니다. 상대적으로 자주 일어나며 셰이더나 상태 변경보다는 빠릅니다.
	거리 비례 (Depth):
	불투명: 앞에서 뒤로(Front-to-Back) 그려서 뒤에 가려진 정점을 미리 연산에서 제외(Early-Z)합니다.
	투명: 뒤에서 앞으로(Back-to-Front) 그려야 올바른 알파 블렌딩 결과가 나옵니다.

	// 1. Pass는 4비트면 충분하므로 uint8_t로도 충분하지만, 가독성을 위해 유지
	// 2. 셰이더 조합 ID (16비트)
	// 3. 상태(RS, BS, DSS) 조합 ID (8비트)
	// 4. 리소스(Mesh, Material, Collider) 16비트
	// 5. 거리 계산 (20비트) - float 거리를 받아서 20비트 정수로 변환
	// 4 + 16 + 8 + 16 + 20 -> 64비트 hash 비트별구분
*/

uint32_t RenderSystem::GetRenderPassKey_Shaders(size_t hashMaterial)
{
	Material* pMaterial = _ResourceSystem.GetResource<Material>(hashMaterial);
	size_t hash = hashMaterial;
	if (pMaterial->GetVS() != _HashNotInitialize) hash_combine(hash, pMaterial->GetVS());
	if (pMaterial->GetHS() != _HashNotInitialize) hash_combine(hash, pMaterial->GetHS());
	if (pMaterial->GetDS() != _HashNotInitialize) hash_combine(hash, pMaterial->GetDS());
	if (pMaterial->GetGS() != _HashNotInitialize) hash_combine(hash, pMaterial->GetGS());
	if (pMaterial->GetPS() != _HashNotInitialize) hash_combine(hash, pMaterial->GetPS());

	auto iter = m_hRP_Shaders.find(hash);
	if (iter != m_hRP_Shaders.end())
		return iter->second;
	uint16_t newID = static_cast<uint16_t>(m_resRP_Shaders.size());
	m_hRP_Shaders[hash] = newID;
	m_resRP_Shaders.push_back(hashMaterial);
	return newID;
}

uint32_t RenderSystem::GetRenderPassKey_States(E_RSState stateRS, E_DSState stateDS, E_BSState stateBS, UINT ds_stencilref, float* bs_factor, UINT bs_mask)
{
	size_t hash = 0;
	hash_combine(hash, std::hash<UINT>{}(static_cast<UINT>(stateRS)));
	hash_combine(hash, std::hash<UINT>{}(static_cast<UINT>(stateBS)));
	hash_combine(hash, std::hash<UINT>{}(static_cast<UINT>(stateDS)));
	float blendFactor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	if (bs_factor != nullptr)
	{
		for (int i = 0; i < 4; i++)
			blendFactor[i] = bs_factor[i];
	}
	for (int i = 0; i < 4; ++i)
		hash_combine(hash, std::hash<float>{}(blendFactor[i]));
	hash_combine(hash, std::hash<UINT>{}(bs_mask));
	hash_combine(hash, std::hash<UINT>{}(ds_stencilref));

	auto iter = m_hRP_States.find(hash);
	if (iter != m_hRP_States.end())
		return iter->second;

	uint8_t newID = static_cast<uint8_t>(m_resRP_States.size());
	m_hRP_States[hash] = newID;
	m_resRP_States.push_back({ stateRS, stateBS, {blendFactor[0], blendFactor[1], blendFactor[2], blendFactor[3]}, bs_mask, stateDS, ds_stencilref });
	return newID;
}

uint32_t RenderSystem::GetRenderPassKey_Resources(size_t hashMesh, size_t hashMat, E_Collider collider, UINT idx)
{
	size_t hash = 0;
	hash_combine(hash, std::hash<size_t>{}(hashMesh));
	hash_combine(hash, std::hash<size_t>{}(hashMat));
	hash_combine(hash, std::hash<size_t>{}(static_cast<uint32_t>(collider)));
	hash_combine(hash, std::hash<size_t>{}(idx));
	auto iter = m_hRP_Resources.find(hash);
	if (iter != m_hRP_Resources.end())
		return iter->second;

	uint16_t newID = static_cast<uint16_t>(m_resRP_Resources.size());
	m_hRP_Resources[hash] = newID;
	m_resRP_Resources.push_back({ hashMesh, hashMat, collider, idx });
	return newID;
}

uint32_t RenderSystem::GetRenderPassKey_DistToCamera(float dist)
{
	// 거리를 정수화 (예: 0.01 유닛 단위 정밀도)
	// 20비트 최대값은 1,048,575 (약 10,000 유닛까지 커버 가능)
	uint32_t distInt = static_cast<uint32_t>(dist * 100.0f);
	return (distInt > 0xFFFFF) ? 0xFFFFF : distInt;
}

_RPKey RenderSystem::GenerateRenderPassHash(uint32_t hashPass, uint32_t hashShaders, uint32_t hashStates, uint32_t hashResources, uint32_t hashDist)
{
	_RPKey key = 0;

	// 1. 패스 (4비트)
	key |= (static_cast<uint64_t>(hashPass) & (UINT)0xF) << 60;

	// 2. 셰이더 (16비트)
	key |= (static_cast<uint64_t>(hashShaders) & (UINT)0xFFFF) << 44;

	// 3. 상태 (8비트)
	key |= (static_cast<uint64_t>(hashStates) & (UINT)0xFF) << 36;

	// 4. 메쉬/머티리얼/컬라이더(16비트)
	key |= (static_cast<uint64_t>(hashResources) & (UINT)0xFFFF) << 20;

	// 5. 거리 (20비트)
	//  패스에 따른 정렬 방향 결정
	// 투명의 경우에는 렌더타겟에 먼 객체부터 채워넣어야 하므로 값을 반전해 넣는다
	// 불투명의 경우에도 정렬을통해 거리가 가까운 객체부터그림으로써(early-z) gpu부하를 줄인다
	// 투명: 뒤에서부터 그려야 하므로 거리가 클수록 키값이 작아야 함 (반전)
	// 불투명: 앞에서부터 그려야 하므로 거리가 작을수록 키값이 작음 (그대로)
	if (static_cast<uint32_t>(E_RenderPass::Transparent) == hashPass)
		hashDist = 0xFFFFF - hashDist;
	key |= (static_cast<uint64_t>(hashDist) & (UINT)0xFFFFF);

	return key;
}


void RenderSystem::EnqueueRenderItem(_RPKey sortKey, Archetype* pArchetype, size_t entityRow, size_t entityCol, UINT renderCnt, UINT startIdx)
{
	m_hRP_CommandQueue.push_back({ sortKey, pArchetype, entityRow, entityCol, renderCnt, startIdx });
}

void RenderSystem::SortRenderItem()
{
	std::sort(m_hRP_CommandQueue.begin(), m_hRP_CommandQueue.end(), [&](const RenderItem& a, const RenderItem& b) { return a.sortKey < b.sortKey; });
}

void RenderSystem::ClearRenderItem()
{
	m_hRP_CommandQueue.clear();
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
				uint32_t hashDist = GetRenderPassKey_DistToCamera(distToCam);

				const auto& pRenderAsset = _ResourceSystem.GetResource<RenderAsset>(renders[col].hash_asset_Render);
				size_t hashMesh = pRenderAsset->m_hMeshMats.hash_mesh;
				BaseMesh* pMesh = _ResourceSystem.GetResource<BaseMesh>(hashMesh);

				E_Collider eCollider = pColliders ? (*pColliders)[col].type : E_Collider::BOX;

				//Collect Draw MeshMaterial
				for (UINT matIdx = 0; matIdx < pRenderAsset->m_hMeshMats.hash_mats.size(); matIdx++)
				{
					size_t hashMaterial = pRenderAsset->m_hMeshMats.hash_mats[matIdx];
					const auto& pMaterial = _ResourceSystem.GetResource<Material>(hashMaterial);
					uint32_t hashPass = pMaterial->GetHashPass();
					uint32_t hashShader = pMaterial->GetHashShaders();
					uint32_t hashStates = pMaterial->GetHashStates();
					uint32_t hashResources = GetRenderPassKey_Resources(hashMesh, hashMaterial, eCollider, 129);
					EnqueueRenderItem(GenerateRenderPassHash(hashPass, hashShader, hashStates, hashResources, hashDist), archetype, row, col, pMesh->GetRendIndices()[matIdx].count, pMesh->GetRendIndices()[matIdx].idx);
				}

				//Collect Draw Shadow
				if (passMasks & _ToMask32(E_RenderPass::Shadow))
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
						uint32_t hashResources = GetRenderPassKey_Resources(hashMesh, hashMaterial, eCollider, 130);
						EnqueueRenderItem(GenerateRenderPassHash(hashPass, hashShader, hashStates, hashResources, hashDist), archetype, row, col, pMesh->GetRendIndices()[matIdx].count, pMesh->GetRendIndices()[matIdx].idx);
					}
				}

				//Collect Draw Cubemap
				if (passMasks & _ToMask32(E_RenderPass::Cubemap))
				{
					const auto& pMaterial_Cubemap = _ResourceSystem.GetResource<Material>(GetHashMat_Cubemap(pMesh->GetVerticesType()));
					for (UINT matIdx = 0; matIdx < pRenderAsset->m_hMeshMats.hash_mats.size(); matIdx++)
					{
						size_t hashMaterial = pRenderAsset->m_hMeshMats.hash_mats[matIdx];
						const auto& pMaterial = _ResourceSystem.GetResource<Material>(hashMaterial);
						uint32_t hashPass = pMaterial_Cubemap->GetHashPass();
						uint32_t hashShader = pMaterial_Cubemap->GetHashShaders();
						uint32_t hashStates = pMaterial->GetHashStates();
						uint32_t hashResources = GetRenderPassKey_Resources(hashMesh, hashMaterial, eCollider, 131);
						EnqueueRenderItem(GenerateRenderPassHash(hashPass, hashShader, hashStates, hashResources, hashDist), archetype, row, col, pMesh->GetRendIndices()[matIdx].count, pMesh->GetRendIndices()[matIdx].idx);
					}
				}

				//Collect Draw Debug
				if (_InputSystem.IsDebugRender() && (passMasks & _ToMask32(E_RenderPass::Debug)))
				{
					size_t hashMaterial = GetHashMat_Debug(eCollider);
					const auto& pMaterial = _ResourceSystem.GetResource<Material>(hashMaterial);
					uint32_t hashPass = pMaterial->GetHashPass();
					uint32_t hashShader = pMaterial->GetHashShaders();
					uint32_t hashStates = pMaterial->GetHashStates();
					for (UINT colliderIdx = 0; colliderIdx < pMesh->GetCLs().size(); colliderIdx++)
					{
						uint32_t hashResources = GetRenderPassKey_Resources(hashMesh, hashMaterial, eCollider, colliderIdx);
						UINT renderCnt = (eCollider == E_Collider::BOX) ? 1 : 60;
						EnqueueRenderItem(GenerateRenderPassHash(hashPass, hashShader, hashStates, hashResources, hashDist), archetype, row, col, renderCnt, 0);
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
		uint32_t hashDist = GetRenderPassKey_DistToCamera(distToCam);

		const auto& pRenderAsset = _ResourceSystem.GetResource<RenderAsset>(render.hash_asset_Render);
		size_t hashMesh = pRenderAsset->m_hMeshMats.hash_mesh;
		BaseMesh* pMesh = _ResourceSystem.GetResource<BaseMesh>(hashMesh);

		E_Collider eCollider = collider.type;

		//Collect Picking Triangle
		{
			size_t hashMaterial = pRenderAsset->m_hMeshMats.hash_mats[collider.idxMat];
			const auto& pMaterial = _ResourceSystem.GetResource<Material>(hashMaterial);
			const auto& pMaterial_Picking = _ResourceSystem.GetResource<Material>(_EngineSystem.m_hash_Mat_Picking);
			uint32_t hashPass = pMaterial_Picking->GetHashPass();
			uint32_t hashShader = pMaterial->GetHashShaders();
			uint32_t hashStates = pMaterial_Picking->GetHashStates();
			uint32_t hashResources = GetRenderPassKey_Resources(hashMesh, hashMaterial, eCollider, 132);
			EnqueueRenderItem(GenerateRenderPassHash(hashPass, hashShader, hashStates, hashResources, hashDist), archetype, row, col, 3, collider.idxPicking);
		}

		//Collect Write Outline
		{
			for (UINT matIdx = 0; matIdx < pRenderAsset->m_hMeshMats.hash_mats.size(); matIdx++)
			{
				size_t hashMaterial = pRenderAsset->m_hMeshMats.hash_mats[matIdx];
				const auto& pMaterial = _ResourceSystem.GetResource<Material>(hashMaterial);
				uint32_t hashPass = pMaterial->GetHashPass();
				uint32_t hashShader = pMaterial->GetHashShaders();
				uint32_t hashStates = GetRenderPassKey_States(E_RSState::SOLID_CULLBACK_CW, E_DSState::Outline_Write, E_BSState::Outline_Write, 1);
				uint32_t hashResources = GetRenderPassKey_Resources(hashMesh, hashMaterial, eCollider, 133);
				EnqueueRenderItem(GenerateRenderPassHash(hashPass, hashShader, hashStates, hashResources, hashDist), archetype, row, col, pMesh->GetRendIndices()[matIdx].count, pMesh->GetRendIndices()[matIdx].idx);
			}
		}

		//Collect Draw Outline
		{
			size_t hashMaterial = GetHashMat_Outline(pMesh->GetVerticesType());
			const auto& pMaterial = _ResourceSystem.GetResource<Material>(hashMaterial);
			uint32_t hashPass = pMaterial->GetHashPass();
			uint32_t hashShader = pMaterial->GetHashShaders();
			uint32_t hashStates = pMaterial->GetHashStates();
			uint32_t hashResources = GetRenderPassKey_Resources(hashMesh, hashMaterial, eCollider, 134);
			for (UINT matIdx = 0; matIdx < pRenderAsset->m_hMeshMats.hash_mats.size(); matIdx++)
				EnqueueRenderItem(GenerateRenderPassHash(hashPass, hashShader, hashStates, hashResources, hashDist), archetype, row, col, pMesh->GetRendIndices()[matIdx].count, pMesh->GetRendIndices()[matIdx].idx);
		}
	}
}