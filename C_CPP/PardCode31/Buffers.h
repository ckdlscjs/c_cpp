#pragma once
#include "CommonHeader.h"
template<typename T>
class BaseBuffer
{
public:
	BaseBuffer();
	~BaseBuffer();
	BaseBuffer(const BaseBuffer&) = delete;
	BaseBuffer& operator=(const BaseBuffer&) = delete;
	BaseBuffer(BaseBuffer&&) = delete;
	BaseBuffer& operator=(BaseBuffer&&) = delete;
	void UpdateBufferData(ID3D11DeviceContext* pDeviceContext, void* data);
	ID3D11Buffer* GetBuffer();
protected:
	ID3D11Buffer* m_pBuffer;
};

template<typename T>
inline BaseBuffer<T>::BaseBuffer()
{
	std::cout << "Initialize : " << "BaseBuffer <" << typeid(T).name() << "> Class" << '\n';
}

template<typename T>
inline BaseBuffer<T>::~BaseBuffer()
{
	std::cout << "Release : " << "BaseBuffer <" << typeid(T).name() << "> Class" << '\n';
	m_pBuffer->Release();
}

template<typename T>
inline void BaseBuffer<T>::UpdateBufferData(ID3D11DeviceContext* pDeviceContext, void* data)
{
	//std::cout << "ConstantBuffer" << " Class" << " UpdateSubresource" << '\n';
	/*
	 virtual void STDMETHODCALLTYPE UpdateSubresource(
		_In_  ID3D11Resource* pDstResource,						//������Ʈ ��� ����
		_In_  UINT DstSubresource,								//��� ���۸��ҽ� ������ ��ġ�� ����ϱ����� �ε���,
		_In_opt_  const D3D11_BOX* pDstBox,						//������Ʈ�� ���ҽ��� �������ҽ� �����ϴ� ����ü(D3D11BOX)�� ���� ������, NULL�� ������ü�� ������Ʈ��
		_In_  const void* pSrcData,								//������Ʈ�� ����
		_In_  UINT SrcRowPitch,									//������Ʈ�� �������� �� �� ũ��
		_In_  UINT SrcDepthPitch) = 0);							//������Ʈ�� �������� ���� 1�� ũ��
	*/
	//m_pBuffer�� �̹� ����� ũ��� ������ ũ�⸸ŭ data�� ��ġ�� ũ�⸸ŭ ����ü������ �����Ǿ������Ƿ� ���������� �����ʴ´�
	pDeviceContext->UpdateSubresource(m_pBuffer, NULL, NULL, data, NULL, NULL);
}

template<typename T>
inline ID3D11Buffer* BaseBuffer<T>::GetBuffer()
{
	return m_pBuffer;
}

class ConstantBuffer : public BaseBuffer<ConstantBuffer>
{
	friend class BaseBuffer<ConstantBuffer>;
public:
	ConstantBuffer(ID3D11Device* pDevice, UINT size_buffer, void* data);
	ConstantBuffer(const ConstantBuffer&) = delete;
	ConstantBuffer& operator=(const ConstantBuffer&) = delete;
	ConstantBuffer(ConstantBuffer&&) = delete;
	ConstantBuffer& operator=(ConstantBuffer&&) = delete;
};

class VertexBuffer : public BaseBuffer<VertexBuffer>
{
	friend class BaseBuffer<VertexBuffer>;
public:
	VertexBuffer(ID3D11Device* pDevice, void* vertices, UINT size_vertex, UINT size_vertices);
	VertexBuffer(const VertexBuffer&) = delete;
	VertexBuffer& operator=(const VertexBuffer&) = delete;
	VertexBuffer(VertexBuffer&&) = delete;
	VertexBuffer& operator=(VertexBuffer&&) = delete;
	UINT GetCountVertices();
	UINT GetVertexSize();
private:
	UINT m_iSize_vertex;
	UINT m_iSize_vertices;
};

class IndexBuffer : public BaseBuffer<IndexBuffer>
{
	friend class BaseBuffer<IndexBuffer>;
public:
	IndexBuffer(ID3D11Device* pDevice, void* indices, UINT size_indices);
	IndexBuffer(const IndexBuffer&) = delete;
	IndexBuffer& operator=(const IndexBuffer&) = delete;
	IndexBuffer(IndexBuffer&&) = delete;
	IndexBuffer& operator=(IndexBuffer&&) = delete;
	UINT GetCountIndices();
private:
	UINT m_iSize_Indices;
};