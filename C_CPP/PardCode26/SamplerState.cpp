#include "SamplerState.h"

SamplerState::SamplerState(ID3D11Device* pDevice)
{
	std::cout << "Initialize : " << "SamplerState" << " Class" << '\n';

	HRESULT result;
	D3D11_SAMPLER_DESC sampler_desc;
	ID3D11SamplerState* pSamplers;
	ZeroMemory(&sampler_desc, sizeof(D3D11_SAMPLER_DESC));

	//Wrap_Linear
	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	result = pDevice->CreateSamplerState(&sampler_desc, &pSamplers);
	_ASEERTION_NULCHK(pSamplers, "Wrap_Linear");
	m_pSamplers.push_back(pSamplers);

	//Wrap_Anisotrpic
	sampler_desc.Filter = D3D11_FILTER_ANISOTROPIC;
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	result = pDevice->CreateSamplerState(&sampler_desc, &pSamplers);
	_ASEERTION_NULCHK(pSamplers, "Wrap_Anisotropic");
	m_pSamplers.push_back(pSamplers);

	//�ʿ�� �߰�
	/*
	// Point_Clamp: �ȼ� ��Ʈ, ��������Ʈ, UI � ���
	D3D11_SAMPLER_DESC pointClampDesc = DefaultSamplerDesc();
	pointClampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	pointClampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	pointClampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	pointClampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	pDevice->CreateSamplerState(&pointClampDesc, &m_pSamplers[static_cast<size_t>(Samplers::POINT_CLAMP)]);
	assert(m_pSamplers[static_cast<size_t>(Samplers::POINT_CLAMP)]);

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

SamplerState::~SamplerState()
{
	std::cout << "Release : " << "SamplerState" << " Class" << '\n';
	for (auto iter = m_pSamplers.begin(); iter != m_pSamplers.end();)
	{
		(*iter)->Release();
		iter = m_pSamplers.erase(iter);
	}
}
/*
//void ID3D11DeviceContext::PSSetSamplers(
//	[in] UINT                   StartSlot,
//	[in] UINT                   NumSamplers,
//	[in] ID3D11SamplerState* const* ppSamplerStates
//);
//StartSlot: ���̴� �ڵ忡�� ���÷��� ���ε��� �������� ���� ��ȣ�Դϴ�.HLSL ���̴����� sampler s0, sampler s1 ������ ����� ���, �� sX�� X�� �ش��ϴ� ��ȣ�Դϴ�.
//NumSamplers : ���ε��� ���÷� ������Ʈ�� �����Դϴ�(���� 1��).
//ppSamplerStates : ���ε��� ID3D11SamplerState �����͵��� �迭�Դϴ�.
*/
void SamplerState::SetVS(ID3D11DeviceContext* pDeviceContext, E_Samplers sampler, UINT startIdx)
{
	pDeviceContext->VSSetSamplers(startIdx, 1, &m_pSamplers[(UINT)sampler]);
}

void SamplerState::SetPS(ID3D11DeviceContext* pDeviceContext, E_Samplers sampler, UINT startIdx)
{
	pDeviceContext->PSSetSamplers(startIdx, 1, &m_pSamplers[(UINT)sampler]);
}
