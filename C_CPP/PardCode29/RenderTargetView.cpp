#include "RenderTargetView.h"

RenderTargetView::RenderTargetView(ID3D11Device* pDevice, IDXGISwapChain* pSwapChain)
{
	HRESULT hResult;
	ID3D11Texture2D* pBuffer;
	hResult = pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBuffer);
	_ASEERTION_CREATE(hResult, "BackBuffer");

	hResult = pDevice->CreateRenderTargetView(pBuffer, NULL, &m_pRTV);
	_ASEERTION_CREATE(hResult, "RTV");

	pBuffer->Release();
}

RenderTargetView::~RenderTargetView()
{
}

ID3D11RenderTargetView* RenderTargetView::GetRTV()
{
	return m_pRTV;
}
