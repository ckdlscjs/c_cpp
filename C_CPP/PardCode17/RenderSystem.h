#pragma once
#include "BaseSystem.h"

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
class Texture2D;
class TempObj;

class RenderSystem : public BaseSystem<RenderSystem>
{
	friend class BaseSystem<RenderSystem>;
private:
	RenderSystem();
	~RenderSystem();
	RenderSystem(const RenderSystem&) = delete;
	RenderSystem& operator=(const RenderSystem&) = delete;
	RenderSystem(RenderSystem&&) = delete;
	RenderSystem& operator=(RenderSystem&&) = delete;
public:
	void Init(HWND hWnd, UINT width, UINT height);
	void Frame();
	void Render();
	void Release();
	
	size_t CreateVertexBuffer(void* vertices, UINT size_vertex, UINT size_vertices);
	size_t CreateInputLayout(D3D11_INPUT_ELEMENT_DESC* pInputElementDescs, UINT size_layout);
	size_t CreateIndexBuffer(void* indices, UINT size_indices);
	size_t CreateConstantBuffer(void* data, UINT size_buffer);
	size_t CreateVertexShader(std::wstring shaderName, std::string entryName, std::string target);
	size_t CreatePixelShader(std::wstring shaderName, std::string entryName, std::string target);
	size_t CreateTexture(const std::wstring& szFilePath);
	size_t CreateTexture2D(const ScratchImage* resource);
	ID3DBlob* CompileShader(std::wstring shaderName, std::string entryName, std::string target);

private:
	UINT m_iWidth = 800;
	UINT m_iHeight = 600;
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
	std::unordered_map<size_t, VertexBuffer*> m_pCVBs;
	std::unordered_map<size_t, IndexBuffer*> m_pCIBs;
	std::unordered_map<size_t, InputLayout*> m_pCILs;
	std::unordered_map<size_t, VertexShader*> m_pCVSs;
	std::unordered_map<size_t, PixelShader*> m_pCPSs;
	std::unordered_map<size_t, ConstantBuffer*> m_pCCBs;
	std::unordered_map<size_t, Texture2D*> m_pCTXs;
	
	size_t m_lIdx_CVBs = 1;
	size_t m_lIdx_CIBs = 1;
	size_t m_lIdx_CILs = 1;
	size_t m_lIdx_CVSs = 1;
	size_t m_lIdx_CPSs = 1;
	size_t m_lIdx_CCBs = 1;
	size_t m_lIdx_CTXs = 1;
public:
	std::vector<TempObj*> objs;
};
//SingletonClasses
#define _RenderSystem RenderSystem::GetInstance()