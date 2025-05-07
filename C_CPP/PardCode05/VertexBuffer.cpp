#include "VertexBuffer.h"

VertexBuffer::VertexBuffer()
{
	std::cout << "VertexBuffer" << " Class" << " 생성자 호출" << '\n';
}

VertexBuffer::~VertexBuffer()
{
	std::cout << "VertexBuffer" << " Class" << " 소멸자 호출" << '\n';
	Release();
}

bool VertexBuffer::Init(ID3D11Device* pDevice, void* vertices, UINT size_vertex, UINT size_vertices)
{
	/*
	typedef struct D3D11_BUFFER_DESC
	{
	UINT ByteWidth;						//생성할 정점 버퍼의 크기(바이트단위)
	D3D11_USAGE Usage;					//버퍼가 쓰일 방식, D3D11_USAGE열거형의 원소중 하나를 선택하여 사용한다
	UINT BindFlags;						//어떠한 버퍼로 쓰일지 선택한다(정점버퍼의경우 D3D11_BIND_VERTEX_BUFFER)
	UINT CPUAccessFlags;				//CPU가 버퍼에 접근하는 방식을 결정하는 플래그들을 지정한다, 버퍼의용도에따라 지정
	UINT MiscFlags;						//기타플래그
	UINT StructureByteStride;			//구조적버퍼에 저장된 원소하나의크기(바이트단위), 구조적버퍼외에는 0을 지정한다
	} 	D3D11_BUFFER_DESC;

	typedef struct D3D11_SUBRESOURCE_DATA
	{
	const void *pSysMem;				//정점버퍼를 초기화할 자료를 담은 시스템 메모리 배열을 가리키는 포인터
	UINT SysMemPitch;					//정점버퍼에는 쓰이지않는다, 텍스쳐리소스에사용된다
	UINT SysMemSlicePitch;				//정점버퍼에는 쓰이지 않는다, 텍스쳐리소스에사용된다
	} 	D3D11_SUBRESOURCE_DATA;
	*/
	D3D11_BUFFER_DESC buff_desc = {};	//버텍스버퍼 디스크립션
	buff_desc.Usage = D3D11_USAGE_DEFAULT;
	buff_desc.ByteWidth = size_vertex * size_vertices;
	buff_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	buff_desc.CPUAccessFlags = 0;
	buff_desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA init_data = {};
	init_data.pSysMem = vertices;

	m_iSize_vertex = size_vertex;
	m_iSize_vertices = size_vertices;
	HRESULT hResult = pDevice->CreateBuffer(&buff_desc, &init_data, &m_pBuffer);
	if (FAILED(hResult))
		return false;
	return true;
}

bool VertexBuffer::SetVertexBuffer(ID3D11DeviceContext* pDeviceContext)
{
	UINT stride = m_iSize_vertex;
	UINT offset = 0;
	pDeviceContext->IASetVertexBuffers(0, 1, &m_pBuffer, &stride, &offset);
	return true;
}

bool VertexBuffer::Release()
{
	m_pBuffer->Release();
	return true;
}

UINT VertexBuffer::GetCountVertices()
{
	return m_iSize_vertices;
}
