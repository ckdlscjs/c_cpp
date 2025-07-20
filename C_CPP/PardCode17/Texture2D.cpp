#include "Texture2D.h"

Texture2D::Texture2D(ID3D11Device* pDevice, const ScratchImage* resource)
{
	std::cout << "Initialize : " << "Texture2D" << " Class" << '\n';
	HRESULT result;

	result = CreateTexture(pDevice, resource->GetImages(), resource->GetImageCount(), resource->GetMetadata(), &m_pTexture);
	_ASEERTION_CREATE(result, "Texture not create successfully");
	// Resource를 사용하기위한 ResourceView를 생성한다
	D3D11_SHADER_RESOURCE_VIEW_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	desc.Format = resource->GetMetadata().format;
	desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	desc.Texture2D.MipLevels = (UINT)resource->GetMetadata().mipLevels;
	desc.Texture2D.MostDetailedMip = 0;

	result = pDevice->CreateShaderResourceView(m_pTexture, &desc, &m_pSRV);
	_ASEERTION_CREATE(result, "SRV not create successfully");

	/*
	D3D11_SAMPLER_DESC sampler_desc;
	ZeroMemory(&sampler_desc, sizeof(D3D11_SAMPLER_DESC));
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.Filter = D3D11_FILTER_ANISOTROPIC; //D3D11_FILTER_ANISOTROPIC 축소, 배율 및 밉 수준 샘플링에 이방성 보간을 사용합니다.
	sampler_desc.MinLOD = 0;
	sampler_desc.MaxLOD = (UINT)imageData.GetMetadata().mipLevels;

	// 샘플러스테이트 생성
	result = pDevice->CreateSamplerState(&sampler_desc, &m_pSamplerState);
	if (FAILED(result))
		throw std::exception("SamplerState not create successfully");
	*/
}

Texture2D::~Texture2D()
{
	std::cout << "Release : " << "Texture2D" << " Class" << '\n';
	m_pTexture->Release();
	m_pSRV->Release();
}

//정점에서의 텍스쳐사용시
void Texture2D::SetVS(ID3D11DeviceContext* pDeviceContext, UINT startIdx)
{
	pDeviceContext->VSSetShaderResources(startIdx, 1, &m_pSRV);
}

//일반적인 픽셀세이더에서의 사용
void Texture2D::SetPS(ID3D11DeviceContext* pDeviceContext, UINT startIdx)
{
	pDeviceContext->PSSetShaderResources(startIdx, 1, &m_pSRV);
}
