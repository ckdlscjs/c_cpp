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
class ShaderResourceView;
class RenderTargetView;
class DepthStencilView;
class SamplerState;
class RasterizerState;
class DepthStencilState;
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
	
	ID3DBlob* CompileShader(std::wstring shaderName, std::string entryName, std::string target);
	size_t CreateTexture(const std::wstring& szFilePath, DirectX::DDS_FLAGS flag = DirectX::DDS_FLAGS::DDS_FLAGS_NONE);
	size_t CreateTexture(const std::wstring& szFilePath, DirectX::WIC_FLAGS flag = DirectX::WIC_FLAGS::WIC_FLAGS_NONE);
	template<typename T>
	size_t CreateMeshFromGeometry(const std::wstring szName, std::vector<std::vector<Vector3>>&& points, std::vector<T>&& vertices, std::vector<UINT>&& indices, E_Colliders collider = E_Colliders::AABB);
	template<typename T>
	size_t CreateMesh(const std::wstring& szFilePath, E_Colliders collider = E_Colliders::AABB);
	template<typename T>
	size_t CreateMaterial(const std::wstring& szFilePath, const std::wstring& vsName, const std::wstring& psName);
	template<typename T>
	std::vector<size_t> CreateMaterials(const std::wstring& szFilePath, const std::vector<std::wstring>& vss, const std::vector<std::wstring>& pss);
	void Material_SetTextures(size_t hash_material, const std::vector<TX_HASH>& textures);

private:
	size_t CreateVertexBuffer(const std::wstring& szName, void* vertices, UINT size_vertex, UINT size_vertices);
	size_t CreateIndexBuffer(const std::wstring& szName, void* indices, UINT size_indices);
	size_t CreateInputLayout(const std::wstring& szName, D3D11_INPUT_ELEMENT_DESC* pInputElementDescs, UINT size_layout, ID3DBlob* vsBlob);
	size_t CreateVertexShader(std::wstring shaderName, std::string entryName, std::string target);
	size_t CreatePixelShader(std::wstring shaderName, std::string entryName, std::string target);
	size_t CreateConstantBuffer(const type_info& typeinfo, UINT size_buffer, void* data = nullptr);
	size_t CreateShaderResourceView(const std::wstring& szName, const ScratchImage* resource);
	size_t CreateRenderTargetView(const std::wstring& szName);
	size_t CreateRenderTargetView(const std::wstring& szName, UINT width, UINT height);
	size_t CreateDepthStencilView(const std::wstring& szName, UINT width, UINT height);
	void ClearRenderViews(float red, float green, float blue, float alpha);
	
private:
	UINT m_iWidth = 800;
	UINT m_iHeight = 600;

	//사용을위해 분할한 클래스객체들
private:
	Direct3D*											m_pCDirect3D = nullptr;
	SwapChain*											m_pCSwapChain = nullptr;
	SamplerState*										m_pCSamplers = nullptr;
	RasterizerState*									m_pCRSStaets = nullptr;
	DepthStencilState*									m_pCDSStates = nullptr;
	RenderTargetView*									m_pCRTV = nullptr;
	DepthStencilView*									m_pCDSV = nullptr;
	std::unordered_map<size_t, VertexBuffer*>			m_pCVBs;
	std::unordered_map<size_t, IndexBuffer*>			m_pCIBs;
	std::unordered_map<size_t, InputLayout*>			m_pCILs;
	std::unordered_map<size_t, VertexShader*>			m_pCVSs;
	std::unordered_map<size_t, PixelShader*>			m_pCPSs;
	std::unordered_map<size_t, ConstantBuffer*>			m_pCCBs;
	std::unordered_map<size_t, ShaderResourceView*>		m_pCVWs;
public:
	//추후 오브젝트시스템으로분리
	TempObj* SkyObj;
	std::vector<TempObj*> objs;
	std::vector<TempObj*> ortho_objs;
};
//SingletonClasses
#define _RenderSystem RenderSystem::GetInstance()



