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
// HullShader
////////////////////////////////////////////////////////////////////////////////
HullShader::HullShader(ID3D11Device* pDevice, ID3DBlob* pBlob)
{
	_ASEERTION_NULCHK(pBlob, "BlobNULL");

	m_pBlob = pBlob;
	HRESULT hResult = pDevice->CreateHullShader(m_pBlob->GetBufferPointer(), m_pBlob->GetBufferSize(), nullptr, &m_pShader);
	_ASEERTION_CREATE(hResult, "HullShader");
}

////////////////////////////////////////////////////////////////////////////////
// DomainShader
////////////////////////////////////////////////////////////////////////////////
DomainShader::DomainShader(ID3D11Device* pDevice, ID3DBlob* pBlob)
{
	_ASEERTION_NULCHK(pBlob, "BlobNULL");

	m_pBlob = pBlob;
	HRESULT hResult = pDevice->CreateDomainShader(m_pBlob->GetBufferPointer(), m_pBlob->GetBufferSize(), nullptr, &m_pShader);
	_ASEERTION_CREATE(hResult, "DomainShader");
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
// ComputeShader
////////////////////////////////////////////////////////////////////////////////
ComputeShader::ComputeShader(ID3D11Device* pDevice, ID3DBlob* pBlob)
{
	_ASEERTION_NULCHK(pBlob, "BlobNULL");

	m_pBlob = pBlob;
	HRESULT hResult = pDevice->CreateComputeShader(m_pBlob->GetBufferPointer(), m_pBlob->GetBufferSize(), nullptr, &m_pShader);
	_ASEERTION_CREATE(hResult, "PixelShader");
}
