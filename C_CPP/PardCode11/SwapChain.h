#pragma once
#include "std.h"
class SwapChain
{
public:
	SwapChain();
	~SwapChain();

	void Init(ID3D11Device* pDevice, HWND hwnd, UINT iWidth, UINT iHeight);
	void ClearRenderTargetColor(ID3D11DeviceContext* pDeviceContext, float red, float green, float blue, float alpha);
	void Present(bool vsync);
	void Release();

private:
	IDXGISwapChain* m_pSwapChain;
	ID3D11RenderTargetView* m_pRTV;
};

