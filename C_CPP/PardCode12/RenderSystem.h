#pragma once
#include "std.h"

//클래스 전방선언
class Window;
class SwapChain;
class Direct3D;
class VertexBuffer;
class InputLayout;
class VertexShader;
class PixelShader;
class ConstantBuffer;
class IndexBuffer;
class TempObj;
class RenderSystem
{
private:
	RenderSystem();
	~RenderSystem();
	RenderSystem(RenderSystem& renderSystem) = delete;
	RenderSystem& operator=(const RenderSystem& renderSystem) = delete;
public:
	static RenderSystem& GetInstance(); //Singleton함수, 런타임중 호출시 정적변수로써 생성된다
	void Init(HWND hWnd, UINT width, UINT height);
	void Frame();
	void Render();
	void Release();
	
	VertexBuffer* CreateVertexBuffer(void* vertices, UINT size_vertex, UINT size_vertices);
	InputLayout* CreateInputLayout(D3D11_INPUT_ELEMENT_DESC* pInputElementDescs, UINT size_layout);
	IndexBuffer* CreateIndexBuffer(void* indices, UINT size_indices);
	ConstantBuffer* CreateConstantBuffer(void* data, UINT size_buffer);
	ID3DBlob* CompileShader(std::wstring shaderName, std::string entryName, std::string target);
	VertexShader* CreateVertexShader(std::wstring shaderName, std::string entryName, std::string target);
	PixelShader* CreatePixelShader(std::wstring shaderName, std::string entryName, std::string target);

	//Chapter10, 시간경과에따른 임시변수
private:
	float m_fWidth = 800.0f;
	float m_fHeight = 600.0f;
	DWORD m_dwOldTick = 0;
	DWORD m_dwCurTick = 0;
	float m_fDeltatime = 0.0f;
	float m_fElapsedtime = 0.0f;

	//사용을위해 분할한 클래스객체들
private:
	Direct3D* m_pCDirect3D;
	SwapChain* m_pCSwapChain;
	ID3DBlob* m_pBlob_VS;
	ID3DBlob* m_pBlob_PS;
public:
	//추후 리소스시스템으로 분리해야함
	std::unordered_map<size_t, VertexBuffer*> m_pCVBs;
	std::unordered_map<size_t, IndexBuffer*> m_pCIBs;
	std::unordered_map<size_t, InputLayout*> m_pCILs;
	std::unordered_map<size_t, VertexShader*> m_pCVSs;
	std::unordered_map<size_t, PixelShader*> m_pCPSs;
	std::unordered_map<size_t, ConstantBuffer*> m_pCCBs;
	
	std::vector<TempObj*> objs;
};
//SingletonClasses
#define _RenderSystem RenderSystem::GetInstance()