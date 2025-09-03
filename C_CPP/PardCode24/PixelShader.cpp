#include "PixelShader.h"

PixelShader::PixelShader(ID3D11Device* pDevice, ID3DBlob* pBlob)
{
	std::cout << "Initialize : " << "PixelShader" << " Class" << '\n';
	_ASEERTION_NULCHK(pBlob, "BlobNULL");

	HRESULT hResult = pDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &m_pPS);
	_ASEERTION_CREATE(hResult, "PixelShader");
}

PixelShader::~PixelShader()
{
	std::cout << "Release : " << "PixelShader" << " Class" << '\n';
	m_pPS->Release();
}

void PixelShader::SetPixelShader(ID3D11DeviceContext* pDeviceContext)
{
	pDeviceContext->PSSetShader(m_pPS, nullptr, 0);
}
