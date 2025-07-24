#include "Texture2D.h"

Texture2D::Texture2D(ID3D11Device* pDevice, const ScratchImage* resource, Samplers sampler)
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

	m_Sampler = sampler;
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

Samplers Texture2D::GetSampler()
{
	return m_Sampler;
}
