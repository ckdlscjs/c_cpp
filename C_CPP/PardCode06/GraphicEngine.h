#pragma once
#include "std.h"

//SwapChain클래스 전방선언
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

	static GraphicEngine& Get(); //Singleton함수, 런타임중 호출시 정적변수로써 생성된다

	//쉐이더변수들, 추후 클래스로 분할 예정
private:
	ID3DBlob* m_pBlob_VS;
	ID3DBlob* m_pBlob_PS;
	ID3D11VertexShader* m_pVS;
	ID3D11PixelShader* m_pPS;

	//사용을위해 분할한 클래스객체들
private:
	class Direct3D* m_pCDirect3D;
	class SwapChain* m_pCSwapChain;
	class VertexBuffer* m_pCVertexBuffer;
	class InputLayout* m_pCInputLayout;
};