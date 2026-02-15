#pragma once
#include "BaseSystem.h"

//클래스 전방선언
class Window;
class SwapChain;
class Direct3D;
class VertexBuffer;
class InputLayout;
class VertexShader;
class GeometryShader;
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
class RenderAsset;
class ColliderAsset;

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
	~RenderSystem();
	void Init(HWND hWnd, UINT width, UINT height);
	void Frame(float deltatime, float elapsedtime);
	void PreRender(float deltatime, float elapsedtime);
	void Render(float deltatime, float elapsedtime);
	void PostRender();
	void OnResize(UINT width, UINT height);
	void SetViewportSize(UINT iWidth, UINT iHeight);
	void SetViewportSize(D3D11_VIEWPORT* pViewport, UINT iWidth, UINT iHeight);
	ID3D11Device* GetD3DDevice() const;
	ID3D11DeviceContext* GetD3DDeviceContext() const;
	
	/////////////////////////////
	//응용프로그램 부 접근
	/////////////////////////////
	//Texture
	size_t CreateTexture(const std::wstring& szFilePath, ScratchImage&& image);
	size_t CreateTexture(const std::wstring& szFilePath);

	//Geometry(Parsing)
	size_t CreateGeometry(const std::wstring& szFilePath);

	//Mesh
	template<typename T>
	size_t CreateMeshFromGeometry(const std::wstring& szName, const std::map<UINT, std::vector<Vertex_PTNTB_Skinned>>& verticesByMaterial, const std::map<UINT, std::vector<UINT>>& indicesByMaterial, const std::vector<std::vector<Vector3>>& pointsByMeshs);
	template<typename T>
	size_t CreateMeshFromGeometry(size_t hash_geometry);

	//Material
	size_t CreateMaterial(const std::wstring& szFilePath);
	std::vector<size_t> CreateMaterials(const std::wstring& szFilePath, std::map<UINT, MTL_TEXTURES>& texturesByMaterial);
	std::vector<size_t> CreateMaterialsFromGeometry(size_t hash_geometry);
	void Material_SetShaders(size_t hash_material, const UINT flag);
	void Material_SetVS(size_t hash_material, const std::wstring& vsName);
	template<typename T>
	void Material_SetIL(size_t hash_material, const std::wstring& vsName);
	void Material_SetGS(size_t hash_material, const std::wstring& gsName);
	void Material_SetPS(size_t hash_material, const std::wstring& psName);
	void Material_SetTextures(size_t hash_material, const std::vector<TX_HASH>& textures);

	//Animation
	size_t CreateAnimaitonFromGeometry(size_t hash_geometry);

	//Collider
	template<typename T>
	const std::unordered_set<size_t>& CreateColliders(size_t hash_mesh, E_Collider collider = E_Collider::AABB);

	//Asset(Components <-> API) 리소스 생성
	size_t CreateRenderAsset(const std::wstring& szName, const std::vector<Mesh_Material>& hashs);
	size_t CreateColliderAsset(const std::wstring& szName, const std::unordered_set<size_t>& hashs);
	const ColliderAsset* GetColliderAsset(size_t hash);

	//기법
	void CreateCubeMapTexture(int iSize);
	size_t CreateShadowMapTexture(const int width, const int height);


private:
	//API리소스 생성
	ID3DBlob* CompileShader(std::wstring shaderName, std::string entryName, std::string target);
	size_t CreateVertexBuffer(const std::wstring& szName, void* vertices, UINT size_vertex, UINT size_vertices);
	size_t CreateIndexBuffer(const std::wstring& szName, void* indices, UINT size_indices);
	size_t CreateInputLayout(const std::wstring& szName, D3D11_INPUT_ELEMENT_DESC* pInputElementDescs, UINT size_layout, ID3DBlob* vsBlob);
	size_t CreateVertexShader(std::wstring shaderName, std::string entryName, std::string target);
	size_t CreateGeometryShader(std::wstring shaderName, std::string entryName, std::string target);
	size_t CreatePixelShader(std::wstring shaderName, std::string entryName, std::string target);
	size_t CreateConstantBuffer(const type_info& typeinfo, UINT size_buffer, void* data = nullptr);
	size_t CreateShaderResourceView(const std::wstring& szName, const ScratchImage* resource);
	size_t CreateRenderTargetView(const std::wstring& szName);
	size_t CreateRenderTargetView(const std::wstring& szName, UINT width, UINT height);
	size_t CreateDepthStencilView(const std::wstring& szName, UINT width, UINT height);
	std::vector<size_t> CreateCubeMapViews(const int width, const int height);
	std::vector<size_t> CreateCubeMapView(const int width, const int height);

	//API 파이프라인
