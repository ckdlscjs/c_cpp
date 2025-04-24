#pragma once
#include "std.h"

//SwapChainŬ���� ���漱��
class Window;
class SwapChain;
class DeviceContext;

class GraphicEngine
{
public:
	GraphicEngine();
	~GraphicEngine();
	
	bool Init(Window* pWindow);
	bool Release();

	static GraphicEngine& Get(); //Singleton�Լ�, ������Ÿ�ӿ� ���������� �����ȴ�
private:
	ID3D11Device* m_pD3D_Device;
	//ID3D11DeviceContext* m_pD3D_DeviceContext;
	D3D_FEATURE_LEVEL m_D3D_FeatureLevel;
	
private:
	IDXGIDevice* m_pDXGI_Device;
	IDXGIAdapter* m_pDXGI_Adapter;
	IDXGIFactory* m_pDXGI_Factory;

	//��������� ������ Ŭ������ü��
public:
	class SwapChain* m_pCSwapChain;
	class DeviceContext* m_pCDeviceContext;
};