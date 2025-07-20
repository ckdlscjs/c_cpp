#include "Texture2D.h"

Texture2D::Texture2D(ID3D11Device* pDevice, const ScratchImage* resource)
{
	std::cout << "Initialize : " << "Texture2D" << " Class" << '\n';
	HRESULT result;

	result = CreateTexture(pDevice, resource->GetImages(), resource->GetImageCount(), resource->GetMetadata(), &m_pTexture);
	_ASEERTION_CREATE(result, "Texture not create successfully");
	// Resource�� ����ϱ����� ResourceView�� �����Ѵ�
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
	sampler_desc.Filter = D3D11_FILTER_ANISOTROPIC; //D3D11_FILTER_ANISOTROPIC ���, ���� �� �� ���� ���ø��� �̹漺 ������ ����մϴ�.
	sampler_desc.MinLOD = 0;
	sampler_desc.MaxLOD = (UINT)imageData.GetMetadata().mipLevels;

	// ���÷�������Ʈ ����
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

//���������� �ؽ��Ļ���
void Texture2D::SetVS(ID3D11DeviceContext* pDeviceContext, UINT startIdx)
{
	pDeviceContext->VSSetShaderResources(startIdx, 1, &m_pSRV);
}

//�Ϲ����� �ȼ����̴������� ���
void Texture2D::SetPS(ID3D11DeviceContext* pDeviceContext, UINT startIdx)
{
	pDeviceContext->PSSetShaderResources(startIdx, 1, &m_pSRV);
}
