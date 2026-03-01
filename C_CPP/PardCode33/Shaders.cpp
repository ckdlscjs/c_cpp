#include "Shaders.h"

////////////////////////////////////////////////////////////////////////////////
// VertexShader
////////////////////////////////////////////////////////////////////////////////
VertexShader::VertexShader(ID3D11Device* pDevice, ID3DBlob* pBlob)
{
	_ASEERTION_NULCHK(pBlob, "BlobNULL");

	m_pBlob = pBlob;

	HRESULT hResult = pDevice->CreateVertexShader(m_pBlob->GetBufferPointer(), m_pBlob->GetBufferSize(), nullptr, &m_pShader);
	_ASEERTION_CREATE(hResult, "VertexShader");
}

////////////////////////////////////////////////////////////////////////////////
// PixelShader
////////////////////////////////////////////////////////////////////////////////
PixelShader::PixelShader(ID3D11Device* pDevice, ID3DBlob* pBlob)
{
	_ASEERTION_NULCHK(pBlob, "BlobNULL");

	m_pBlob = pBlob;
	HRESULT hResult = pDevice->CreatePixelShader(m_pBlob->GetBufferPointer(), m_pBlob->GetBufferSize(), nullptr, &m_pShader);
	_ASEERTION_CREATE(hResult, "PixelShader");
}

////////////////////////////////////////////////////////////////////////////////
// GeometryShader
////////////////////////////////////////////////////////////////////////////////
GeometryShader::GeometryShader(ID3D11Device* pDevice, ID3DBlob* pBlob)
{
	_ASEERTION_NULCHK(pBlob, "BlobNULL");

	m_pBlob = pBlob;
	HRESULT hResult = pDevice->CreateGeometryShader(m_pBlob->GetBufferPointer(), m_pBlob->GetBufferSize(), nullptr, &m_pShader);
	_ASEERTION_CREATE(hResult, "GeometryShader");
}
