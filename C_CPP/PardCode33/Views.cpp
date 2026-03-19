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

