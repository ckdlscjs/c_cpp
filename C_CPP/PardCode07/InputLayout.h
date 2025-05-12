#pragma once
#include "std.h"

class InputLayout
{
public:
	InputLayout();
	~InputLayout();
	void Init(ID3D11Device* pDevice, D3D11_INPUT_ELEMENT_DESC* pInputElementDescs, UINT size_layout, void* shader_byte_code, SIZE_T size_byte_shader);
	void SetInputLayout(ID3D11DeviceContext* pDeviceContext);
	void Release();
private:
	ID3D11InputLayout* m_pInputLayout;
};

