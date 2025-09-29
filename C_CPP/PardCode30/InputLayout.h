#pragma once
#include "CommonHeader.h"

class InputLayout
{
public:
	InputLayout(ID3D11Device* pDevice, D3D11_INPUT_ELEMENT_DESC* pInputElementDescs, UINT size_layout, void* shader_byte_code, SIZE_T size_byte_shader);
	~InputLayout();
	InputLayout(const InputLayout&) = delete;
	InputLayout& operator=(const InputLayout&) = delete;
	InputLayout(InputLayout&&) = delete;
	InputLayout& operator=(InputLayout&&) = delete;
	ID3D11InputLayout* GetInputLayout();
private:
	ID3D11InputLayout* m_pInputLayout;
};

