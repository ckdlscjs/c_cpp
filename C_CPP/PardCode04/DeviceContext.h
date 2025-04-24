#pragma once
#include "std.h"

class SwapChain;
class DeviceContext
{
public:
	DeviceContext();
	~DeviceContext();
	bool Init(ID3D11DeviceContext* pDeviceContext);
	bool ClearRenderTargetColor(SwapChain* pSwapChain, float red, float green, float blue, float alpha);
	bool Release();
private:
	ID3D11DeviceContext* m_pD3D_DeviceContext;
};

