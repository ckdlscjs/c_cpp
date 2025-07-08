#include "PixelShader.h"

PixelShader::PixelShader()
{
	std::cout << "PixelShader" << " Class" << " 생성자 호출" << '\n';
}

PixelShader::~PixelShader()
{
	std::cout << "PixelShader" << " Class" << " 소멸자 호출" << '\n';
	Release();
}

void PixelShader::Init(ID3D11Device* pDevice, ID3DBlob* pBlob)
{
	_ASEERTION_NULCHK(pBlob, "BlobNULL");

	HRESULT hResult = pDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &m_pPS);
	_ASEERTION_CREATE(hResult, "PixelShader");
}


void PixelShader::Release()
{
	m_pPS->Release();
}

void PixelShader::SetPixelShader(ID3D11DeviceContext* pDeviceContext)
{
	pDeviceContext->PSSetShader(m_pPS, nullptr, 0);
}
