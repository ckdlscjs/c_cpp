#pragma once
#include "std.h"

//Ŭ���� ���漱��
class Window;
class SwapChain;
class Direct3D;
class VertexBuffer;
class InputLayout;
class VertexShader;
class PixelShader;
class ConstantBuffer;

class GraphicEngine
{
public:
	GraphicEngine();
	~GraphicEngine();
	
	void Init(HWND hWnd, UINT width, UINT height);
	void Frame();
	void Render();
	void Release();
	
	void CreateVertexBuffer(void* vertices, UINT size_vertex, UINT size_vertices);
	void CreateInputLayout(D3D11_INPUT_ELEMENT_DESC* pInputElementDescs, UINT size_layout);
	void CreateIndexBuffer();
	void CreateConstantBuffer();
	void CreateVertexShader(std::wstring shaderName, std::string entryName, std::string target);
	void CreatePixelShader(std::wstring shaderName, std::string entryName, std::string target);

	ID3DBlob* CompileShader(std::wstring shaderName, std::string entryName, std::string target);

	static GraphicEngine& Get(); //Singleton�Լ�, ��Ÿ���� ȣ��� ���������ν� �����ȴ�

private:
	ID3DBlob* m_pBlob_VS;
	ID3DBlob* m_pBlob_PS;

	//Chapter9, �ð���������� �ӽú���
private:
	unsigned long m_lOldtime = 0;
	float m_fDeltatime = 0.0f;
	float m_fAngle = 0.0f;

	//��������� ������ Ŭ������ü��
private:
	Direct3D* m_pCDirect3D;
	SwapChain* m_pCSwapChain;
	VertexBuffer* m_pCVertexBuffer;
	InputLayout* m_pCInputLayout;
	VertexShader* m_pCVertexShader;
	PixelShader* m_pCPixelShader;
	std::vector<ConstantBuffer*> m_pCConstantBuffers;
};