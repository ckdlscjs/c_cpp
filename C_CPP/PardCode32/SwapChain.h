#pragma once
#include "CommonHeader.h"

class SwapChain
{
public:
	SwapChain(ID3D11Device* pDevice, HWND hwnd, UINT iWidth, UINT iHeight);
	~SwapChain();
	SwapChain(const SwapChain&) = delete;
	SwapChain& operator=(const SwapChain&) = delete;
	SwapChain(SwapChain&&) = delete;
	SwapChain& operator=(SwapChain&&) = delete;
	IDXGISwapChain* GetSwapChain();

private:
	IDXGISwapChain* m_pSwapChain;
};

