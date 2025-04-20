#pragma once
#include "std.h"

//SwapChain클래스 전방선언
class SwapChain;
class Window;

class GraphicEngine
{
public:
	GraphicEngine();
	~GraphicEngine();
	
	bool Init(Window* pWindow);
	bool Release();

	static GraphicEngine& Get(); //Singleton함수, 컴파일타임에 정적변수로 생성된다
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

