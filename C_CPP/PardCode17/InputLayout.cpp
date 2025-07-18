#include "InputLayout.h"

InputLayout::InputLayout(ID3D11Device* pDevice, D3D11_INPUT_ELEMENT_DESC* pInputElementDescs, UINT size_layout, void* shader_byte_code, SIZE_T size_byte_shader)
{
	std::cout << "InputLayout" << " Class" << " 생성자 호출" << '\n';
	HRESULT hResult = pDevice->CreateInputLayout(pInputElementDescs, size_layout, shader_byte_code, size_byte_shader, &m_pInputLayout);
	_ASEERTION_CREATE(hResult, "InputLayout");
}

InputLayout::~InputLayout()
{
	std::cout << "InputLayout" << " Class" << " 소멸자 호출" << '\n';
	m_pInputLayout->Release();
}

void InputLayout::SetInputLayout(ID3D11DeviceContext* pDeviceContext)
{
	pDeviceContext->IASetInputLayout(m_pInputLayout);
}
