#include "VertexShader.h"

VertexShader::VertexShader(ID3D11Device* pDevice, ID3DBlob* pBlob)
{
	std::cout << "VertexShader" << " Class" << " ������ ȣ��" << '\n';
	_ASEERTION_NULCHK(pBlob, "BlobNULL");

	HRESULT hResult = pDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &m_pVS);
	_ASEERTION_CREATE(hResult, "VertexShader");
}

VertexShader::~VertexShader()
{
	std::cout << "VertexShader" << " Class" << " �Ҹ��� ȣ��" << '\n';
	m_pVS->Release();
}

void VertexShader::SetVertexShader(ID3D11DeviceContext* pDeviceContext)
{
	pDeviceContext->VSSetShader(m_pVS, nullptr, 0);
}
