#pragma once
#include "BaseView.h"

class RenderTargetView : public BaseView
{
public:
	RenderTargetView(ID3D11Device* pDevice, IDXGISwapChain* pSwapChain);
	virtual ~RenderTargetView();
	RenderTargetView(const RenderTargetView&) = delete;
	RenderTargetView& operator=(const RenderTargetView&) = delete;
	RenderTargetView(RenderTargetView&&) = delete;
	RenderTargetView& operator=(RenderTargetView&&) = delete;

	ID3D11RenderTargetView* GetRTV();

private:
	ID3D11RenderTargetView* m_pRTV;
};
