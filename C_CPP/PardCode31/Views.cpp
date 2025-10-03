#include "Views.h"
////////////////////////////////////////////////////////////////////////////////
// ShaderResourceView
////////////////////////////////////////////////////////////////////////////////

//�ؽ��� ����� SRV����
ShaderResourceView::ShaderResourceView(ID3D11Device* pDevice, const ScratchImage* pResource)
{
	HRESULT hResult;
	ID3D11Resource* pBuffer;
	hResult = CreateTexture(pDevice, pResource->GetImages(), pResource->GetImageCount(), pResource->GetMetadata(), &pBuffer); //RC1
	_ASEERTION_CREATE(hResult, "Texture not create successfully");
	// Resource�� ����ϱ����� ResourceView�� �����Ѵ�
	D3D11_SHADER_RESOURCE_VIEW_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	desc.Format = pResource->GetMetadata().format;
	desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	desc.Texture2D.MipLevels = (UINT)pResource->GetMetadata().mipLevels;
	desc.Texture2D.MostDetailedMip = 0;

	hResult = pDevice->CreateShaderResourceView(pBuffer, &desc, &m_pView); //RC2
	_ASEERTION_CREATE(hResult, "SRV not create successfully");
	pBuffer->Release();	//CreateTexture�� ����ī���� ����
}

//���� ����� SRV����
ShaderResourceView::ShaderResourceView(ID3D11Device* pDevice, ID3D11Texture2D* pBuffer, DXGI_FORMAT format)
{
	Resize(pDevice, pBuffer, format);
}

void ShaderResourceView::Resize(ID3D11Device* pDevice, ID3D11Texture2D* pBuffer, DXGI_FORMAT format)
{
	HRESULT hResult;
	// ���̴� ���ҽ� ���� ������ �����մϴ�.
	D3D11_SHADER_RESOURCE_VIEW_DESC desc_srv;
	desc_srv.Format = format;
	desc_srv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	desc_srv.Texture2D.MostDetailedMip = 0;
	desc_srv.Texture2D.MipLevels = 1;

	// ���̴� ���ҽ� �並 ����ϴ�.
	hResult = pDevice->CreateShaderResourceView(pBuffer, &desc_srv, &m_pView);
	_ASEERTION_CREATE(hResult, "SRV");
}


////////////////////////////////////////////////////////////////////////////////
// RenderTargetView
////////////////////////////////////////////////////////////////////////////////

RenderTargetView::RenderTargetView(ID3D11Device* pDevice, IDXGISwapChain* pSwapChain)
{
	Resize(pDevice, pSwapChain);
}

RenderTargetView::RenderTargetView(ID3D11Device* pDevice, ID3D11Texture2D* pBuffer)
{
	Resize(pDevice, pBuffer, DXGI_FORMAT_R32G32B32A32_FLOAT);
}

//����۱��
void RenderTargetView::Resize(ID3D11Device* pDevice, IDXGISwapChain* pSwapChain)
{
	HRESULT hResult;
	ID3D11Texture2D* pBuffer;
	hResult = pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBuffer);
	_ASEERTION_CREATE(hResult, "BackBuffer");

	//�Ѱܹ��� ����۸� ������� RendrTargetView�� ����
	hResult = pDevice->CreateRenderTargetView(pBuffer, NULL, &m_pView);
	_ASEERTION_CREATE(hResult, "RTV");

	pBuffer->Release();
}

//���۱��
void RenderTargetView::Resize(ID3D11Device* pDevice, ID3D11Texture2D* pBuffer, DXGI_FORMAT format)
{
	HRESULT hResult;

	// ���� Ÿ�� ���� ������ �����մϴ�.
	D3D11_RENDER_TARGET_VIEW_DESC desc_rtv;
	desc_rtv.Format = format;
	desc_rtv.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	desc_rtv.Texture2D.MipSlice = 0;

	// ���� Ÿ�� �並 �����Ѵ�.
	hResult = pDevice->CreateRenderTargetView(pBuffer, &desc_rtv, &m_pView);
	_ASEERTION_CREATE(hResult, "RTV");
}

////////////////////////////////////////////////////////////////////////////////
// DepthStencilView
////////////////////////////////////////////////////////////////////////////////

DepthStencilView::DepthStencilView(ID3D11Device* pDevice, ID3D11Texture2D* pBuffer)
{
	Resize(pDevice, pBuffer, DXGI_FORMAT_D24_UNORM_S8_UINT);
}

void DepthStencilView::Resize(ID3D11Device* pDevice, ID3D11Texture2D* pBuffer, DXGI_FORMAT format)
{
	HRESULT hResult;

	D3D11_DEPTH_STENCIL_VIEW_DESC desc_dsv;
	ZeroMemory(&desc_dsv, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	desc_dsv.Format = format;													//���̸��� D24_UNROM_S8_UINT������
	desc_dsv.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	desc_dsv.Texture2D.MipSlice = 0;
	hResult = pDevice->CreateDepthStencilView(pBuffer, &desc_dsv, &m_pView);	//�ش���۸� �̿��Ͽ� ���̽��ٽ� �並 ����	
	_ASEERTION_CREATE(hResult, "DSV");
}


