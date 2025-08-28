#pragma once
#include "CommonHeader.h"

class PixelShader
{
public:
	PixelShader(ID3D11Device* pDevice, ID3DBlob* pBlob);
	~PixelShader();
	PixelShader(const PixelShader&) = delete;
	PixelShader& operator=(const PixelShader&) = delete;
	PixelShader(PixelShader&&) = delete;
	PixelShader& operator=(PixelShader&&) = delete;
	void SetPixelShader(ID3D11DeviceContext* pDeviceContext);
private:
	ID3D11PixelShader* m_pPS;
};

