#pragma once
#include "std.h"

//SwapChainŬ���� ���漱��
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
	class Direct3D* m_pCDirect3D;
	class SwapChain* m_pCSwapChain;
	class VertexBuffer* m_pCVertexBuffer;
	class InputLayout* m_pCInputLayout;
	class VertexShader* m_pCVertexShader;
	class PixelShader* m_pCPixelShader;
	class ConstantBuffer* m_pCConstantBuffer;
};