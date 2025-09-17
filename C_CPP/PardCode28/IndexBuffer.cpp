#include "IndexBuffer.h"

IndexBuffer::IndexBuffer(ID3D11Device* pDevice, void* indices, UINT size_indices)
{
	std::cout << "Initialize : " << "IndexBuffer" << " Class" << '\n';
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
	D3D11_BUFFER_DESC buff_desc = {};		//���� ��ũ����
	ZeroMemory(&buff_desc, sizeof(buff_desc));
	buff_desc.Usage = D3D11_USAGE_DEFAULT;
	buff_desc.ByteWidth = sizeof(UINT) * size_indices;		//����� ��ü�� ��üũ��
	buff_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;			//�ε������ۿ� ���ǹǷ� �ش��÷��׸� �����Ѵ�
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
	UINT offset = 0;				//������
	pDeviceContext->IASetIndexBuffer(m_pBuffer, DXGI_FORMAT_R32_UINT, offset); //4����Ʈ, 32��Ʈ���ǹ�
}

UINT IndexBuffer::GetCountIndices()
{
	return m_iSize_Indices;
}
