#pragma once
#include "CommonHeader.h"
class SwapChain;
class Direct3D
{
public:
	Direct3D();
	~Direct3D();
	Direct3D(const Direct3D&) = delete;
	Direct3D& operator=(const Direct3D&) = delete;
	Direct3D(Direct3D&&) = delete;
	Direct3D& operator=(Direct3D&&) = delete;
	void SetViewportSize(UINT iWidth, UINT iHeight);
	void DrawVertex_TriangleList(UINT vertexCount, UINT startIdx);
	void DrawVertex_TriangleStrip(UINT vertexCount, UINT startIdx);
	void DrawIndex_TriagleList(UINT indexCount, UINT startIdx, INT vertexOffset);
	void DrawIndex_TriangleStrip(UINT indexCount, UINT startIdx, INT vertexOffset);

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();
	//ID3D11DeviceContext* GetDeffContext();
private:
	ID3D11Device* m_pD3D_Device;
	ID3D11DeviceContext* m_pD3D_DeviceContext;
	//ID3D11DeviceContext* m_pD3D_DefferedContext;
	D3D_FEATURE_LEVEL m_D3D_FeatureLevel;
};

