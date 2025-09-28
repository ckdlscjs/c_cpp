#include "SamplerState.h"

SamplerState::SamplerState(ID3D11Device* pDevice)
{
	std::cout << "Initialize : " << "SamplerState" << " Class" << '\n';

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

SamplerState::~SamplerState()
{
	std::cout << "Release : " << "SamplerState" << " Class" << '\n';
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
void SamplerState::SetVS(ID3D11DeviceContext* pDeviceContext, E_Samplers sampler, UINT startIdx)
{
	pDeviceContext->VSSetSamplers(startIdx, 1, &m_pStates[(UINT)sampler]);
}

void SamplerState::SetPS(ID3D11DeviceContext* pDeviceContext, E_Samplers sampler, UINT startIdx)
{
	pDeviceContext->PSSetSamplers(startIdx, 1, &m_pStates[(UINT)sampler]);
}
