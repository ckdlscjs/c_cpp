#include "ConstantBuffer.h"

ConstantBuffer::ConstantBuffer()
{
	std::cout << "ConstantBuffer" << " Class" << " 생성자 호출" << '\n';
}

ConstantBuffer::~ConstantBuffer()
{
	std::cout << "ConstantBuffer" << " Class" << " 소멸자 호출" << '\n';
	Release();
}

void ConstantBuffer::Init(ID3D11Device* pDevice, void* data, UINT size_buffer)
{
	_ASEERTION_NULLCHK(!m_pBuffer, "ConstantBuffer NOTNULL");
	D3D11_BUFFER_DESC buff_desc;
	ZeroMemory(&buff_desc, sizeof(buff_desc));
	buff_desc.Usage = D3D11_USAGE_DEFAULT;
	buff_desc.ByteWidth = size_buffer;
	buff_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;			//상수버퍼에 사용되므로 디스크립션의 플래그를 상수버퍼로 지정한다
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

void ConstantBuffer::SetVS(ID3D11DeviceContext* pDeviceContext, UINT startIdx)
{
	/*
		//상수버퍼를 셰이더코드에서 정의할때 register(bX)에서 b뒤의 x번호와 맞춘다, 
		void VSSetConstantBuffers(
		UINT StartSlot,								// 바인딩을 시작할 버텍스 셰이더의 슬롯 인덱스 (0부터 D3D11_COMMONSHADER_CONSTANT_BUFFER_HW_SLOT_COUNT - 1)
		UINT NumBuffers,							// 바인딩할 상수 버퍼의 개수
		ID3D11Buffer *const *ppConstantBuffers);	// 바인딩할 상수 버퍼들의 배열 포인터
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
