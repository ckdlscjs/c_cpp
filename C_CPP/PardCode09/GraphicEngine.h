#pragma once
#include "std.h"

//SwapChain클래스 전방선언
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

	static GraphicEngine& Get(); //Singleton함수, 런타임중 호출시 정적변수로써 생성된다

private:
	ID3DBlob* m_pBlob_VS;
	ID3DBlob* m_pBlob_PS;

	//Chapter9, 시간경과에따른 임시변수
private:
	unsigned long m_lOldtime = 0;
	float m_fDeltatime = 0.0f;
	float m_fAngle = 0.0f;

	//사용을위해 분할한 클래스객체들
private:
	class Direct3D* m_pCDirect3D;
	class SwapChain* m_pCSwapChain;
	class VertexBuffer* m_pCVertexBuffer;
	class InputLayout* m_pCInputLayout;
	class VertexShader* m_pCVertexShader;
	class PixelShader* m_pCPixelShader;
	class ConstantBuffer* m_pCConstantBuffer;
};