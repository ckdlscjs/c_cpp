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
	void Init();
	void SetViewportSize(UINT iWidth, UINT iHeight);
	void DrawVertex_TriangleList(UINT vertexCount, UINT startIdx);
	void DrawVertex_TriangleStrip(UINT vertexCount, UINT startIdx);
	void DrawIndex_TriagleList(UINT indexCount, UINT startIdx, INT vertexOffset);
	void DrawIndex_TriangleStrip(UINT indexCount, UINT startIdx, INT vertexOffset);
	void Release();

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();

private:
	ID3D11Device* m_pD3D_Device;
	ID3D11DeviceContext* m_pD3D_DeviceContext;
	//ID3D11DeviceContext* m_pD3D_DefferedContext;
	D3D_FEATURE_LEVEL m_D3D_FeatureLevel;
};

