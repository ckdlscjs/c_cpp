#include "ConstantBuffer.h"

ConstantBuffer::ConstantBuffer()
{
	std::cout << "ConstantBuffer" << " Class" << " ������ ȣ��" << '\n';
}

ConstantBuffer::~ConstantBuffer()
{
	std::cout << "ConstantBuffer" << " Class" << " �Ҹ��� ȣ��" << '\n';
	Release();
}

void ConstantBuffer::Init(ID3D11Device* pDevice, void* data, UINT size_buffer)
{
	_ASEERTION_NULLCHK(!m_pBuffer, "ConstantBuffer NOTNULL");
	D3D11_BUFFER_DESC buff_desc;
	ZeroMemory(&buff_desc, sizeof(buff_desc));
	buff_desc.Usage = D3D11_USAGE_DEFAULT;
	buff_desc.ByteWidth = size_buffer;
	buff_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;			//������ۿ� ���ǹǷ� ��ũ������ �÷��׸� ������۷� �����Ѵ�
	buff_desc.CPUAccessFlags = 0;
	buff_desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA init_data = {};
	ZeroMemory(&init_data, sizeof(init_data));
	init_data.pSysMem = data;

	HRESULT hResult = pDevice->CreateBuffer(&buff_desc, &init_data, &m_pBuffer);
	_ASEERTION_CREATE(hResult, "ConstantBuffer");
}

void ConstantBuffer::UpdateBufferData(ID3D11DeviceContext* pDeviceContext, void* data)
{
	std::cout << "ConstantBuffer" << " Class" << " UpdateSubresource" << '\n';
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

void ConstantBuffer::SetVS(ID3D11DeviceContext* pDeviceContext, UINT startIdx)
{
	/*
		//������۸� ���̴��ڵ忡�� �����Ҷ� register(bX)���� b���� x��ȣ�� �����, 
		void VSSetConstantBuffers(
		UINT StartSlot,								// ���ε��� ������ ���ؽ� ���̴��� ���� �ε��� (0���� D3D11_COMMONSHADER_CONSTANT_BUFFER_HW_SLOT_COUNT - 1)
		UINT NumBuffers,							// ���ε��� ��� ������ ����
		ID3D11Buffer *const *ppConstantBuffers);	// ���ε��� ��� ���۵��� �迭 ������
	*/
	pDeviceContext->VSSetConstantBuffers(startIdx, 1, &m_pBuffer);
}

void ConstantBuffer::SetPS(ID3D11DeviceContext* pDeviceContext, UINT startIdx)
{
	pDeviceContext->PSSetConstantBuffers(startIdx, 1, &m_pBuffer);
}


void ConstantBuffer::Release()
{
	m_pBuffer->Release();
}
