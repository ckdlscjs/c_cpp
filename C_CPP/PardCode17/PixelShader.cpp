#include "PixelShader.h"

PixelShader::PixelShader(ID3D11Device* pDevice, ID3DBlob* pBlob)
{
	std::cout << "PixelShader" << " Class" << " ������ ȣ��" << '\n';
	_ASEERTION_NULCHK(pBlob, "BlobNULL");

	HRESULT hResult = pDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &m_pPS);
	_ASEERTION_CREATE(hResult, "PixelShader");
}

PixelShader::~PixelShader()
{
	std::cout << "PixelShader" << " Class" << " �Ҹ��� ȣ��" << '\n';
	m_pPS->Release();
}

void PixelShader::SetPixelShader(ID3D11DeviceContext* pDeviceContext)
{
	pDeviceContext->PSSetShader(m_pPS, nullptr, 0);
}
