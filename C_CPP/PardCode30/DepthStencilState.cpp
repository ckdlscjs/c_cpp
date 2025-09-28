#include "DepthStencilState.h"

DepthStencilState::DepthStencilState(ID3D11Device* pDevice)
{
	std::cout << "Initialize : " << "DepthStencilState" << " Class" << '\n';
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

DepthStencilState::~DepthStencilState()
{
	std::cout << "Release : " << "DepthStencilState" << " Class" << '\n';
}

void DepthStencilState::SetDS(ID3D11DeviceContext* pDeviceContext, E_DSStates state, UINT stencilRef)
{
	pDeviceContext->OMSetDepthStencilState(m_pStates[UINT(state)], stencilRef);
}
