#include "InputLayout.h"

InputLayout::InputLayout()
{
	std::cout << "InputLayout" << " Class" << " ������ ȣ��" << '\n';
}

InputLayout::~InputLayout()
{
	std::cout << "InputLayout" << " Class" << " �Ҹ��� ȣ��" << '\n';
	Release();
}

void InputLayout::Init(ID3D11Device* pDevice, D3D11_INPUT_ELEMENT_DESC* pInputElementDescs, UINT size_layout, void* shader_byte_code, SIZE_T size_byte_shader)
{
	HRESULT hResult = pDevice->CreateInputLayout(pInputElementDescs, size_layout, shader_byte_code, size_byte_shader, &m_pInputLayout);
	_ASEERTION_CREATE(hResult, "InputLayout");
}

void InputLayout::SetInputLayout(ID3D11DeviceContext* pDeviceContext)
{
	pDeviceContext->IASetInputLayout(m_pInputLayout);
}


void InputLayout::Release()
{
	m_pInputLayout->Release();
}
