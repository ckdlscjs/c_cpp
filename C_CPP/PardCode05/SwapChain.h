#pragma once
#include "std.h"
class SwapChain
{
public:
	SwapChain();
	~SwapChain();

	ID3D11RenderTargetView* GetRenderTargetView() const;
	bool Init(ID3D11Device* pDevice, HWND hwnd, UINT iWidth, UINT iHeight);
	bool ClearRenderTargetColor(ID3D11DeviceContext* pDeviceContext, float red, float green, float blue, float alpha);
	bool Present(bool vsync);
	bool Release();

private:
	IDXGISwapChain* m_pSwapChain;
	ID3D11RenderTargetView* m_pRTV;
};

