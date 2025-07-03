#pragma once
#include "std.h"

class VertexShader
{
public:
	VertexShader();
	~VertexShader();
	void Init(ID3D11Device* pDevice, ID3DBlob* pBlob);
	void Release();

	void SetVertexShader(ID3D11DeviceContext* pDeviceContext);
private:
	ID3D11VertexShader* m_pVS;
};

