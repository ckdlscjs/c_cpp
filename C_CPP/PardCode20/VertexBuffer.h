#pragma once
#include "std.h"

class VertexBuffer
{
public:
	VertexBuffer(ID3D11Device* pDevice, void* vertices, UINT size_vertex, UINT size_vertices);
	~VertexBuffer();
	VertexBuffer(const VertexBuffer&) = delete;
	VertexBuffer& operator=(const VertexBuffer&) = delete;
	VertexBuffer(VertexBuffer&&) = delete;
	VertexBuffer& operator=(VertexBuffer&&) = delete;
	void SetVertexBuffer(ID3D11DeviceContext* pDeviceContext);
	UINT GetCountVertices();
private:
	UINT m_iSize_vertex;
	UINT m_iSize_vertices;
private:
	ID3D11Buffer* m_pBuffer;
};

