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


	HRESULT result;
	ID3D11DepthStencilState* pState;
	D3D11_DEPTH_STENCIL_DESC depth_stencil_desc;
	ZeroMemory(&depth_stencil_desc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	depth_stencil_desc.DepthEnable = TRUE;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;				//픽셀이 기존보다 가깝거나 같을시
	depth_stencil_desc.StencilEnable = FALSE;

	//DEFAULT
	result = pDevice->CreateDepthStencilState(&depth_stencil_desc, &pState);
	_ASEERTION_NULCHK(pState, "DEPTH, WRITEALL, LESSEQUAL");
	m_pStates.push_back(pState);

	//SKYBOX, Transparent objects(투명)
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;		//깊이버퍼에 값을 쓰지않는다
	result = pDevice->CreateDepthStencilState(&depth_stencil_desc, &pState);
	_ASEERTION_NULCHK(pState, "DEPTH, MASKZERO, LESSEQUAL");
	m_pStates.push_back(pState);

	//2D렌더링(화가알고리즘)
	depth_stencil_desc.DepthEnable = FALSE;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	result = pDevice->CreateDepthStencilState(&depth_stencil_desc, &pState);
	_ASEERTION_NULCHK(pState, "UI");
	m_pStates.push_back(pState);

	/*
	//스텐실버퍼에 오브젝트그리기
	depthDesc.DepthEnable = TRUE;
	depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;

	depthDesc.StencilEnable = TRUE;
	depthDesc.StencilReadMask = 0xFF;
	depthDesc.StencilWriteMask = 0xFF;

	// 스텐실 테스트를 무조건 통과시키고, 스텐실 버퍼에 1을 씁니다.
	depthDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depthDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE; // 1로 대체
	depthDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	depthDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depthDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	depthDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	*/

	/*
	//스텐실버퍼를이용해 외곽선 그리기
	depthDesc.DepthEnable = TRUE;
	depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO; // 깊이 쓰기 금지
	depthDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthDesc.StencilEnable = TRUE;
	depthDesc.StencilReadMask = 0xFF;
	depthDesc.StencilWriteMask = 0xFF;

	// 스텐실 값이 1과 같지 않은 경우에만 통과하여 외곽선을 그립니다.
	depthDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depthDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDesc.FrontFace.StencilFunc = D3D11_COMPARISON_NOT_EQUAL; // 1과 같지 않으면 통과

	depthDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depthDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDesc.BackFace.StencilFunc = D3D11_COMPARISON_NOT_EQUAL;
	*/
	//필요시 추가
}

ID3D11DepthStencilState* DepthStencilState::GetState(E_DSStates stateIdx)
{
	return BaseState::GetState(static_cast<UINT>(stateIdx));
}

