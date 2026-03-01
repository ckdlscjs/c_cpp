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
ShaderResourceView::ShaderResourceView(ID3D11Device* pDevice, ID3D11Texture2D* pBuffer, D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc)
{
	Resize(pDevice, pBuffer, srvDesc);
}

void ShaderResourceView::Resize(ID3D11Device* pDevice, ID3D11Texture2D* pBuffer, D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc)
{
	HRESULT hResult;
	// 렌더 타겟 뷰를 생성한다.
	hResult = pDevice->CreateShaderResourceView(pBuffer, &srvDesc, &m_pView);
	_ASEERTION_CREATE(hResult, "SRV");
}


////////////////////////////////////////////////////////////////////////////////
// RenderTargetView
////////////////////////////////////////////////////////////////////////////////

RenderTargetView::RenderTargetView(ID3D11Device* pDevice, IDXGISwapChain* pSwapChain)
{
	Resize(pDevice, pSwapChain);
}

RenderTargetView::RenderTargetView(ID3D11Device* pDevice, ID3D11Texture2D* pBuffer, D3D11_RENDER_TARGET_VIEW_DESC rtvDesc)
{
	Resize(pDevice, pBuffer, rtvDesc);
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
void RenderTargetView::Resize(ID3D11Device* pDevice, ID3D11Texture2D* pBuffer, D3D11_RENDER_TARGET_VIEW_DESC rtvDesc)
{
	HRESULT hResult;
	// 렌더 타겟 뷰를 생성한다.
	hResult = pDevice->CreateRenderTargetView(pBuffer, &rtvDesc, &m_pView);
	_ASEERTION_CREATE(hResult, "RTV");
}

////////////////////////////////////////////////////////////////////////////////
// DepthStencilView
////////////////////////////////////////////////////////////////////////////////

DepthStencilView::DepthStencilView(ID3D11Device* pDevice, ID3D11Texture2D* pBuffer, D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc)
{
	Resize(pDevice, pBuffer, dsvDesc);
}

void DepthStencilView::Resize(ID3D11Device* pDevice, ID3D11Texture2D* pBuffer, D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc)
{
	HRESULT hResult;
	hResult = pDevice->CreateDepthStencilView(pBuffer, &dsvDesc, &m_pView);	//해당버퍼를 이용하여 깊이스텐실 뷰를 생성	
	_ASEERTION_CREATE(hResult, "DSV");
}
