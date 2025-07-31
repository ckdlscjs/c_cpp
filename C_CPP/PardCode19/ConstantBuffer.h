#pragma once
#include "std.h"
class ConstantBuffer
{
public:
	ConstantBuffer(ID3D11Device* pDevice, void* data, UINT size_buffer);
	~ConstantBuffer();
	ConstantBuffer(const ConstantBuffer&) = delete;
	ConstantBuffer& operator=(const ConstantBuffer&) = delete;
	ConstantBuffer(ConstantBuffer&&) = delete;
	ConstantBuffer& operator=(ConstantBuffer&&) = delete;
	void UpdateBufferData(ID3D11DeviceContext* pDeviceContext, void* data);
	void SetVS(ID3D11DeviceContext* pDeviceContext, UINT startIdx = 0);
	void SetPS(ID3D11DeviceContext* pDeviceContext, UINT startIdx = 0);

private:
	ID3D11Buffer* m_pBuffer;
};

