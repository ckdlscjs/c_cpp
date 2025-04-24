#pragma once
#include "std.h"

//SwapChain클래스 전방선언
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

	static GraphicEngine& Get(); //Singleton함수, 컴파일타임에 정적변수로 생성된다
private:
	ID3D11Device* m_pD3D_Device;
	//ID3D11DeviceContext* m_pD3D_DeviceContext;
	D3D_FEATURE_LEVEL m_D3D_FeatureLevel;
	
private:
	IDXGIDevice* m_pDXGI_Device;
	IDXGIAdapter* m_pDXGI_Adapter;
	IDXGIFactory* m_pDXGI_Factory;

	//사용을위해 분할한 클래스객체들
public:
	class SwapChain* m_pCSwapChain;
	class DeviceContext* m_pCDeviceContext;
};