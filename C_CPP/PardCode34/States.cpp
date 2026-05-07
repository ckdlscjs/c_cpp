#include "States.h"

////////////////////////////////////////////////////////////////////////////////
// DepthStencilState
////////////////////////////////////////////////////////////////////////////////
DepthStencilState::DepthStencilState(ID3D11Device* pDevice)
{
	//typedef struct D3D11_DEPTH_STENCIL_DESC
	//{
	//	BOOL DepthEnable;						// 깊이 테스트 활성화 여부
	//	D3D11_DEPTH_WRITE_MASK DepthWriteMask;	// 깊이 버퍼에 값 쓰기 활성화 마스크
	//	D3D11_COMPARISON_FUNC DepthFunc;		// 깊이 테스트 비교 함수


	// 스텐실버퍼는 비트마스킹 값을 이용해서 결과만을 남긴다, 이를이용해 OMSET에서 마스킹값을 지정하고 비교연산자(OP)를 이용해 사용한다
	//	BOOL StencilEnable;						// 스텐실 테스트 활성화 여부
	//	UINT8 StencilReadMask;					// 스텐실 버퍼 읽기 마스크
	//	UINT8 StencilWriteMask;					// 스텐실 버퍼 쓰기 마스크

	
	//	D3D11_DEPTH_STENCILOP_DESC FrontFace;	// 앞면(Front-Facing) 폴리곤 스텐실 설정
	//	D3D11_DEPTH_STENCILOP_DESC BackFace;	// 뒷면(Back-Facing) 폴리곤 스텐실 설정
	//} D3D11_DEPTH_STENCIL_DESC;


	//// D3D11_DEPTH_STENCILOP_DESC 구조체
	//// 스텐실 테스트 결과에 따른 동작을 정의합니다.
	//typedef struct D3D11_DEPTH_STENCILOP_DESC
	//{
	//	D3D11_STENCIL_OP StencilFailOp;			// 스텐실 테스트 실패 시 작업
	//	D3D11_STENCIL_OP StencilDepthFailOp;	// 스텐실 통과, 깊이 테스트 실패 시 작업
	//	D3D11_STENCIL_OP StencilPassOp;			// 스텐실, 깊이 테스트 모두 통과 시 작업
	//	D3D11_COMPARISON_FUNC StencilFunc;		// 스텐실 테스트 비교 함수
	//} D3D11_DEPTH_STENCILOP_DESC;

	m_pStates.resize((UINT)E_DSState::COUNT);

	HRESULT result;
	D3D11_DEPTH_STENCIL_DESC depth_stencil_desc;
	ZeroMemory(&depth_stencil_desc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	depth_stencil_desc.DepthEnable = TRUE;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;				//픽셀이 기존보다 가깝거나 같을시
	depth_stencil_desc.StencilEnable = FALSE;

	//Default
	result = pDevice->CreateDepthStencilState(&depth_stencil_desc, &m_pStates[(UINT)E_DSState::Default]);
	_ASEERTION_NULCHK(m_pStates[(UINT)E_DSState::Default], "DEPTH, WRITEALL, LESSEQUAL");

	//SkyBox, Transparent objects(투명)
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;		//깊이버퍼에 값을 쓰지않는다
	result = pDevice->CreateDepthStencilState(&depth_stencil_desc, &m_pStates[(UINT)E_DSState::SkyBox]);
	_ASEERTION_NULCHK(m_pStates[(UINT)E_DSState::SkyBox], "DEPTH, MASKZERO, LESSEQUAL");

	//2D렌더링(화가알고리즘)
	depth_stencil_desc.DepthEnable = FALSE;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	result = pDevice->CreateDepthStencilState(&depth_stencil_desc, &m_pStates[(UINT)E_DSState::UI]);
	_ASEERTION_NULCHK(m_pStates[(UINT)E_DSState::UI], "UI");

	// -------------------------------------------------------------------------
	// [3] Outline_Write (피킹된 물체의 영역을 스텐실 버퍼에 기록)
	// -------------------------------------------------------------------------
	ZeroMemory(&depth_stencil_desc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	depth_stencil_desc.DepthEnable = TRUE;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	depth_stencil_desc.StencilEnable = TRUE;
	depth_stencil_desc.StencilReadMask = 0xFF;
	depth_stencil_desc.StencilWriteMask = 0xFF;

	// 앞면 설정: 스텐실/깊이 테스트 모두 통과 시 스텐실 값을 Ref 값으로 교체(REPLACE)
	depth_stencil_desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	depth_stencil_desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS; // 무조건 통과해서 기록

	// 뒷면도 동일하게 설정 (단면 렌더링 시에는 큰 의미 없으나 안전하게 설정)
	depth_stencil_desc.BackFace = depth_stencil_desc.FrontFace;

	result = pDevice->CreateDepthStencilState(&depth_stencil_desc, &m_pStates[(UINT)E_DSState::Outline_Write]);
	_ASEERTION_NULCHK(m_pStates[(UINT)E_DSState::Outline_Write], "Outline_Write");


	// -------------------------------------------------------------------------
	// [4] Outline_Draw (기록된 영역을 제외한 나머지 외곽선만 렌더링)
	// -------------------------------------------------------------------------
	ZeroMemory(&depth_stencil_desc, sizeof(D3D11_DEPTH_STENCIL_DESC));

	//// 물체에 가려져도 스텐실버퍼를 그리는경우
	//// 아웃라인은 보통 물체 뒤에 가려져도 보여야 하므로 DepthEnable을 FALSE로 하거나 
	//// DepthFunc을 ALWAYS로 설정하는 경우가 많습니다.
	//depth_stencil_desc.DepthEnable = FALSE;
	//depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;

	// 물체에 가려지면 해당 버퍼를 표기하지 않는경우
	// 수정: 깊이 테스트를 활성화하여 다른 물체에 가려지면 안 그리도록 합니다.
	// 아웃라인 자체가 깊이 버퍼를 갱신해서 뒤에 그릴 다른 물체에 영향을 주면 안 되므로 ZERO
	// 기존 깊이값(다른 물체)보다 가깝거나 같을 때만 그립니다.
	depth_stencil_desc.DepthEnable = TRUE;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	depth_stencil_desc.StencilEnable = TRUE;
	depth_stencil_desc.StencilReadMask = 0xFF;
	depth_stencil_desc.StencilWriteMask = 0xFF;

	// 앞면 설정: 스텐실 값이 기록된 값(Ref)과 '같지 않을 때만' 픽셀을 그립니다.
	depth_stencil_desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.FrontFace.StencilFunc = D3D11_COMPARISON_NOT_EQUAL;

	depth_stencil_desc.BackFace = depth_stencil_desc.FrontFace;

	result = pDevice->CreateDepthStencilState(&depth_stencil_desc, &m_pStates[(UINT)E_DSState::Outline_Draw]);
	_ASEERTION_NULCHK(m_pStates[(UINT)E_DSState::Outline_Draw], "Outline_Draw");

	//필요시 추가
}

ID3D11DepthStencilState* DepthStencilState::GetState(E_DSState stateIdx)
{
	return BaseState::GetState(static_cast<UINT>(stateIdx));
}

////////////////////////////////////////////////////////////////////////////////
// SamplerState
////////////////////////////////////////////////////////////////////////////////
SamplerState::SamplerState(ID3D11Device* pDevice)
{
	m_pStates.resize((UINT)E_SMState::COUNT);
	HRESULT result;
	D3D11_SAMPLER_DESC sampler_desc;
	ZeroMemory(&sampler_desc, sizeof(D3D11_SAMPLER_DESC));

	//Linear_Wrap
	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	result = pDevice->CreateSamplerState(&sampler_desc, &m_pStates[(UINT)E_SMState::LINEAR_WRAP]);
	_ASEERTION_NULCHK(m_pStates[(UINT)E_SMState::LINEAR_WRAP], "Linear_Wrap");

	//Anisotrpic_Wrap
	sampler_desc.Filter = D3D11_FILTER_ANISOTROPIC;
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	result = pDevice->CreateSamplerState(&sampler_desc, &m_pStates[(UINT)E_SMState::ANISOTROPIC_WRAP]);
	_ASEERTION_NULCHK(m_pStates[(UINT)E_SMState::ANISOTROPIC_WRAP], "Anisotropic_Wrap");


	//Point_Clamp, 픽셀 아트, 스프라이트, UI 등에 사용
	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT; // 깊이 버퍼 샘플링에는 Point 필터가 적합합니다.
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	result = pDevice->CreateSamplerState(&sampler_desc, &m_pStates[(UINT)E_SMState::POINT_CLAMP]);
	_ASEERTION_NULCHK(m_pStates[(UINT)E_SMState::POINT_CLAMP], "Point_Clamp");

	//그림자를위한 비교 SAMPLER
	sampler_desc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT; 
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampler_desc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
	sampler_desc.MinLOD = 0;
	sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;
	result = pDevice->CreateSamplerState(&sampler_desc, &m_pStates[(UINT)E_SMState::POINT_CLAMP_COMPARISON]);
	_ASEERTION_NULCHK(m_pStates[(UINT)E_SMState::POINT_CLAMP_COMPARISON], "Point_Clamp_Comparison");


	/*
	//필요시 추가
	// Linear_Clamp: 그림자 맵, 라이트 맵, 데칼 등에 사용
	D3D11_SAMPLER_DESC linearClampDesc = DefaultSamplerDesc();
	linearClampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	linearClampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	linearClampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	linearClampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	pDevice->CreateSamplerState(&linearClampDesc, &m_pSamplers[static_cast<size_t>(Samplers::LINEAR_CLAMP)]);
	assert(m_pSamplers[static_cast<size_t>(Samplers::LINEAR_CLAMP)]);

	// Anisotropic_Clamp: 텍스처가 비스듬히 보이지만 반복되면 안 되는 경우 사용
	D3D11_SAMPLER_DESC anisotropicClampDesc = DefaultSamplerDesc();
	anisotropicClampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	anisotropicClampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	anisotropicClampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	anisotropicClampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	anisotropicClampDesc.MaxAnisotropy = 16;
	pDevice->CreateSamplerState(&anisotropicClampDesc, &m_pSamplers[static_cast<size_t>(Samplers::ANISOTROPIC_CLAMP)]);
	assert(m_pSamplers[static_cast<size_t>(Samplers::ANISOTROPIC_CLAMP)]);
	*/
}

ID3D11SamplerState* SamplerState::GetState(E_SMState stateIdx)
{
	return BaseState::GetState(static_cast<UINT>(stateIdx));
}

////////////////////////////////////////////////////////////////////////////////
// RasterizerState
////////////////////////////////////////////////////////////////////////////////
RasterizerState::RasterizerState(ID3D11Device* pDevice)
{
	//	  typedef struct D3D11_RASTERIZER_DESC	
	//   {
	//   D3D11_FILL_MODE FillMode;				//폴리곤을 어떻게 채울것인가(sold/wireframe)
	//   D3D11_CULL_MODE CullMode;				//컬링
	//   BOOL FrontCounterClockwise;			//true시 ccw를 앞면, false시 cw를앞면 일반적으로는 cw 

	//   INT DepthBias;							//깊이바이어스, 24비트기준으로 일반적으로 100~1000의값을사용한다
	//   FLOAT DepthBiasClamp;					//바이어스가 너무 커져 물체가 벽을 뚫고 보이는 부작용등을 막기위한 한계치
	//   FLOAT SlopeScaledDepthBias;			//경사면에 비례해 바이어스를 가변적으로 사용하기 위한 옵션값, 일반적으로 평면보다 경사면이 깊이여드름이 더 자주생긴다

	//   BOOL DepthClipEnable;					//0~1(ndc)밖의 폴리곤을 잘라낸다, 일반적으로 true로활성화
	//   BOOL ScissorEnable;					//화면의 지정영역의 픽셀만 렌더링, UI나 미니맵등 특정영역의 업데이트시 유용, 일반적으로 TRUE
	//   BOOL MultisampleEnable;				//멀티샘플링(MSAA)
	//   BOOL AntialiasedLineEnable;			//MSAA사용대신 line에 안티앨리어싱적용
	//   } 	D3D11_RASTERIZER_DESC;
	m_pStates.resize((UINT)E_RSState::COUNT);
	HRESULT result;
	D3D11_RASTERIZER_DESC rasterizer_desc;
	ZeroMemory(&rasterizer_desc, sizeof(D3D11_RASTERIZER_DESC));

	rasterizer_desc.DepthClipEnable = TRUE;
	rasterizer_desc.DepthBias = 1000;
	rasterizer_desc.DepthBiasClamp = 0.0f;
	rasterizer_desc.SlopeScaledDepthBias = 2.0f;
	//rasterizer_desc.ScissorEnable = TRUE;
	rasterizer_desc.MultisampleEnable = TRUE;
	rasterizer_desc.AntialiasedLineEnable = TRUE;
	
	//SOLID, CULLBACK, CW
	rasterizer_desc.FillMode = D3D11_FILL_SOLID;
	rasterizer_desc.CullMode = D3D11_CULL_BACK;
	rasterizer_desc.FrontCounterClockwise = FALSE;
	result = pDevice->CreateRasterizerState(&rasterizer_desc, &m_pStates[(UINT)E_RSState::SOLID_CULLBACK_CW]);
	_ASEERTION_NULCHK(m_pStates[(UINT)E_RSState::SOLID_CULLBACK_CW], "SOLID, CULLBACK, CW");

	//SOLID, CULLBACK, CCW
	rasterizer_desc.FillMode = D3D11_FILL_SOLID;
	rasterizer_desc.CullMode = D3D11_CULL_BACK;
	rasterizer_desc.FrontCounterClockwise = TRUE;
	result = pDevice->CreateRasterizerState(&rasterizer_desc, &m_pStates[(UINT)E_RSState::SOLID_CULLBACK_CCW]);
	_ASEERTION_NULCHK(m_pStates[(UINT)E_RSState::SOLID_CULLBACK_CCW], "SOLID, CULLBACK, CCW");

	//SOLID, CULLFRONT, CW
	rasterizer_desc.FillMode = D3D11_FILL_SOLID;
	rasterizer_desc.CullMode = D3D11_CULL_FRONT;
	rasterizer_desc.FrontCounterClockwise = FALSE;
	result = pDevice->CreateRasterizerState(&rasterizer_desc, &m_pStates[(UINT)E_RSState::SOLID_CULLFRONT_CW]);
	_ASEERTION_NULCHK(m_pStates[(UINT)E_RSState::SOLID_CULLFRONT_CW], "SOLID, CULLFRONT, CW");

	//SOLID, CULLFRONT, CCW
	rasterizer_desc.FillMode = D3D11_FILL_SOLID;
	rasterizer_desc.CullMode = D3D11_CULL_FRONT;
	rasterizer_desc.FrontCounterClockwise = TRUE;
	result = pDevice->CreateRasterizerState(&rasterizer_desc, &m_pStates[(UINT)E_RSState::SOLID_CULLFRONT_CCW]);
	_ASEERTION_NULCHK(m_pStates[(UINT)E_RSState::SOLID_CULLFRONT_CCW], "SOLID, CULLFRONT, CCW");

	//WIREFRAME, CULLBACK, CW
	rasterizer_desc.FillMode = D3D11_FILL_WIREFRAME;
	rasterizer_desc.CullMode = D3D11_CULL_BACK;
	rasterizer_desc.FrontCounterClockwise = FALSE;
	result = pDevice->CreateRasterizerState(&rasterizer_desc, &m_pStates[(UINT)E_RSState::WIRE_CULLBACK_CW]);
	_ASEERTION_NULCHK(m_pStates[(UINT)E_RSState::WIRE_CULLBACK_CW], "WIREFRAME, CULLBACK, CW");

	//필요시 추가
}

ID3D11RasterizerState* RasterizerState::GetState(E_RSState stateIdx)
{
	return BaseState::GetState(static_cast<UINT>(stateIdx));;
}

////////////////////////////////////////////////////////////////////////////////
// BlendState
////////////////////////////////////////////////////////////////////////////////
BlendState::BlendState(ID3D11Device* pDevice)
{
	// 블렌드 상태 구조체를 초기화 합니다.
	/*
	* FinalColor= (SrcColor⋅SrcFactor) blendop (DestColor⋅DestFactor) 로 정의된다, blendop(연산자)
	*	typedef struct D3D11_BLEND_DESC
		{
		BOOL AlphaToCoverageEnable;							//MSAA사용시설정
		BOOL IndependentBlendEnable;						//렌더타겟개별설정할지 통합할지
		D3D11_RENDER_TARGET_BLEND_DESC RenderTarget[ 8 ];	//최대8개
		} 	D3D11_BLEND_DESC;

		typedef struct D3D11_RENDER_TARGET_BLEND_DESC
		{
		BOOL BlendEnable;									//블렌딩활성화여부
		D3D11_BLEND SrcBlend;								//SrcFactor, DescFactor를 의미한다, rgb를의미
		D3D11_BLEND DestBlend;
		D3D11_BLEND_OP BlendOp;								//RGB의 blendOperator
		D3D11_BLEND SrcBlendAlpha;							//SrcFactor, DescFactor를 의미한다, a를의미
		D3D11_BLEND DestBlendAlpha;
		D3D11_BLEND_OP BlendOpAlpha;						//A의 blendOperator
		UINT8 RenderTargetWriteMask;						//렌더타겟의 R,G,B,A채널중 어떤채널을 쓸것인가 지정한다, 기본은 WRITE_ENABLE_ALL, 즉 모든채널을쓴다
		} 	D3D11_RENDER_TARGET_BLEND_DESC;

		D3D11_BLEND_ZERO			(0,0,0,0)								해당 색상을 최종 결과에서 완전히 제외합니다.
		D3D11_BLEND_ONE				(1,1,1,1)								해당 색상을 그대로 사용합니다.
		D3D11_BLEND_SRC_COLOR		(SrcR,SrcG,SrcB,SrcA)					Source의 색상 채널 값을 계수로 사용.
		D3D11_BLEND_INV_SRC_COLOR	(1−SrcR,1−SrcG,1−SrcB,1−SrcA)			Source 색상의 반전 값을 계수로 사용.
		D3D11_BLEND_SRC_ALPHA		(SrcA,SrcA,SrcA,SrcA)					Source의 알파 값을 $\text{RGB}$와 A 채널 모두의 계수로 사용. 가장 일반적인 투명(알파) 블렌딩의 $\text{SrcFactor}$입니다.
		D3D11_BLEND_INV_SRC_ALPHA	(1−SrcA,1−SrcA,1−SrcA,1−SrcA)			1에서 Source 알파 값을 뺀 값을 계수로 사용. 가장 일반적인 투명(알파) 블렌딩의 $\text{DestFactor}$입니다.
		D3D11_BLEND_DEST_COLOR		(DestR,DestG,DestB,DestA)				Destination의 색상 채널 값을 계수로 사용.
		D3D11_BLEND_INV_DEST_ALPHA	(1−DestA,1−DestA,1−DestA,1−DestA)		Destination 알파 값의 반전 값을 계수로 사용.
		D3D11_BLEND_BLEND_FACTOR	OMSetBlendState에서 지정된 BlendFactor 값	프로그램 코드로 임의의 색상/알파 값을 계수로 사용합니다.

		blendfactor->D3D11_BLEND_INV_BLEND_FACTOR 사용시에 지정값사용됨
		비트가 1일 때 : 해당 샘플에 색상 쓰기(Write) 허용.
		비트가 0일 때 : 해당 샘플에 색상 쓰기(Write) 금지.
		3. 가장 일반적인 사용법 : 0xFFFFFFFF 또는 - 1
	*/
	m_pStates.resize((UINT)E_BSState::COUNT);
	HRESULT result;
	D3D11_BLEND_DESC blend_desc;
	ZeroMemory(&blend_desc, sizeof(D3D11_BLEND_DESC));

	blend_desc.AlphaToCoverageEnable = FALSE;		//MSAA
	blend_desc.IndependentBlendEnable = FALSE;		//개별

	// 알파 블렌드를 비활성화 설정합니다(Opaque)
	blend_desc.RenderTarget[0].BlendEnable = FALSE;
	blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL; //0x0F, 8바이트변수
	result = pDevice->CreateBlendState(&blend_desc, &m_pStates[(UINT)E_BSState::Opaque]);
	_ASEERTION_NULCHK(m_pStates[(UINT)E_BSState::Opaque], "Opaque");

	// 알파블렌드 값을 설정합니다(Transparent)
	// 용도: 투명한 객체 (유리, 나뭇잎, 투명 UI 등) 렌더링.
	// 공식: FinalColor = (SrcColor * SrcA) + (DestColor * (1 - SrcA))
	//RGB
	blend_desc.RenderTarget[0].BlendEnable = TRUE;
	blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	//Alpha
	blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	result = pDevice->CreateBlendState(&blend_desc, &m_pStates[(UINT)E_BSState::Transparent]);
	_ASEERTION_NULCHK(m_pStates[(UINT)E_BSState::Transparent], "Transparent");

	// 가산블렌딩 AdditiveBlending
	// 용도: 발광체, 불꽃, 폭발, 마법 효과 등 밝은 효과 렌더링. 색상을 누적시켜 밝아집니다.
	// 공식: FinalColor = (SrcColor * 1) + (DestColor * 1)
	// RenderTarget[0] 설정
	blend_desc.RenderTarget[0].BlendEnable = TRUE; // 블렌딩 활성화

	// 색상 블렌딩 설정 (RGB)
	blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;          // Src 계수 = 1 (Source 색상 그대로 사용)
	blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;         // Dest 계수 = 1 (Destination 색상 그대로 사용)
	blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;        // 연산 = 덧셈 (두 색상을 단순히 더함)

	// 알파 블렌딩 설정 (A) - 보통 투명도는 사용하지 않거나, D3D11_BLEND_ONE으로 설정
	blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	result = pDevice->CreateBlendState(&blend_desc, &m_pStates[(UINT)E_BSState::Additive]);
	_ASEERTION_NULCHK(m_pStates[(UINT)E_BSState::Additive], "AdditiveBlending");

	// -------------------------------------------------------------------------
	// [4] Outline_Write (색상 출력 금지, 스텐실 마킹 전용)
	// 용도: 스텐실 버퍼에 영역만 기록하고 RTV의 기존 색상은 보존함.
	// -------------------------------------------------------------------------
	ZeroMemory(&blend_desc, sizeof(D3D11_BLEND_DESC));
	blend_desc.AlphaToCoverageEnable = FALSE;
	blend_desc.IndependentBlendEnable = FALSE;

	blend_desc.RenderTarget[0].BlendEnable = FALSE; // 블렌딩 불필요
	// 핵심: 모든 채널(R, G, B, A) 쓰기를 금지함. 
	// 이 상태에서 그리면 픽셀 셰이더가 돌아도 RTV에는 아무 변화가 없습니다.
	blend_desc.RenderTarget[0].RenderTargetWriteMask = 0;

	result = pDevice->CreateBlendState(&blend_desc, &m_pStates[(UINT)E_BSState::Outline_Write]);
	_ASEERTION_NULCHK(m_pStates[(UINT)E_BSState::Outline_Write], "Outline_Write");


	// -------------------------------------------------------------------------
	// [5] Outline_Draw (일반 불투명 렌더링과 동일하나 명시적으로 분리)
	// 용도: 실제 아웃라인 색상을 RTV에 덮어씌움.
	// -------------------------------------------------------------------------
	ZeroMemory(&blend_desc, sizeof(D3D11_BLEND_DESC));
	blend_desc.AlphaToCoverageEnable = FALSE;
	blend_desc.IndependentBlendEnable = FALSE;

	// 아웃라인은 보통 불투명하게(Opaque) 덧칠하므로 BlendEnable은 FALSE입니다.
	blend_desc.RenderTarget[0].BlendEnable = FALSE;
	blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	result = pDevice->CreateBlendState(&blend_desc, &m_pStates[(UINT)E_BSState::Outline_Draw]);
	_ASEERTION_NULCHK(m_pStates[(UINT)E_BSState::Outline_Draw], "Outline_Draw");
}

ID3D11BlendState* BlendState::GetState(E_BSState stateIdx)
{
	return BaseState::GetState(static_cast<UINT>(stateIdx));
}
