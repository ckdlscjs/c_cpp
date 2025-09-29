#include "States.h"

////////////////////////////////////////////////////////////////////////////////
// DepthStencilState
////////////////////////////////////////////////////////////////////////////////
DepthStencilState::DepthStencilState(ID3D11Device* pDevice)
{
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


	//Point_Clamp, �ȼ� ��Ʈ, ��������Ʈ, UI � ���
	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT; // ���� ���� ���ø����� Point ���Ͱ� �����մϴ�.
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	result = pDevice->CreateSamplerState(&sampler_desc, &pSamplers);
	_ASEERTION_NULCHK(pSamplers, "Point_Clamp");
	m_pStates.push_back(pSamplers);


	/*
	//�ʿ�� �߰�
	// Linear_Clamp: �׸��� ��, ����Ʈ ��, ��Į � ���
	D3D11_SAMPLER_DESC linearClampDesc = DefaultSamplerDesc();
	linearClampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	linearClampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	linearClampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	linearClampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	pDevice->CreateSamplerState(&linearClampDesc, &m_pSamplers[static_cast<size_t>(Samplers::LINEAR_CLAMP)]);
	assert(m_pSamplers[static_cast<size_t>(Samplers::LINEAR_CLAMP)]);

	// Anisotropic_Clamp: �ؽ�ó�� �񽺵��� �������� �ݺ��Ǹ� �� �Ǵ� ��� ���
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

	//�ʿ�� �߰�
}

ID3D11RasterizerState* RasterizerState::GetState(E_RSStates stateIdx)
{
	return BaseState::GetState(static_cast<UINT>(stateIdx));;
}
