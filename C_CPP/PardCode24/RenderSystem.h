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
class SamplerState;
class RasterizerState;
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
	void Frame(float deltatime);
	void PreRender();
	void Render(float deltatime);
	void PostRender();
	void Release();
	
	ID3D11Device* GetD3DDevice() const;
	ID3D11DeviceContext* GetD3DDeviceContext() const;

	void OnResize(UINT width, UINT height);
	VertexBuffer* CreateVertexBuffer(size_t hash, void* vertices, UINT size_vertex, UINT size_vertices);
	InputLayout* CreateInputLayout(size_t hash, D3D11_INPUT_ELEMENT_DESC* pInputElementDescs, UINT size_layout);
	IndexBuffer* CreateIndexBuffer(size_t hash, void* indices, UINT size_indices);
	ConstantBuffer* CreateConstantBuffer(size_t hash, UINT size_buffer, void* data = nullptr);
	VertexShader* CreateVertexShader(size_t hash, std::wstring shaderName, std::string entryName, std::string target);
	PixelShader* CreatePixelShader(size_t hash, std::wstring shaderName, std::string entryName, std::string target);
	size_t CreateTexture(const std::wstring& szFilePath, DirectX::WIC_FLAGS flag = DirectX::WIC_FLAGS::WIC_FLAGS_NONE, Samplers sampler = Samplers::WRAP_LINEAR);
	Texture2D* CreateTexture2D(size_t hash, const ScratchImage* resource, Samplers sampler);
	size_t CreateMesh(const std::wstring& szFilePath, Colliders collider = Colliders::AABB);
	ID3DBlob* CompileShader(std::wstring shaderName, std::string entryName, std::string target);

private:
	UINT m_iWidth = 800;
	UINT m_iHeight = 600;

	//사용을위해 분할한 클래스객체들
private:
	Direct3D* m_pCDirect3D;
	SwapChain* m_pCSwapChain;
	SamplerState* m_pCSamplers;
	RasterizerState* m_pCRSStaets;
	ID3DBlob* m_pBlob_VS;
	ID3DBlob* m_pBlob_PS;
	std::unordered_map<size_t, VertexBuffer*> m_pCVBs;
	std::unordered_map<size_t, IndexBuffer*> m_pCIBs;
	std::unordered_map<size_t, InputLayout*> m_pCILs;
	std::unordered_map<size_t, VertexShader*> m_pCVSs;
	std::unordered_map<size_t, PixelShader*> m_pCPSs;
	std::unordered_map<size_t, ConstantBuffer*> m_pCCBs;
	std::unordered_map<size_t, Texture2D*> m_pCTXs;

	//패리티체크를위해 1부터둔다
	size_t m_lIdx_CVBs = 1;
	size_t m_lIdx_CIBs = 1;
	size_t m_lIdx_CILs = 1;
	size_t m_lIdx_CVSs = 1;
	size_t m_lIdx_CPSs = 1;
	size_t m_lIdx_CCBs = 1;
	size_t m_lIdx_CTXs = 1;
public:
	//추후 오브젝트시스템으로분리
	TempObj* SkyObj;
	std::vector<TempObj*> objs;
};
//SingletonClasses
#define _RenderSystem RenderSystem::GetInstance()