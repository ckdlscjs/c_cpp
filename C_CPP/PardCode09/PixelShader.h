#pragma once
#include "std.h"

class PixelShader
{
public:
	PixelShader();
	~PixelShader();
	void Init(ID3D11Device* pDevice, ID3DBlob* pBlob);
	void Release();

	void SetPixelShader(ID3D11DeviceContext* pDeviceContext);
private:
	ID3D11PixelShader* m_pPS;
};

