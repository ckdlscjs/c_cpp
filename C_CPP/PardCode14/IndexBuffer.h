#pragma once
#include "std.h"

class IndexBuffer
{
public:
	IndexBuffer();
	~IndexBuffer();
	void Init(ID3D11Device* pDevice, void* indices, UINT size_indices);
	void SetIndexBuffer(ID3D11DeviceContext* pDeviceContext);
	void Release();
	UINT GetCountIndices();
private:
	UINT m_iSize_Indices;
private:
	ID3D11Buffer* m_pBuffer;
};

