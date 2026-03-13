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
		_In_  ID3D11Resource* pDstResource,						//업데이트 대상 버퍼
		_In_  UINT DstSubresource,								//대상 버퍼리소스 내부의 위치를 계산하기위한 인덱스,
		_In_opt_  const D3D11_BOX* pDstBox,						//업데이트할 리소스의 하위리소스 정의하는 구조체(D3D11BOX)에 대한 포인터, NULL시 버퍼전체를 업데이트함
		_In_  const void* pSrcData,								//업데이트할 내용
		_In_  UINT SrcRowPitch,									//업데이트할 데이터의 한 행 크기
		_In_  UINT SrcDepthPitch) = 0);							//업데이트할 데이터의 깊이 1개 크기
	*/
	//m_pBuffer에 이미 사용할 크기와 동일한 크기만큼 data의 위치에 크기만큼 구조체등으로 나열되어있으므로 별도지정을 하지않는다
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
