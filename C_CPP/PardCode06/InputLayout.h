#pragma once
#include "std.h"

class InputLayout
{
public:
	InputLayout();
	~InputLayout();
	bool Init(ID3D11Device* pDevice, D3D11_INPUT_ELEMENT_DESC* pInputElementDescs, UINT size_layout, void* shader_byte_code, SIZE_T size_byte_shader);
	bool SetInputLayout(ID3D11DeviceContext* pDeviceContext);
	bool Release();
private:
	ID3D11InputLayout* m_pInputLayout;
};