private:
	void ClearRenderViews(float red, float green, float blue, float alpha);

	void SetIA_Topology(D3D_PRIMITIVE_TOPOLOGY topology);
	void SetIA_InputLayout(ID3D11InputLayout* pInputLayout);
	void SetIA_VertexBuffer(ID3D11Buffer* pBuffer, UINT iSizeVertex, UINT offset = 0);
	void SetIA_IndexBuffer(ID3D11Buffer* pBuffer, UINT offset = 0);

	void SetVS_Shader(ID3D11VertexShader* pVS);
	void SetVS_ShaderResourceView(ID3D11ShaderResourceView* pSRV, UINT startIdx = 0);
	void SetVS_ConstantBuffer(ID3D11Buffer* pBuffer, UINT startIdx = 0);
	void SetVS_SamplerState(ID3D11SamplerState* pState, UINT startIdx = 0);

	void SetGS_Shader(ID3D11GeometryShader* pGS);

	void SetPS_Shader(ID3D11PixelShader* pPS);
	void SetPS_ShaderResourceView(ID3D11ShaderResourceView* pSRV, UINT startIdx = 0);
	void SetPS_ConstantBuffer(ID3D11Buffer* pBuffer, UINT startIdx = 0);
	void SetPS_SamplerState(ID3D11SamplerState* pState, UINT startIdx = 0);

	void SetRS_RasterizerState(ID3D11RasterizerState* pState);
	void SetRS_Viewport(D3D11_VIEWPORT* pViewport);
	void SetOM_DepthStenilState(ID3D11DepthStencilState* pState, UINT stencilRef = 1);
	void SetOM_BlendState(ID3D11BlendState* pState, const FLOAT* blendFactor, UINT sampleMask = 0xFFFFFFFF);

	void Draw_Vertices(UINT vertexCount, UINT startIdx);
	void Draw_Indices(UINT indexCount, UINT startIdx, INT vertexOffset);
	void SwapchainPresent(bool vsync);

	//RenderPass
private:
	void RenderSkySphere(const Matrix4x4& matView, const Matrix4x4& matProj);
	void RenderGeometry(const Matrix4x4& matView, const Matrix4x4& matProj);
	void RenderBillboard(const Vector3& campos, const Matrix4x4& matView, const Matrix4x4& matProj);
	void RenderShadowMap(const Matrix4x4& matView, const Matrix4x4& matProj);
	void RenderEnviornmentMap(const Matrix4x4& matView, const Matrix4x4& matProj);
	void RenderCubeMap();
	void RenderCubeMapTexture(UINT cubemapIdx);
	void RenderUI(const Matrix4x4& matOrtho);
 
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
	std::unordered_map<size_t, GeometryShader*>			m_pCGSs;
	std::unordered_map<size_t, PixelShader*>			m_pCPSs;
	std::unordered_map<size_t, ConstantBuffer*>			m_pCCBs;
	std::unordered_map<size_t, ShaderResourceView*>		m_pCSRVs;
	std::unordered_map<size_t, RenderTargetView*>		m_pCRTVs;
	std::unordered_map<size_t, DepthStencilView*>		m_pCDSVs;

	//AssetSystem분리필요?
	std::unordered_map<size_t, RenderAsset*>			m_pCRAs;
	std::unordered_map<size_t, ColliderAsset*>			m_pCCAs;

public:
	size_t												m_hash_RTV_BB;
	size_t												m_hash_RTV_0;
	size_t												m_hash_DSV_0;
	D3D11_VIEWPORT										m_vp_BB;

	//큐브맵 렌더링을 위한 변수들
	size_t												m_hash_RTV_CubeMap[6];
	size_t												m_hash_SRV_CubeMap;
	size_t												m_hash_DSV_CubeMap;
	D3D11_VIEWPORT										m_vp_CubeMap;
	size_t												m_hash_CubemapLookup;

	//그림자맵 렌더링을 위한 변수들
	size_t												m_hash_DSV_ShadowMap;
	size_t												m_hash_Mat_ShadowMap;

public:
};
//SingletonClasses
#define _RenderSystem RenderSystem::GetInstance()

//템플릿 추론 명시
template void RenderSystem::Material_SetIL<Vertex_PC>(size_t hash_material, const std::wstring& vsName);
template void RenderSystem::Material_SetIL<Vertex_PT>(size_t hash_material, const std::wstring& vsName);
template void RenderSystem::Material_SetIL<Vertex_PTN>(size_t hash_material, const std::wstring& vsName);
template void RenderSystem::Material_SetIL<Vertex_PTNTB>(size_t hash_material, const std::wstring& vsName);

template size_t RenderSystem::CreateMeshFromGeometry<Vertex_PTN>(size_t hash_geometry);
template size_t RenderSystem::CreateMeshFromGeometry<Vertex_PTNTB>(size_t hash_geometry);
template size_t RenderSystem::CreateMeshFromGeometry<Vertex_PTNTB_Skinned>(size_t hash_geometry);
template size_t RenderSystem::CreateMeshFromGeometry<Vertex_PT>(const std::wstring& szName, const std::map<UINT, std::vector<Vertex_PTNTB_Skinned>>& verticesByMaterial, const std::map<UINT, std::vector<UINT>>& indicesByMaterial, const std::vector<std::vector<Vector3>>& pointsByMeshs);
template size_t RenderSystem::CreateMeshFromGeometry<Vertex_PTN>(const std::wstring& szName, const std::map<UINT, std::vector<Vertex_PTNTB_Skinned>>& verticesByMaterial, const std::map<UINT, std::vector<UINT>>& indicesByMaterial, const std::vector<std::vector<Vector3>>& pointsByMeshs);
template size_t RenderSystem::CreateMeshFromGeometry<Vertex_PTNTB>(const std::wstring& szName, const std::map<UINT, std::vector<Vertex_PTNTB_Skinned>>& verticesByMaterial, const std::map<UINT, std::vector<UINT>>& indicesByMaterial, const std::vector<std::vector<Vector3>>& pointsByMeshs);
template const std::unordered_set<size_t>& RenderSystem::CreateColliders<Vertex_PTN>(size_t hash_mesh, E_Collider collider);
template const std::unordered_set<size_t>& RenderSystem::CreateColliders<Vertex_PTNTB>(size_t hash_mesh, E_Collider collider);
