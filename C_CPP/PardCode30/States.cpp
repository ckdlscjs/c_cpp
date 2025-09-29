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
