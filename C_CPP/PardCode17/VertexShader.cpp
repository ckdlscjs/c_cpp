#include "VertexShader.h"

VertexShader::VertexShader(ID3D11Device* pDevice, ID3DBlob* pBlob)
{
	std::cout << "VertexShader" << " Class" << " 생성자 호출" << '\n';
	_ASEERTION_NULCHK(pBlob, "BlobNULL");

	HRESULT hResult = pDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &m_pVS);
	_ASEERTION_CREATE(hResult, "VertexShader");
}

VertexShader::~VertexShader()
{
	std::cout << "VertexShader" << " Class" << " 소멸자 호출" << '\n';
	m_pVS->Release();
}

void VertexShader::SetVertexShader(ID3D11DeviceContext* pDeviceContext)
{
	pDeviceContext->VSSetShader(m_pVS, nullptr, 0);
}
