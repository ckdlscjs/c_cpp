#include "PixelShader.h"

PixelShader::PixelShader(ID3D11Device* pDevice, ID3DBlob* pBlob)
{
	std::cout << "Initialize : " << "PixelShader" << " Class" << '\n';
	_ASEERTION_NULCHK(pBlob, "BlobNULL");

	m_pBlob = pBlob;
	HRESULT hResult = pDevice->CreatePixelShader(m_pBlob->GetBufferPointer(), m_pBlob->GetBufferSize(), nullptr, &m_pPS);
	_ASEERTION_CREATE(hResult, "PixelShader");
}

PixelShader::~PixelShader()
{
	std::cout << "Release : " << "PixelShader" << " Class" << '\n';
	m_pPS->Release();
	m_pBlob->Release();
}

void PixelShader::SetPixelShader(ID3D11DeviceContext* pDeviceContext)
{
	pDeviceContext->PSSetShader(m_pPS, nullptr, 0);
}
