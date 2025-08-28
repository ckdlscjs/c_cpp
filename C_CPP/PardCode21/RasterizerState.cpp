#include "RasterizerState.h"

RasterizerState::RasterizerState(ID3D11Device* pDevice)
{
	std::cout << "Initialize : " << "RasterizerState" << " Class" << '\n';

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

	//SOLID, CULLBACK, CW
	rasterizer_desc.FillMode = D3D11_FILL_SOLID;
	rasterizer_desc.CullMode = D3D11_CULL_BACK;
	rasterizer_desc.FrontCounterClockwise = FALSE;
	rasterizer_desc.DepthClipEnable = TRUE;                
	rasterizer_desc.MultisampleEnable = TRUE;              
	result = pDevice->CreateRasterizerState(&rasterizer_desc, &pState);
	_ASEERTION_NULCHK(pState, "SOLID, CULLBACK, CW");
	m_pStates.push_back(pState);

	//WIREFRAME, CULLBACK, CW
	rasterizer_desc.FillMode = D3D11_FILL_WIREFRAME;
	rasterizer_desc.CullMode = D3D11_CULL_BACK;
	result = pDevice->CreateRasterizerState(&rasterizer_desc, &pState);
	_ASEERTION_NULCHK(pState, "WIREFRAME, CULLBACK, CW");
	m_pStates.push_back(pState);

	//SOLID, CULLFRONT, CW
	rasterizer_desc.FillMode = D3D11_FILL_SOLID;
	rasterizer_desc.CullMode = D3D11_CULL_FRONT;
	result = pDevice->CreateRasterizerState(&rasterizer_desc, &pState);
	_ASEERTION_NULCHK(pState, "SOLID, CULLFRONT, CW");
	m_pStates.push_back(pState);

	//필요시 추가
}

RasterizerState::~RasterizerState()
{
	std::cout << "Release : " << "SamplerState" << " Class" << '\n';
	for (auto iter = m_pStates.begin(); iter != m_pStates.end();)
	{
		(*iter)->Release();
		iter = m_pStates.erase(iter);
	}
}

void RasterizerState::SetRS(ID3D11DeviceContext* pDeviceContext, UINT idx)
{
	pDeviceContext->RSSetState(m_pStates[idx]);
}
