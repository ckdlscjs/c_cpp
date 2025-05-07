#pragma once
#include "std.h"

//SwapChainŬ���� ���漱��
class Window;
class SwapChain;
class Direct3D;
class VertexBuffer;
class InputLayout;

class GraphicEngine
{
public:
	GraphicEngine();
	~GraphicEngine();
	
	bool Init(Window* pWindow);
	bool Frame();
	bool Render();
	bool Release();

	static GraphicEngine& Get(); //Singleton�Լ�, ��Ÿ���� ȣ��� ���������ν� �����ȴ�

	//���̴�������, ���� Ŭ������ ���� ����
private:
	ID3DBlob* m_pBlob_VS;
	ID3DBlob* m_pBlob_PS;
	ID3D11VertexShader* m_pVS;
	ID3D11PixelShader* m_pPS;

	//��������� ������ Ŭ������ü��
private:
	class Direct3D* m_pCDirect3D;
	class SwapChain* m_pCSwapChain;
	class VertexBuffer* m_pCVertexBuffer;
	class InputLayout* m_pCInputLayout;
};