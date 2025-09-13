#pragma once
#include "CommonHeader.h"

class IndexBuffer
{
public:
	IndexBuffer(ID3D11Device* pDevice, void* indices, UINT size_indices);
	~IndexBuffer();
	IndexBuffer(const IndexBuffer&) = delete;
	IndexBuffer& operator=(const IndexBuffer&) = delete;
	IndexBuffer(IndexBuffer&&) = delete;
	IndexBuffer& operator=(IndexBuffer&&) = delete;
	void SetIndexBuffer(ID3D11DeviceContext* pDeviceContext);
	UINT GetCountIndices();
private:
	UINT m_iSize_Indices;
private:
	ID3D11Buffer* m_pBuffer;
};

