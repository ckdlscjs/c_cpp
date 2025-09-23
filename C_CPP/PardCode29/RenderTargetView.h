#pragma once
#include "ShaderResourceView.h"

class RenderTargetView : public ShaderResourceView
{
public:
	RenderTargetView(ID3D11Device* pDevice, IDXGISwapChain* pSwapChain);
	RenderTargetView(ID3D11Device* pDevice, UINT width, UINT height);
	virtual ~RenderTargetView();
	RenderTargetView(const RenderTargetView&) = delete;
	RenderTargetView& operator=(const RenderTargetView&) = delete;
	RenderTargetView(RenderTargetView&&) = delete;
	RenderTargetView& operator=(RenderTargetView&&) = delete;
	ID3D11RenderTargetView* GetRTV();
	void Resize(ID3D11Device* pDevice, IDXGISwapChain* pSwapChain);
	void Resize(ID3D11Device* pDevice, UINT width, UINT height);
private:
	ID3D11RenderTargetView* m_pRTV;
};
