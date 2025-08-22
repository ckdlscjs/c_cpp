#pragma once
#include "CommonHeader.h"

class VertexShader
{
public:
	VertexShader(ID3D11Device* pDevice, ID3DBlob* pBlob);
	~VertexShader();
	VertexShader(const VertexShader&) = delete;
	VertexShader& operator=(const VertexShader&) = delete;
	VertexShader(VertexShader&&) = delete;
	VertexShader& operator=(VertexShader&&) = delete;
	void SetVertexShader(ID3D11DeviceContext* pDeviceContext);
private:
	ID3D11VertexShader* m_pVS;
};

