#pragma once
#include "BaseSystem.h"

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
class Texture2D;
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
	size_t CreateTexture2D(const std::wstring& szName, const ScratchImage* resource);
	
private:
	UINT m_iWidth = 800;
	UINT m_iHeight = 600;

	//��������� ������ Ŭ������ü��
private:
	Direct3D*										m_pCDirect3D;
	SwapChain*										m_pCSwapChain;
	SamplerState*									m_pCSamplers;
	RasterizerState*								m_pCRSStaets;
	DepthStencilState*								m_pCDSStates;
	std::unordered_map<size_t, VertexBuffer*>		m_pCVBs;
	std::unordered_map<size_t, IndexBuffer*>		m_pCIBs;
	std::unordered_map<size_t, InputLayout*>		m_pCILs;
	std::unordered_map<size_t, VertexShader*>		m_pCVSs;
	std::unordered_map<size_t, PixelShader*>		m_pCPSs;
	std::unordered_map<size_t, ConstantBuffer*>		m_pCCBs;
	std::unordered_map<size_t, Texture2D*>			m_pCTXs;

public:
	//���� ������Ʈ�ý������κи�
	TempObj* SkyObj;
	std::vector<TempObj*> objs;
};
//SingletonClasses
#define _RenderSystem RenderSystem::GetInstance()


