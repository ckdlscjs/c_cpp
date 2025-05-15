#pragma once
#include "std.h"
class ConstantBuffer
{
public:
	ConstantBuffer();
	~ConstantBuffer();
	void Init(ID3D11Device* pDevice, void* data, UINT size_buffer);
	void UpdateBufferData(ID3D11DeviceContext* pDeviceContext, void* data);
	void SetVS(ID3D11DeviceContext* pDeviceContext, UINT startIdx = 0);
	void SetPS(ID3D11DeviceContext* pDeviceContext, UINT startIdx = 0);
	void Release();

private:
	ID3D11Buffer* m_pBuffer;
};

