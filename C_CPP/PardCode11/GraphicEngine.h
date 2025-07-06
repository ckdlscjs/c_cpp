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
class IndexBuffer;

class GraphicEngine
{
public:
	GraphicEngine();
	~GraphicEngine();
	
	void Init(HWND hWnd, UINT width, UINT height);
	void Frame();
	void Render();
	void Release();
	
	VertexBuffer* CreateVertexBuffer(void* vertices, UINT size_vertex, UINT size_vertices);
	InputLayout* CreateInputLayout(D3D11_INPUT_ELEMENT_DESC* pInputElementDescs, UINT size_layout);
	IndexBuffer* CreateIndexBuffer(void* indices, UINT size_indices);
	ConstantBuffer* CreateConstantBuffer();
	VertexShader* CreateVertexShader(std::wstring shaderName, std::string entryName, std::string target);
	PixelShader* CreatePixelShader(std::wstring shaderName, std::string entryName, std::string target);

	ID3DBlob* CompileShader(std::wstring shaderName, std::string entryName, std::string target);

	static GraphicEngine& Get(); //Singleton�Լ�, ��Ÿ���� ȣ��� ���������ν� �����ȴ�

private:
	ID3DBlob* m_pBlob_VS;
	ID3DBlob* m_pBlob_PS;

	//Chapter10, �ð���������� �ӽú���
private:
	float m_fWidth = 800.0f;
	float m_fHeight = 600.0f;
	unsigned long m_lOldtime = 0;
	unsigned long m_lNewtime = 0.0f;
	float m_fDeltatime = 0.0f;
	float m_fDeltaPos = 0.0f;
	float m_fDeltaScale = 0.0f;
	float m_fDeltaRotate = 0.0f;


	//��������� ������ Ŭ������ü��
private:
	Direct3D* m_pCDirect3D;
	SwapChain* m_pCSwapChain;
	VertexBuffer* m_pCVertexBuffer;
	IndexBuffer* m_pCIndexBuffer;
	InputLayout* m_pCInputLayout;
	VertexShader* m_pCVertexShader;
	PixelShader* m_pCPixelShader;
	std::vector<ConstantBuffer*> m_pCConstantBuffers;
};