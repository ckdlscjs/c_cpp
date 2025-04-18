#pragma once
//d3d11 라이브러리를 사용하기위한 설정
#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")	

class GraphicEngine
{
public:
	GraphicEngine();
	~GraphicEngine();
	
	bool Init();
	bool Release();

	static GraphicEngine& Get(); //Singleton함수, 컴파일타임에 정적변수로 생성된다
private:
	ID3D11Device* m_D3DDevice;
	ID3D11DeviceContext* m_D3DDeviceContext;
	D3D_FEATURE_LEVEL m_FeatureLevel;
};

