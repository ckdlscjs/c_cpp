#pragma once
#include "std.h"
class SwapChain
{
public:
	SwapChain(ID3D11Device* pDevice, HWND hwnd, UINT iWidth, UINT iHeight);
	~SwapChain();
	SwapChain(const SwapChain&) = delete;
	SwapChain& operator=(const SwapChain&) = delete;
	SwapChain(SwapChain&&) = delete;
	SwapChain& operator=(SwapChain&&) = delete;
	void ClearRenderTargetColor(ID3D11DeviceContext* pDeviceContext, float red, float green, float blue, float alpha);
	void Present(bool vsync);

private:
	IDXGISwapChain* m_pSwapChain;
	ID3D11RenderTargetView* m_pRTV;
	
	ID3D11DepthStencilView* m_pDSV;
};

