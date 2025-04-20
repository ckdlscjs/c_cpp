#pragma once
#include "std.h"

//SwapChainŬ���� ���漱��
class SwapChain;
class Window;

class GraphicEngine
{
public:
	GraphicEngine();
	~GraphicEngine();
	
	bool Init(Window* pWindow);
	bool Release();

	static GraphicEngine& Get(); //Singleton�Լ�, ������Ÿ�ӿ� ���������� �����ȴ�
private:
	ID3D11Device* m_D3DDevice;
	ID3D11DeviceContext* m_D3DDeviceContext;
	D3D_FEATURE_LEVEL m_FeatureLevel;
	
private:
	IDXGIDevice* m_dxgi_device;
	IDXGIAdapter* m_dxgi_adapter;
	IDXGIFactory* m_dxgi_factory;

private:
	class SwapChain* m_pCSwapChain;
};

