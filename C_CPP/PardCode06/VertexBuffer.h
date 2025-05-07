#pragma once
#include "std.h"

class VertexBuffer
{
public:
	VertexBuffer();
	~VertexBuffer();
	bool Init(ID3D11Device* pDevice, void* vertices, UINT size_vertex, UINT size_vertices);
	bool SetVertexBuffer(ID3D11DeviceContext* pDeviceContext);
	bool Release();
	UINT GetCountVertices();
private:
	UINT m_iSize_vertex;
	UINT m_iSize_vertices;
private:
	ID3D11Buffer* m_pBuffer;
};

