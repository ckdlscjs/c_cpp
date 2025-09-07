#include "VertexShader.h"

VertexShader::VertexShader(ID3D11Device* pDevice, ID3DBlob* pBlob)
{
	std::cout << "Initialize : " << "VertexShader" << " Class" << '\n';
	_ASEERTION_NULCHK(pBlob, "BlobNULL");

	m_pBlob = pBlob;

	HRESULT hResult = pDevice->CreateVertexShader(m_pBlob->GetBufferPointer(), m_pBlob->GetBufferSize(), nullptr, &m_pVS);
	_ASEERTION_CREATE(hResult, "VertexShader");
}

VertexShader::~VertexShader()
{
	std::cout << "Release : " << "VertexShader" << " Class" << '\n';
	m_pVS->Release();
	m_pBlob->Release();
}

void VertexShader::SetVertexShader(ID3D11DeviceContext* pDeviceContext)
{
	pDeviceContext->VSSetShader(m_pVS, nullptr, 0);
}

ID3DBlob* VertexShader::GetBlob() const
{
	return m_pBlob;
}
