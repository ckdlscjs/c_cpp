#include "VertexBuffer.h"

VertexBuffer::VertexBuffer()
{
	std::cout << "VertexBuffer" << " Class" << " ������ ȣ��" << '\n';
}

VertexBuffer::~VertexBuffer()
{
	std::cout << "VertexBuffer" << " Class" << " �Ҹ��� ȣ��" << '\n';
	Release();
}

bool VertexBuffer::Init(ID3D11Device* pDevice, void* vertices, UINT size_vertex, UINT size_vertices)
{
	/*
	typedef struct D3D11_BUFFER_DESC
	{
	UINT ByteWidth;						//������ ���� ������ ũ��(����Ʈ����)
	D3D11_USAGE Usage;					//���۰� ���� ���, D3D11_USAGE�������� ������ �ϳ��� �����Ͽ� ����Ѵ�
	UINT BindFlags;						//��� ���۷� ������ �����Ѵ�(���������ǰ�� D3D11_BIND_VERTEX_BUFFER)
	UINT CPUAccessFlags;				//CPU�� ���ۿ� �����ϴ� ����� �����ϴ� �÷��׵��� �����Ѵ�, �����ǿ뵵������ ����
	UINT MiscFlags;						//��Ÿ�÷���
	UINT StructureByteStride;			//���������ۿ� ����� �����ϳ���ũ��(����Ʈ����), ���������ۿܿ��� 0�� �����Ѵ�
	} 	D3D11_BUFFER_DESC;

	typedef struct D3D11_SUBRESOURCE_DATA
	{
	const void *pSysMem;				//�������۸� �ʱ�ȭ�� �ڷḦ ���� �ý��� �޸� �迭�� ����Ű�� ������
	UINT SysMemPitch;					//�������ۿ��� �������ʴ´�, �ؽ��ĸ��ҽ������ȴ�
	UINT SysMemSlicePitch;				//�������ۿ��� ������ �ʴ´�, �ؽ��ĸ��ҽ������ȴ�
	} 	D3D11_SUBRESOURCE_DATA;
	*/
	D3D11_BUFFER_DESC buff_desc = {};	//���ؽ����� ��ũ����
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
