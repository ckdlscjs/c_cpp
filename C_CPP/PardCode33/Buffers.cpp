#include "Buffers.h"

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

////////////////////////////////////////////////////////////////////////////////
// ConstantBuffer
////////////////////////////////////////////////////////////////////////////////
ConstantBuffer::ConstantBuffer(ID3D11Device* pDevice, void* data, UINT count)
{
	std::cout << "Initialize : " << "ConstantBuffer" << " Class" << '\n';
	_ASEERTION_NULCHK(!m_pBuffer, "ConstantBuffer NOTNULL");
	m_iStride = count;
	m_iSize = 1;
	D3D11_BUFFER_DESC buff_desc;
	ZeroMemory(&buff_desc, sizeof(buff_desc));
	buff_desc.Usage = D3D11_USAGE_DEFAULT;
	buff_desc.ByteWidth = m_iStride * m_iSize;
	buff_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;		
	buff_desc.CPUAccessFlags = 0;
	buff_desc.MiscFlags = 0;
	
	D3D11_SUBRESOURCE_DATA init_data = {};
	ZeroMemory(&init_data, sizeof(init_data));
	init_data.pSysMem = data;

	HRESULT hResult = pDevice->CreateBuffer(&buff_desc, data ? &init_data : nullptr, &m_pBuffer);
	_ASEERTION_CREATE(hResult, "ConstantBuffer");
}

////////////////////////////////////////////////////////////////////////////////
// VertexBuffer
////////////////////////////////////////////////////////////////////////////////
VertexBuffer::VertexBuffer(ID3D11Device* pDevice, void* data, UINT stride, UINT count)
{
	std::cout << "Initialize : " << "VertexBuffer" << " Class" << '\n';
	m_iStride = stride;
	m_iSize = count;
	D3D11_BUFFER_DESC buff_desc = {};						//버텍스버퍼 디스크립션
	ZeroMemory(&buff_desc, sizeof(buff_desc));
	buff_desc.Usage = D3D11_USAGE_DEFAULT;
	buff_desc.ByteWidth = m_iStride * m_iSize;				//사용할 객체의 전체크기
	buff_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;			//정점버퍼에 사용되므로 해당플래그를 지정한다
	buff_desc.CPUAccessFlags = 0;
	buff_desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA init_data = {};
	ZeroMemory(&init_data, sizeof(init_data));
	init_data.pSysMem = data;

	HRESULT hResult = pDevice->CreateBuffer(&buff_desc, data ? &init_data : nullptr, &m_pBuffer);
	_ASEERTION_CREATE(hResult, "VertexBuffer");
}

UINT VertexBuffer::GetCountVertices()
{
	return GetSize();
}

UINT VertexBuffer::GetVertexSize()
{
	return GetStride();
}

////////////////////////////////////////////////////////////////////////////////
// IndexBuffer
////////////////////////////////////////////////////////////////////////////////
IndexBuffer::IndexBuffer(ID3D11Device* pDevice, void* data, UINT count)
{
	std::cout << "Initialize : " << "IndexBuffer" << " Class" << '\n';
	m_iStride = sizeof(UINT);
	m_iSize = count;
	D3D11_BUFFER_DESC buff_desc = {};						//버퍼 디스크립션
	ZeroMemory(&buff_desc, sizeof(buff_desc));
	buff_desc.Usage = D3D11_USAGE_DEFAULT;
	buff_desc.ByteWidth = m_iStride * m_iSize;		//사용할 객체의 전체크기
	buff_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;			//인덱스버퍼에 사용되므로 해당플래그를 지정한다
	buff_desc.CPUAccessFlags = 0;
	buff_desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA init_data = {};
	ZeroMemory(&init_data, sizeof(init_data));
	init_data.pSysMem = data;

	HRESULT hResult = pDevice->CreateBuffer(&buff_desc, data ? &init_data : nullptr, &m_pBuffer);
	_ASEERTION_CREATE(hResult, "IndexBuffer");
}

UINT IndexBuffer::GetCountIndices()
{
	return GetSize();
}

////////////////////////////////////////////////////////////////////////////////
// StructBuffer
////////////////////////////////////////////////////////////////////////////////
StructBuffer::StructBuffer(ID3D11Device* pDevice, void* data, UINT stride, UINT count)
{
	std::cout << "Initialize : " << "StructBuffer" << " Class" << '\n';
	m_iStride = stride;
	m_iSize = count;
	D3D11_BUFFER_DESC buff_desc = {};														//버퍼 디스크립션
	ZeroMemory(&buff_desc, sizeof(buff_desc));
	buff_desc.Usage = D3D11_USAGE_DEFAULT;
	buff_desc.ByteWidth = m_iStride * m_iSize;												//사용할 객체의 전체크기
	buff_desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;			//UAV, SRV연산용
	buff_desc.CPUAccessFlags = 0;															//updateSubResource로 사용하되 추후 read시 별도의 staing버퍼를만든다
	buff_desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	buff_desc.StructureByteStride = m_iStride;

	D3D11_SUBRESOURCE_DATA init_data = {};
	ZeroMemory(&init_data, sizeof(init_data));
	init_data.pSysMem = data;

	HRESULT hResult = pDevice->CreateBuffer(&buff_desc, data ? &init_data : nullptr, &m_pBuffer);
	_ASEERTION_CREATE(hResult, "StructBuffer");
}


////////////////////////////////////////////////////////////////////////////////
// StagingBuffer
////////////////////////////////////////////////////////////////////////////////
StagingBuffer::StagingBuffer(ID3D11Device* pDevice, UINT stride, UINT count)
{
	std::cout << "Initialize : " << "StagingBuffer" << " Class" << '\n';
	m_iStride = stride;
	m_iSize = count;
	D3D11_BUFFER_DESC buff_desc = {};														//버퍼 디스크립션
	ZeroMemory(&buff_desc, sizeof(buff_desc));
	buff_desc.Usage = D3D11_USAGE_STAGING;
	buff_desc.ByteWidth = m_iStride * m_iSize;												//사용할 객체의 전체크기
	buff_desc.BindFlags = 0;																//StaingBuffer는 셰이더자원으로 사용하지않는다, 오직read
	buff_desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;							//StructBuffer를 복사하기위해 같은 플래그를 지정한다
	buff_desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;										//read용으로 사용한다
	buff_desc.StructureByteStride = stride;

	HRESULT hResult = pDevice->CreateBuffer(&buff_desc,  nullptr, &m_pBuffer);				//staging버퍼는 복사용으로 이후 CopyResource로 데이터를 채운다
	_ASEERTION_CREATE(hResult, "StagingBuffer");
}
