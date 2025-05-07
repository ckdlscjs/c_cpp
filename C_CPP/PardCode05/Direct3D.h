#pragma once
#include "std.h"
/*
//폐기, GraphicEngine쪽으로 이관
*/
class SwapChain;
class Direct3D
{
public:
	Direct3D();
	~Direct3D();
	bool Init();
	bool SetViewportSize(UINT iWidth, UINT iHeight);
	bool DrawVertex_TriangleList(UINT vertexCount, UINT startIdx);
	bool Release();

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();

private:
	ID3D11Device* m_pD3D_Device;
	ID3D11DeviceContext* m_pD3D_DeviceContext;
	//ID3D11DeviceContext* m_pD3D_DefferedContext;
	D3D_FEATURE_LEVEL m_D3D_FeatureLevel;
};

