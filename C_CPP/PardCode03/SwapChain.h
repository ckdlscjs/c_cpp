#pragma once
#include "std.h"
class SwapChain
{
public:
	SwapChain();
	~SwapChain();

	bool Init(IDXGIFactory* pFactory, ID3D11Device* pDevice, HWND hwnd, UINT iWidth, UINT iHeight);
	bool Release();

private:
	IDXGISwapChain* m_pSwapChain;
};

