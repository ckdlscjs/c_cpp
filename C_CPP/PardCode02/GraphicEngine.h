#pragma once
//d3d11 ���̺귯���� ����ϱ����� ����
#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")	

class GraphicEngine
{
public:
	GraphicEngine();
	~GraphicEngine();
	
	bool Init();
	bool Release();

	static GraphicEngine& Get(); //Singleton�Լ�, ������Ÿ�ӿ� ���������� �����ȴ�
private:
	ID3D11Device* m_D3DDevice;
	ID3D11DeviceContext* m_D3DDeviceContext;
	D3D_FEATURE_LEVEL m_FeatureLevel;
};

