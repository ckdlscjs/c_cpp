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
class BlendState;
class TempObj;

class RenderSystem : public BaseSystem<RenderSystem>
{
	friend class BaseSystem<RenderSystem>;
private:
	RenderSystem();
	RenderSystem(const RenderSystem&) = delete;
	RenderSystem& operator=(const RenderSystem&) = delete;
	RenderSystem(RenderSystem&&) = delete;
	RenderSystem& operator=(RenderSystem&&) = delete;
public:
	void Init(HWND hWnd, UINT width, UINT height);
	void Frame(float deltatime, float elapsedtime);
	void PreRender(float deltatime, float elapsedtime);
	void Render(float deltatime, float elapsedtime);
	void PostRender();
	void Release();
	void OnResize(UINT width, UINT height);
	void SetViewportSize(UINT iWidth, UINT iHeight);
	ID3D11Device* GetD3DDevice() const;
	ID3D11DeviceContext* GetD3DDeviceContext() const;

	//응용프로그램부 접근
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
	//API리소스 생성
	ID3DBlob* CompileShader(std::wstring shaderName, std::string entryName, std::string target);
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

private:
	//API 파이프라인
	void ClearRenderViews(float red, float green, float blue, float alpha);

	void SetIA_Topology(D3D_PRIMITIVE_TOPOLOGY topology);
	void SetIA_InputLayout(ID3D11InputLayout* pInputLayout);
	void SetIA_VertexBuffer(ID3D11Buffer* pBuffer, UINT iSizeVertex, UINT offset = 0);
	void SetIA_IndexBuffer(ID3D11Buffer* pBuffer, UINT offset = 0);

	void SetVS_Shader(ID3D11VertexShader* pVS);
	void SetVS_ShaderResourceView(ID3D11ShaderResourceView* pSRV, UINT startIdx = 0);
	void SetVS_ConstantBuffer(ID3D11Buffer* pBuffer, UINT startIdx = 0);
	void SetVS_SamplerState(ID3D11SamplerState* pState, UINT startIdx = 0);

	void SetPS_Shader(ID3D11PixelShader* pPS);
	void SetPS_ShaderResourceView(ID3D11ShaderResourceView* pSRV, UINT startIdx = 0);
	void SetPS_ConstantBuffer(ID3D11Buffer* pBuffer, UINT startIdx = 0);
	void SetPS_SamplerState(ID3D11SamplerState* pState, UINT startIdx = 0);

	void SetRS_RasterizerState(ID3D11RasterizerState* pState);
	void SetOM_DepthStenilState(ID3D11DepthStencilState* pState, UINT stencilRef = 1);
	void SetOM_BlendState(ID3D11BlendState* pState, const FLOAT* blendFactor, UINT sampleMask = 0xFFFFFFFF);

	void Draw_Vertices(UINT vertexCount, UINT startIdx);
	void Draw_Indices(UINT indexCount, UINT startIdx, INT vertexOffset);
	void SwapchainPresent(bool vsync);
private:
	UINT m_iWidth = 800;
	UINT m_iHeight = 600;

	//사용을위해 분할한 클래스객체들
private:
	Direct3D*											m_pCDirect3D = nullptr;
	SwapChain*											m_pCSwapChain = nullptr;
	SamplerState*										m_pCSamplers = nullptr;
	RasterizerState*									m_pCRasterizers = nullptr;
	DepthStencilState*									m_pCDepthStencils = nullptr;
	BlendState*											m_pCBlends = nullptr;
	std::unordered_map<size_t, VertexBuffer*>			m_pCVBs;
	std::unordered_map<size_t, IndexBuffer*>			m_pCIBs;
	std::unordered_map<size_t, InputLayout*>			m_pCILs;
	std::unordered_map<size_t, VertexShader*>			m_pCVSs;
	std::unordered_map<size_t, PixelShader*>			m_pCPSs;
	std::unordered_map<size_t, ConstantBuffer*>			m_pCCBs;
	std::unordered_map<size_t, ShaderResourceView*>		m_pCSRVs;
	std::unordered_map<size_t, RenderTargetView*>		m_pCRTVs;
	std::unordered_map<size_t, DepthStencilView*>		m_pCDSVs;
	size_t												m_hash_RTV_BB;
	size_t												m_hash_RTV_0;
	size_t												m_hash_DSV_0;
public:
	//추후 오브젝트시스템으로분리
	TempObj* SkyObj;
	TempObj* Gizmo;
	TempObj* SpaceShip;
	TempObj* ReflectPlane;
	std::vector<TempObj*> objs;
	std::vector<TempObj*> ortho_objs;
};
//SingletonClasses
#define _RenderSystem RenderSystem::GetInstance()

//템플릿 추론 명시
template size_t RenderSystem::CreateMesh<Vertex_PTN>(const std::wstring& szFilePath, E_Colliders collider);
template size_t RenderSystem::CreateMesh<Vertex_PTNTB>(const std::wstring& szFilePath, E_Colliders collider);
template size_t RenderSystem::CreateMaterial<Vertex_PC>(const std::wstring& szFilePath, const std::wstring& vsName, const std::wstring& psName);
template size_t RenderSystem::CreateMaterial<Vertex_PT>(const std::wstring& szFilePath, const std::wstring& vsName, const std::wstring& psName);
template size_t RenderSystem::CreateMaterial<Vertex_PTN>(const std::wstring& szFilePath, const std::wstring& vsName, const std::wstring& psName);
template size_t RenderSystem::CreateMaterial<Vertex_PTNTB>(const std::wstring& szFilePath, const std::wstring& vsName, const std::wstring& psName);
template std::vector<size_t> RenderSystem::CreateMaterials<Vertex_PTN>(const std::wstring& szFilePath, const std::vector<std::wstring>& vss, const std::vector<std::wstring>& pss);
template std::vector<size_t> RenderSystem::CreateMaterials<Vertex_PTNTB>(const std::wstring& szFilePath, const std::vector<std::wstring>& vss, const std::vector<std::wstring>& pss);
template size_t RenderSystem::CreateMeshFromGeometry<Vertex_PC>(const std::wstring szName, std::vector<std::vector<Vector3>>&& points, std::vector<Vertex_PC>&& vertices, std::vector<UINT>&& indices, E_Colliders collider);
template size_t RenderSystem::CreateMeshFromGeometry<Vertex_PT>(const std::wstring szName, std::vector<std::vector<Vector3>>&& points, std::vector<Vertex_PT>&& vertices, std::vector<UINT>&& indices, E_Colliders collider);
template size_t RenderSystem::CreateMeshFromGeometry<Vertex_PTN>(const std::wstring szName, std::vector<std::vector<Vector3>>&& points, std::vector<Vertex_PTN>&& vertices, std::vector<UINT>&& indices, E_Colliders collider);

