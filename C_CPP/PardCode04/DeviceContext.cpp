#include "DeviceContext.h"
#include "SwapChain.h"
DeviceContext::DeviceContext()
{
	std::cout << "DeviceContext" << " Class" << " ������ ȣ��" << '\n';
}

DeviceContext::~DeviceContext()
{
	std::cout << "DeviceContext" << " Class" << "�Ҹ��� ȣ��" << '\n';
	Release();
}

bool DeviceContext::Init(ID3D11DeviceContext* pDeviceContext)
{
	return m_pD3D_DeviceContext = pDeviceContext;
}

bool DeviceContext::ClearRenderTargetColor(SwapChain* pSwapChain, float red, float green, float blue, float alpha)
{
	FLOAT clearColor[] = { red, green, blue, alpha };
	m_pD3D_DeviceContext->ClearRenderTargetView(pSwapChain->GetRenderTargetView(), clearColor);
	return true;
}

bool DeviceContext::Release()
{
	m_pD3D_DeviceContext->Release();
	return true;
}
