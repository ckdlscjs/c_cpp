#include "Views.h"

////////////////////////////////////////////////////////////////////////////////
// ShaderResourceView
////////////////////////////////////////////////////////////////////////////////
ShaderResourceView::ShaderResourceView(ID3D11Device* pDevice, ID3D11Resource* pBuffer, D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc)
{
	Resize(pDevice, pBuffer, srvDesc);
}


void ShaderResourceView::Resize(ID3D11Device* pDevice, ID3D11Resource* pBuffer, D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc)
{
	ReleaseView();
	HRESULT hResult;
	hResult = pDevice->CreateShaderResourceView(pBuffer, &srvDesc, &m_pView);
	_ASEERTION_CREATE(hResult, "SRV");
}


////////////////////////////////////////////////////////////////////////////////
// RenderTargetView
////////////////////////////////////////////////////////////////////////////////
RenderTargetView::RenderTargetView(ID3D11Device* pDevice, ID3D11Resource* pBuffer, D3D11_RENDER_TARGET_VIEW_DESC rtvDesc)
{
	Resize(pDevice, pBuffer, rtvDesc);
}

////백버퍼기반
//void RenderTargetView::Resize(ID3D11Device* pDevice, IDXGISwapChain* pSwapChain)
//{
//	HRESULT hResult;
//	ID3D11Texture2D* pBuffer;
//	hResult = pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBuffer);
//	_ASEERTION_CREATE(hResult, "BackBuffer");
//
//	//넘겨받은 백버퍼를 기반으로 RendrTargetView를 생성
//	hResult = pDevice->CreateRenderTargetView(pBuffer, NULL, &m_pView);
//	_ASEERTION_CREATE(hResult, "RTV");
//
//	pBuffer->Release();
//}

void RenderTargetView::Resize(ID3D11Device* pDevice, ID3D11Resource* pBuffer, D3D11_RENDER_TARGET_VIEW_DESC rtvDesc)
{
	ReleaseView();
	//버퍼기반
	HRESULT hResult;
	hResult = pDevice->CreateRenderTargetView(pBuffer, &rtvDesc, &m_pView);
	_ASEERTION_CREATE(hResult, "RTV");
}

////////////////////////////////////////////////////////////////////////////////
// DepthStencilView
////////////////////////////////////////////////////////////////////////////////
DepthStencilView::DepthStencilView(ID3D11Device* pDevice, ID3D11Resource* pBuffer, D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc)
{
	Resize(pDevice, pBuffer, dsvDesc);
}

void DepthStencilView::Resize(ID3D11Device* pDevice, ID3D11Resource* pBuffer, D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc)
{
	ReleaseView();
	HRESULT hResult;
	hResult = pDevice->CreateDepthStencilView(pBuffer, &dsvDesc, &m_pView);	//해당버퍼를 이용하여 깊이스텐실 뷰를 생성	
	_ASEERTION_CREATE(hResult, "DSV");
}

////////////////////////////////////////////////////////////////////////////////
// UnorderedAccessView
////////////////////////////////////////////////////////////////////////////////
UnorderedAccessView::UnorderedAccessView(ID3D11Device* pDevice, ID3D11Buffer* pBuffer, D3D11_UNORDERED_ACCESS_VIEW_DESC dsvDesc)
{
	Resize(pDevice, pBuffer, dsvDesc);
}

void UnorderedAccessView::Resize(ID3D11Device* pDevice, ID3D11Buffer* pBuffer, D3D11_UNORDERED_ACCESS_VIEW_DESC dsvDesc)
{
	ReleaseView();
	HRESULT hResult;
	hResult = pDevice->CreateUnorderedAccessView(pBuffer, &dsvDesc, &m_pView);	//해당버퍼를 이용하여 깊이스텐실 뷰를 생성	
	_ASEERTION_CREATE(hResult, "UAV");
}

