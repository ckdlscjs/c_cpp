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

	//void CreateRenderTargetView(ID3D11Device* pDevice);
	//void CreateDepthStencilView(ID3D11Device* pDevice, UINT width, UINT height);
	//void ResizeBuffers(UINT bufferCount, UINT width, UINT height, DXGI_FORMAT format, UINT flags);
	//void ClearRenderTargetColor(ID3D11DeviceContext* pDeviceContext, float red, float green, float blue, float alpha);
	IDXGISwapChain* GetSwapChain();
	void Present(bool vsync);

private:
	IDXGISwapChain* m_pSwapChain;
	/*���� �ʿ�ÿ� ������ �ʿ�, ���ϴ� ���������ǵ��� �����ؼ������*/
	/*ID3D11RenderTargetView* m_pRenderTargetView;
	ID3D11DepthStencilView* m_pDepthStencilView;*/
};

