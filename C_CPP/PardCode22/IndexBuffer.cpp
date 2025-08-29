#include "IndexBuffer.h"

IndexBuffer::IndexBuffer(ID3D11Device* pDevice, void* indices, UINT size_indices)
{
	std::cout << "Initialize : " << "IndexBuffer" << " Class" << '\n';
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
	D3D11_BUFFER_DESC buff_desc = {};		//버퍼 디스크립션
	ZeroMemory(&buff_desc, sizeof(buff_desc));
	buff_desc.Usage = D3D11_USAGE_DEFAULT;
	buff_desc.ByteWidth = sizeof(UINT) * size_indices;		//사용할 객체의 전체크기
	buff_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;			//인덱스버퍼에 사용되므로 해당플래그를 지정한다
	buff_desc.CPUAccessFlags = 0;
	buff_desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA init_data = {};
	ZeroMemory(&init_data, sizeof(init_data));
	init_data.pSysMem = indices;

	m_iSize_Indices = size_indices;

	HRESULT hResult = pDevice->CreateBuffer(&buff_desc, &init_data, &m_pBuffer);
	_ASEERTION_CREATE(hResult, "IndexBuffer");
}

IndexBuffer::~IndexBuffer()
{
	std::cout << "Release : " << "IndexBuffer" << " Class" << '\n';
	m_pBuffer->Release();
}

void IndexBuffer::SetIndexBuffer(ID3D11DeviceContext* pDeviceContext)
{
	UINT offset = 0;				//오프셋
	pDeviceContext->IASetIndexBuffer(m_pBuffer, DXGI_FORMAT_R32_UINT, offset); //4바이트, 32비트를의미
}

UINT IndexBuffer::GetCountIndices()
{
	return m_iSize_Indices;
}
