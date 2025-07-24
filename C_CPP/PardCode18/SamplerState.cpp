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

	//필요시 추가
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
//StartSlot: 셰이더 코드에서 샘플러가 바인딩될 레지스터 슬롯 번호입니다.HLSL 셰이더에서 sampler s0, sampler s1 등으로 선언된 경우, 이 sX의 X에 해당하는 번호입니다.
//NumSamplers : 바인딩할 샘플러 스테이트의 개수입니다(보통 1개).
//ppSamplerStates : 바인딩할 ID3D11SamplerState 포인터들의 배열입니다.
*/
void SamplerState::SetVS(ID3D11DeviceContext* pDeviceContext, Samplers sampler, UINT startIdx)
{
	pDeviceContext->VSSetSamplers(startIdx, 1, &m_pSamplers[(UINT)sampler]);
}

void SamplerState::SetPS(ID3D11DeviceContext* pDeviceContext, Samplers sampler, UINT startIdx)
{
	pDeviceContext->PSSetSamplers(startIdx, 1, &m_pSamplers[(UINT)sampler]);
}
