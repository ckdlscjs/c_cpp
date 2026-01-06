#include "Views.h"
////////////////////////////////////////////////////////////////////////////////
// ShaderResourceView
////////////////////////////////////////////////////////////////////////////////

//텍스쳐 기반의 SRV생성
ShaderResourceView::ShaderResourceView(ID3D11Device* pDevice, const ScratchImage* pResource)
{
	HRESULT hResult;
	ID3D11Resource* pBuffer;
	hResult = CreateTexture(pDevice, pResource->GetImages(), pResource->GetImageCount(), pResource->GetMetadata(), &pBuffer); //RC1
	_ASEERTION_CREATE(hResult, "Texture not create successfully");
	// Resource를 사용하기위한 ResourceView를 생성한다
	D3D11_SHADER_RESOURCE_VIEW_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	desc.Format = pResource->GetMetadata().format;
	desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	desc.Texture2D.MipLevels = (UINT)pResource->GetMetadata().mipLevels;
	desc.Texture2D.MostDetailedMip = 0;

	hResult = pDevice->CreateShaderResourceView(pBuffer, &desc, &m_pView); //RC2
	_ASEERTION_CREATE(hResult, "SRV not create successfully");
	pBuffer->Release();	//CreateTexture의 참조카운팅 감산
}

//버퍼 기반의 SRV생성
ShaderResourceView::ShaderResourceView(ID3D11Device* pDevice, ID3D11Texture2D* pBuffer, DXGI_FORMAT format)
{
	Resize(pDevice, pBuffer, format);
}

void ShaderResourceView::Resize(ID3D11Device* pDevice, ID3D11Texture2D* pBuffer, DXGI_FORMAT format)
{
	HRESULT hResult;
	// 셰이더 리소스 뷰의 설명을 설정합니다.
	D3D11_SHADER_RESOURCE_VIEW_DESC desc_srv;
	desc_srv.Format = format;
	desc_srv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	desc_srv.Texture2D.MostDetailedMip = 0;
	desc_srv.Texture2D.MipLevels = 1;

	// 셰이더 리소스 뷰를 만듭니다.
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

RenderTargetView::RenderTargetView(ID3D11Device* pDevice, ID3D11Texture2D* pBuffer, DXGI_FORMAT format)
{
	Resize(pDevice, pBuffer, format);
}

//백버퍼기반
void RenderTargetView::Resize(ID3D11Device* pDevice, IDXGISwapChain* pSwapChain)
{
	HRESULT hResult;
	ID3D11Texture2D* pBuffer;
	hResult = pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBuffer);
	_ASEERTION_CREATE(hResult, "BackBuffer");

	//넘겨받은 백버퍼를 기반으로 RendrTargetView를 생성
	hResult = pDevice->CreateRenderTargetView(pBuffer, NULL, &m_pView);
	_ASEERTION_CREATE(hResult, "RTV");

	pBuffer->Release();
}

//버퍼기반
void RenderTargetView::Resize(ID3D11Device* pDevice, ID3D11Texture2D* pBuffer, DXGI_FORMAT format)
{
	HRESULT hResult;

	// 렌더 타겟 뷰의 설명을 설정합니다.
	D3D11_RENDER_TARGET_VIEW_DESC desc_rtv;
	desc_rtv.Format = format;
	desc_rtv.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	desc_rtv.Texture2D.MipSlice = 0;

	// 렌더 타겟 뷰를 생성한다.
	hResult = pDevice->CreateRenderTargetView(pBuffer, &desc_rtv, &m_pView);
	_ASEERTION_CREATE(hResult, "RTV");
}

////////////////////////////////////////////////////////////////////////////////
// DepthStencilView
////////////////////////////////////////////////////////////////////////////////

DepthStencilView::DepthStencilView(ID3D11Device* pDevice, ID3D11Texture2D* pBuffer, DXGI_FORMAT format)
{
	Resize(pDevice, pBuffer, format);
}

void DepthStencilView::Resize(ID3D11Device* pDevice, ID3D11Texture2D* pBuffer, DXGI_FORMAT format)
{
	HRESULT hResult;

	D3D11_DEPTH_STENCIL_VIEW_DESC desc_dsv;
	ZeroMemory(&desc_dsv, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	desc_dsv.Format = format;													//깊이맵은 D24_UNROM_S8_UINT가정석
	desc_dsv.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	desc_dsv.Texture2D.MipSlice = 0;
	hResult = pDevice->CreateDepthStencilView(pBuffer, &desc_dsv, &m_pView);	//해당버퍼를 이용하여 깊이스텐실 뷰를 생성	
	_ASEERTION_CREATE(hResult, "DSV");
}


