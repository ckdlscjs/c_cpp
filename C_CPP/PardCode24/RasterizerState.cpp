#include "RasterizerState.h"

RasterizerState::RasterizerState(ID3D11Device* pDevice)
{
	std::cout << "Initialize : " << "RasterizerState" << " Class" << '\n';

 //	  typedef struct D3D11_RASTERIZER_DESC	
 //   {
 //   D3D11_FILL_MODE FillMode;				//�������� ��� ä����ΰ�(sold/wireframe)
 //   D3D11_CULL_MODE CullMode;				//�ø�
 //   BOOL FrontCounterClockwise;			//true�� ccw�� �ո�, false�� cw���ո� �Ϲ������δ� cw 

 //   INT DepthBias;						//���̹��̾
 //   FLOAT DepthBiasClamp;					//
 //   FLOAT SlopeScaledDepthBias;			//

 //   BOOL DepthClipEnable;					//0~1(ndc)���� �������� �߶󳽴�, �Ϲ������� true��Ȱ��ȭ
 //   BOOL ScissorEnable;					//ȭ���� ���������� �ȼ��� ������, UI�� �̴ϸʵ� Ư�������� ������Ʈ�� ����, �Ϲ������� TRUE
 //   BOOL MultisampleEnable;				//��Ƽ���ø�(MSAA)
 //   BOOL AntialiasedLineEnable;			//MSAA����� line�� ��Ƽ�ٸ��������
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

	//�ʿ�� �߰�
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