////////////////////////////////////////////////////////////////////////////////
// SamplerState
////////////////////////////////////////////////////////////////////////////////
SamplerState::SamplerState(ID3D11Device* pDevice)
{
	HRESULT result;
	D3D11_SAMPLER_DESC sampler_desc;
	ID3D11SamplerState* pSamplers;
	ZeroMemory(&sampler_desc, sizeof(D3D11_SAMPLER_DESC));

	//Linear_Wrap
	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	result = pDevice->CreateSamplerState(&sampler_desc, &pSamplers);
	_ASEERTION_NULCHK(pSamplers, "Linear_Wrap");
	m_pStates.push_back(pSamplers);

	//Anisotrpic_Wrap
	sampler_desc.Filter = D3D11_FILTER_ANISOTROPIC;
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	result = pDevice->CreateSamplerState(&sampler_desc, &pSamplers);
	_ASEERTION_NULCHK(pSamplers, "Anisotropic_Wrap");
	m_pStates.push_back(pSamplers);


	//Point_Clamp, 픽셀 아트, 스프라이트, UI 등에 사용
	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT; // 깊이 버퍼 샘플링에는 Point 필터가 적합합니다.
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	result = pDevice->CreateSamplerState(&sampler_desc, &pSamplers);
	_ASEERTION_NULCHK(pSamplers, "Point_Clamp");
	m_pStates.push_back(pSamplers);


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

ID3D11SamplerState* SamplerState::GetState(E_Samplers stateIdx)
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

	//   INT DepthBias;						//깊이바이어스
	//   FLOAT DepthBiasClamp;					//
	//   FLOAT SlopeScaledDepthBias;			//

	//   BOOL DepthClipEnable;					//0~1(ndc)밖의 폴리곤을 잘라낸다, 일반적으로 true로활성화
	//   BOOL ScissorEnable;					//화면의 지정영역의 픽셀만 렌더링, UI나 미니맵등 특정영역의 업데이트시 유용, 일반적으로 TRUE
	//   BOOL MultisampleEnable;				//멀티샘플링(MSAA)
	//   BOOL AntialiasedLineEnable;			//MSAA사용대신 line에 안티앨리어싱적용
	//   } 	D3D11_RASTERIZER_DESC;

	HRESULT result;
	D3D11_RASTERIZER_DESC rasterizer_desc;
	ID3D11RasterizerState* pState;
	ZeroMemory(&rasterizer_desc, sizeof(D3D11_RASTERIZER_DESC));

	rasterizer_desc.FrontCounterClockwise = FALSE;
	rasterizer_desc.DepthClipEnable = TRUE;
	rasterizer_desc.MultisampleEnable = TRUE;

	//SOLID, CULLBACK, CW
	rasterizer_desc.FillMode = D3D11_FILL_SOLID;
	rasterizer_desc.CullMode = D3D11_CULL_BACK;
	result = pDevice->CreateRasterizerState(&rasterizer_desc, &pState);
	_ASEERTION_NULCHK(pState, "SOLID, CULLBACK, CW");
	m_pStates.push_back(pState);

	//SOLID, CULLFRONT, CW
	rasterizer_desc.FillMode = D3D11_FILL_SOLID;
	rasterizer_desc.CullMode = D3D11_CULL_FRONT;
	result = pDevice->CreateRasterizerState(&rasterizer_desc, &pState);
	_ASEERTION_NULCHK(pState, "SOLID, CULLFRONT, CW");
	m_pStates.push_back(pState);

	//WIREFRAME, CULLBACK, CW
	rasterizer_desc.FillMode = D3D11_FILL_WIREFRAME;
	rasterizer_desc.CullMode = D3D11_CULL_BACK;
	result = pDevice->CreateRasterizerState(&rasterizer_desc, &pState);
	_ASEERTION_NULCHK(pState, "WIREFRAME, CULLBACK, CW");
	m_pStates.push_back(pState);

	//필요시 추가
}

ID3D11RasterizerState* RasterizerState::GetState(E_RSStates stateIdx)
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
	HRESULT result;
	D3D11_BLEND_DESC blend_desc;
	ID3D11BlendState* pState;
	ZeroMemory(&blend_desc, sizeof(D3D11_BLEND_DESC));

	blend_desc.AlphaToCoverageEnable = FALSE;		//MSAA
	blend_desc.IndependentBlendEnable = FALSE;		//개별

	// 알파 블렌드를 비활성화 설정합니다(Opaque)
	blend_desc.RenderTarget[0].BlendEnable = FALSE;
	blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL; //0x0F, 8바이트변수
	result = pDevice->CreateBlendState(&blend_desc, &pState);
	_ASEERTION_NULCHK(pState, "Opaque");
	m_pStates.push_back(pState);

	// 알파블렌드 값을 설정합니다(Transparent)
	// // 용도: 투명한 객체 (유리, 나뭇잎, 투명 UI 등) 렌더링.
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
	result = pDevice->CreateBlendState(&blend_desc, &pState);
	_ASEERTION_NULCHK(pState, "Transparent");
	m_pStates.push_back(pState);

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
	result = pDevice->CreateBlendState(&blend_desc, &pState);
	_ASEERTION_NULCHK(pState, "AdditiveBlending");
	m_pStates.push_back(pState);
}

ID3D11BlendState* BlendState::GetState(E_BSStates stateIdx)
{
	return BaseState::GetState(static_cast<UINT>(stateIdx));
}
