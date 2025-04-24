#pragma once
#include "std.h"
class SwapChain
{
public:
	SwapChain();
	~SwapChain();

	ID3D11RenderTargetView* GetRenderTargetView() const;
	bool Init(IDXGIFactory* pFactory, ID3D11Device* pDevice, HWND hwnd, UINT iWidth, UINT iHeight);
	bool Present(bool vsync);
	bool Release();

private:
	IDXGISwapChain* m_pSwapChain;
	ID3D11RenderTargetView* m_pRTV;
};

