#include "VertexShader.h"

VertexShader::VertexShader()
{
	std::cout << "VertexShader" << " Class" << " 생성자 호출" << '\n';
}

VertexShader::~VertexShader()
{
	std::cout << "VertexShader" << " Class" << " 소멸자 호출" << '\n';
	Release();
}

void VertexShader::Init(ID3D11Device* pDevice, ID3DBlob* pBlob)
{
	_ASEERTION_NULCHK(pBlob, "BlobNULL");

	HRESULT hResult = pDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &m_pVS);
	_ASEERTION_CREATE(hResult, "VertexShader");
}


void VertexShader::Release()
{
	m_pVS->Release();
}

void VertexShader::SetVertexShader(ID3D11DeviceContext* pDeviceContext)
{
	pDeviceContext->VSSetShader(m_pVS, nullptr, 0);
}
