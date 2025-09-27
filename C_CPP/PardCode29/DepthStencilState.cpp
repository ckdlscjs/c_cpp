#include "DepthStencilState.h"

DepthStencilState::DepthStencilState(ID3D11Device* pDevice)
{
	std::cout << "Initialize : " << "DepthStencilState" << " Class" << '\n';
	//typedef struct D3D11_DEPTH_STENCIL_DESC
	//{
	//	BOOL DepthEnable;						// ���� �׽�Ʈ Ȱ��ȭ ����
	//	D3D11_DEPTH_WRITE_MASK DepthWriteMask;	// ���� ���ۿ� �� ���� Ȱ��ȭ ����ũ
	//	D3D11_COMPARISON_FUNC DepthFunc;		// ���� �׽�Ʈ �� �Լ�

	//	BOOL StencilEnable;						// ���ٽ� �׽�Ʈ Ȱ��ȭ ����
	//	UINT8 StencilReadMask;					// ���ٽ� ���� �б� ����ũ
	//	UINT8 StencilWriteMask;					// ���ٽ� ���� ���� ����ũ

	//	D3D11_DEPTH_STENCILOP_DESC FrontFace;	// �ո�(Front-Facing) ������ ���ٽ� ����
	//	D3D11_DEPTH_STENCILOP_DESC BackFace;	// �޸�(Back-Facing) ������ ���ٽ� ����
	//} D3D11_DEPTH_STENCIL_DESC;


	//// D3D11_DEPTH_STENCILOP_DESC ����ü
	//// ���ٽ� �׽�Ʈ ����� ���� ������ �����մϴ�.
	//typedef struct D3D11_DEPTH_STENCILOP_DESC
	//{
	//	D3D11_STENCIL_OP StencilFailOp;			// ���ٽ� �׽�Ʈ ���� �� �۾�
	//	D3D11_STENCIL_OP StencilDepthFailOp;	// ���ٽ� ���, ���� �׽�Ʈ ���� �� �۾�
	//	D3D11_STENCIL_OP StencilPassOp;			// ���ٽ�, ���� �׽�Ʈ ��� ��� �� �۾�
	//	D3D11_COMPARISON_FUNC StencilFunc;		// ���ٽ� �׽�Ʈ �� �Լ�
	//} D3D11_DEPTH_STENCILOP_DESC;

	
	HRESULT result;
	ID3D11DepthStencilState* pState;
	D3D11_DEPTH_STENCIL_DESC depth_stencil_desc;
	ZeroMemory(&depth_stencil_desc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	depth_stencil_desc.DepthEnable = TRUE;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;			
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;				//�ȼ��� �������� �����ų� ������
	depth_stencil_desc.StencilEnable = FALSE;

	//DEFAULT
	result = pDevice->CreateDepthStencilState(&depth_stencil_desc, &pState);
	_ASEERTION_NULCHK(pState, "DEPTH, WRITEALL, LESSEQUAL");
	m_pStates.push_back(pState);

	//SKYBOX, Transparent objects(����)
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;		//���̹��ۿ� ���� �����ʴ´�
	result = pDevice->CreateDepthStencilState(&depth_stencil_desc, &pState);
	_ASEERTION_NULCHK(pState, "DEPTH, MASKZERO, LESSEQUAL");
	m_pStates.push_back(pState);

	//2D������(ȭ���˰���)
	depth_stencil_desc.DepthEnable = FALSE;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	result = pDevice->CreateDepthStencilState(&depth_stencil_desc, &pState);
	_ASEERTION_NULCHK(pState, "UI");
	m_pStates.push_back(pState);

	/*
	//���ٽǹ��ۿ� ������Ʈ�׸���
	depthDesc.DepthEnable = TRUE;
	depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;

	depthDesc.StencilEnable = TRUE;
	depthDesc.StencilReadMask = 0xFF;
	depthDesc.StencilWriteMask = 0xFF;

	// ���ٽ� �׽�Ʈ�� ������ �����Ű��, ���ٽ� ���ۿ� 1�� ���ϴ�.
	depthDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depthDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE; // 1�� ��ü
	depthDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	depthDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depthDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	depthDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	*/

	/*
	//���ٽǹ��۸��̿��� �ܰ��� �׸���
	depthDesc.DepthEnable = TRUE;
	depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO; // ���� ���� ����
	depthDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthDesc.StencilEnable = TRUE;
	depthDesc.StencilReadMask = 0xFF;
	depthDesc.StencilWriteMask = 0xFF;

	// ���ٽ� ���� 1�� ���� ���� ��쿡�� ����Ͽ� �ܰ����� �׸��ϴ�.
	depthDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depthDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDesc.FrontFace.StencilFunc = D3D11_COMPARISON_NOT_EQUAL; // 1�� ���� ������ ���

	depthDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depthDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDesc.BackFace.StencilFunc = D3D11_COMPARISON_NOT_EQUAL;
	*/
	//�ʿ�� �߰�
}

DepthStencilState::~DepthStencilState()
{
	std::cout << "Release : " << "DepthStencilState" << " Class" << '\n';
}

void DepthStencilState::SetDS(ID3D11DeviceContext* pDeviceContext, E_DSStates state, UINT stencilRef)
{
	pDeviceContext->OMSetDepthStencilState(m_pStates[UINT(state)], stencilRef);
}
