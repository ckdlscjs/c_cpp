#pragma once
#include "BaseSystem.h"

//클래스 전방선언
class Direct3D;
class SwapChain;

class VertexBuffer;
class ConstantBuffer;
class IndexBuffer;
class StructBuffer;
class StagingBuffer;

class VertexShader;
class InputLayout;
class HullShader;
class DomainShader;
class GeometryShader;
class PixelShader;
class ComputeShader;

class ShaderResourceView;
class RenderTargetView;
class DepthStencilView;
class UnorderedAccessView;

class SamplerState;
class RasterizerState;
class DepthStencilState;
class BlendState;

class Archetype;

class EngineSystem : public BaseSystem<EngineSystem>
{
	friend class BaseSystem<EngineSystem>;
private:
	EngineSystem();
	EngineSystem(const EngineSystem&) = delete;
	EngineSystem& operator=(const EngineSystem&) = delete;
	EngineSystem(EngineSystem&&) = delete;
	EngineSystem& operator=(EngineSystem&&) = delete;

public:
	~EngineSystem();
	void Init();
	
	/////////////////////////////
	//응용프로그램 부 접근
	/////////////////////////////
	ID3D11Device* GetD3DDevice() const;
	ID3D11DeviceContext* GetD3DDeviceContext() const;
	IDXGISwapChain* GetSwapChain() const;

	/////////////////////////////
	//Create FromResources
	/////////////////////////////
	//Texture
	size_t CreateTexture(const std::wstring& szFilePath, ScratchImage&& image);
	size_t CreateTexture(const std::wstring& szFilePath);
	size_t CreateTexture(const std::wstring& szFilePath, size_t hashSRV);

	//Geometry
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
	void Material_SetVS(size_t hash_material, const std::wstring& vsName, const std::string& entryName = "vsmain", const std::string& target = "vs_5_0");
	template<typename T>
	void Material_SetIL(size_t hash_material, const std::wstring& vsName, const std::string& entryName = "vsmain", const std::string& target = "vs_5_0");
	void Material_SetHS(size_t hash_material, const std::wstring& hsName, const std::string& entryName = "hsmain", const std::string& target = "hs_5_0");
	void Material_SetDS(size_t hash_material, const std::wstring& dsName, const std::string& entryName = "dsmain", const std::string& target = "ds_5_0");
	void Material_SetGS(size_t hash_material, const std::wstring& gsName, const std::string& entryName = "gsmain", const std::string& target = "gs_5_0");
	void Material_SetPS(size_t hash_material, const std::wstring& psName, const std::string& entryName = "psmain", const std::string& target = "ps_5_0");
	void Material_SetCS(size_t hash_material, const std::wstring& csName, const std::string& entryName = "csmain", const std::string& target = "cs_5_0");
	void Material_SetTextures(size_t hash_material, const std::vector<TX_HASH>& textures);
	void Material_SetTopology(size_t hash_material, D3D_PRIMITIVE_TOPOLOGY topology);
	void Material_SetHashPass(size_t hash_material, E_RenderPass renderPass);
	void Material_SetHashShaders(size_t hash_material, uint32_t hash_shaders);
	void Material_SetHashStates(size_t hash_material, uint32_t hash_states);

	//Animation
	size_t CreateAnimaitonFromGeometry(size_t hash_geometry);

	//Collider
	const std::vector<size_t>& CreateColliders(size_t hash_mesh, E_Collider collider = E_Collider::BOX);
	size_t CreateComputeVertices(size_t hash_mesh);

	//Asset(Components <-> API) 리소스 생성
	size_t CreateRenderAsset(const std::wstring& szName, const Mesh_Material& hashs);
	size_t CreateComputeAsset(const std::wstring& szName, const std::vector<size_t>& hashs);
	//size_t CreateColliderAsset(const std::wstring& szName, const std::unordered_set<size_t>& hashs);

	/////////////////////////////
	//Create APIResources
	/////////////////////////////
public:
	size_t CreateInputLayout(const std::wstring& szName, D3D11_INPUT_ELEMENT_DESC* pInputElementDescs, UINT size_layout, ID3DBlob* vsBlob);
	size_t CreateVertexBuffer(const std::wstring& szName, UINT stride, UINT count, void* iTriangleCount);
	size_t CreateIndexBuffer(const std::wstring& szName, UINT stride, void* indices);
	size_t CreateConstantBuffer(const type_info& typeinfo, UINT stride, void* data = nullptr);
	size_t CreateStructBuffer(const std::wstring& szName, UINT stride, UINT count, void* data = nullptr);
	size_t CreateStagingBuffer(const std::wstring& szName, UINT stride, UINT count);

	ID3DBlob* CompileShader(std::wstring shaderName, std::string entryName, std::string target);
	size_t CreateVertexShader(std::wstring shaderName, std::string entryName, std::string target);
	size_t CreateHullShader(std::wstring shaderName, std::string entryName, std::string target);
	size_t CreateDomainShader(std::wstring shaderName, std::string entryName, std::string target);
	size_t CreateGeometryShader(std::wstring shaderName, std::string entryName, std::string target);
	size_t CreatePixelShader(std::wstring shaderName, std::string entryName, std::string target);
	size_t CreateComputeShader(std::wstring shaderName, std::string entryName, std::string target);

	ID3D11Resource* CreateD3DBuffer(const ScratchImage* image);
	ID3D11Resource* CreateD3DBuffer(const std::wstring& szName, UINT stride, UINT count, void* data);
	ID3D11Texture2D* CreateD3DBuffer(UINT bindFlags, UINT width, UINT height);
	size_t CreateShaderResourceView(size_t hash, ID3D11Resource* pBuffer, UINT bindFlags);
	size_t CreateRenderTargetView(size_t hash, ID3D11Resource* pBuffer, UINT bindFlags);
	size_t CreateDepthStencilView(size_t hash, ID3D11Resource* pBuffer, UINT bindFlags);
	size_t CreateUnorderedAccessView(size_t hash, ID3D11Resource* pBuffer, UINT bindFlags);
	size_t CreateViews(const std::wstring& szName, UINT bindFlags, UINT width, UINT height);
	size_t CreateViews(const std::wstring& szName, UINT stride, UINT count, void* data);

	void GenerateMipMaps(size_t hashSRV);
	void UpdateConstantBuffer(size_t hash, void* pData);
	void CopyResource(ID3D11Resource* pSrc, ID3D11Resource* pDst);
	void MappedBuffer(ID3D11Resource* pResource, D3D11_MAPPED_SUBRESOURCE* mappedResource);
	void UnMappedBuffer(ID3D11Resource* pResource);

	uint32_t GetRenderPassKey_Pass(E_RenderPass pass);
	uint32_t GetRenderPassKey_Shaders(size_t hashMaterial);
	uint32_t GetRenderPassKey_States(E_RSState stateRS, E_DSState stateDS, E_BSState stateBS, UINT ds_stencilref = 0, float* bs_factor = nullptr, UINT bs_mask = 0xFFFFFFFF);
	uint32_t GetRenderPassKey_Resources(size_t hashMesh, size_t hashMat, E_Collider collider, UINT idx = 0);
	uint32_t GetRenderPassKey_DistToCamera(float dist);
	_RPKey GenerateRenderPassHash(uint32_t hashPass, uint32_t hashShaders, uint32_t hashStates, uint32_t hashResources, uint32_t hashDist);
	void EnqueueRenderItem(_RPKey sortKey, Archetype* pArchetype, size_t entityRow, size_t entityCol, UINT renderCnt, UINT startIdx);
	void SortRenderItem();
	void ClearRenderItem();


public:
	template<typename T>
	T* GetAPIResource(const std::unordered_map<size_t, T*>& containter, size_t hash) const
	{ 
		auto iter = containter.find(hash);
		return iter != containter.end() ? iter->second : nullptr;
	}
	VertexBuffer*				GetVB(size_t hash)	const { return GetAPIResource<VertexBuffer>(m_pCVBs, hash); }
	IndexBuffer*				GetIB(size_t hash)	const { return GetAPIResource<IndexBuffer>(m_pCIBs, hash); }
	StructBuffer*				GetSTB(size_t hash) const { return GetAPIResource<StructBuffer>(m_pCSTBs, hash); }
	StagingBuffer*				GetSGB(size_t hash) const { return GetAPIResource<StagingBuffer>(m_pCSGBs, hash); }
	ConstantBuffer*				GetCB(size_t hash)	const { return GetAPIResource<ConstantBuffer>(m_pCCBs, hash); }
	InputLayout*				GetIL(size_t hash)	const { return GetAPIResource<InputLayout>(m_pCILs, hash); }
	VertexShader*				GetVS(size_t hash)	const { return GetAPIResource<VertexShader>(m_pCVSs, hash); }
	HullShader*					GetHS(size_t hash)	const { return GetAPIResource<HullShader>(m_pCHSs, hash); }
	DomainShader*				GetDS(size_t hash)	const { return GetAPIResource<DomainShader>(m_pCDSs, hash); }
	GeometryShader*				GetGS(size_t hash)	const { return GetAPIResource<GeometryShader>(m_pCGSs, hash); }
	PixelShader*				GetPS(size_t hash)	const { return GetAPIResource<PixelShader>(m_pCPSs, hash); }
	ComputeShader*				GetCS(size_t hash)	const { return GetAPIResource<ComputeShader>(m_pCCSs, hash); }
	ShaderResourceView*			GetSRV(size_t hash) const { return GetAPIResource<ShaderResourceView>(m_pCSRVs, hash); }
	RenderTargetView*			GetRTV(size_t hash) const { return GetAPIResource<RenderTargetView>(m_pCRTVs, hash); }
	DepthStencilView*			GetDSV(size_t hash) const { return GetAPIResource<DepthStencilView>(m_pCDSVs, hash); }
	UnorderedAccessView*		GetUAV(size_t hash) const { return GetAPIResource<UnorderedAccessView>(m_pCUAVs, hash); }

	SamplerState*				GetState_SS() const { return m_pCSamplers; }
	RasterizerState*			GetState_RS() const { return m_pCRasterizers; }
	DepthStencilState*			GetState_DS() const { return m_pCDepthStencils; }
	BlendState*					GetState_BS() const { return m_pCBlends; }

	
	
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
	std::unordered_map<size_t, StructBuffer*>			m_pCSTBs;
	std::unordered_map<size_t, StagingBuffer*>			m_pCSGBs;
	std::unordered_map<size_t, ConstantBuffer*>			m_pCCBs;
	std::unordered_map<size_t, InputLayout*>			m_pCILs;
	std::unordered_map<size_t, VertexShader*>			m_pCVSs;
	std::unordered_map<size_t, HullShader*>				m_pCHSs;
	std::unordered_map<size_t, DomainShader*>			m_pCDSs;
	std::unordered_map<size_t, GeometryShader*>			m_pCGSs;
	std::unordered_map<size_t, PixelShader*>			m_pCPSs;
	std::unordered_map<size_t, ComputeShader*>			m_pCCSs;
	std::unordered_map<size_t, ShaderResourceView*>		m_pCSRVs;
	std::unordered_map<size_t, RenderTargetView*>		m_pCRTVs;
	std::unordered_map<size_t, DepthStencilView*>		m_pCDSVs;
	std::unordered_map<size_t, UnorderedAccessView*>	m_pCUAVs;

public:
	//응용프로그램부변수들, 엔진단위에서 변경후 추후 개별변수로 제어필요
	size_t												m_hash_light_directional;
	size_t												m_hash_light_point;
	size_t												m_hash_light_spot;

	size_t												m_hash_RTV_BB;
	size_t												m_hash_RTV_0;
	size_t												m_hash_DSV_0;
	D3D11_VIEWPORT										m_vp_BB;

	//큐브맵 렌더링을 위한 변수들
	size_t												m_hash_RTV_CubeMap;
	size_t												m_hash_SRV_CubeMap;
	size_t												m_hash_DSV_CubeMap;
	size_t												m_hash_Mat_CubeMap_PTN;
	size_t												m_hash_Mat_CubeMap_PTN_Skinned;
	size_t												m_hash_Mat_CubeMap_PTNTB;
	size_t												m_hash_Mat_CubeMap_PTNTB_Skinned;
	D3D11_VIEWPORT										m_vp_CubeMap;

	//그림자맵 렌더링을 위한 변수들
	size_t												m_hash_DSV_ShadowMap;
	size_t												m_hash_Mat_ShadowMap;

	//아웃라인 렌더링을 위한 변수들
	float												m_fThickness = 1.0f;
	size_t												m_hash_Mat_Outline_PTN;
	size_t												m_hash_Mat_Outline_PTN_Skinned;
	size_t												m_hash_Mat_Outline_PTNTB;
	size_t												m_hash_Mat_Outline_PTNTB_Skinned;

	//디버그 렌더링을 위한 변수들
	size_t												m_hash_Mat_Debug_Box;
	size_t												m_hash_Mat_Debug_Sphere;

	//피킹엔티티lookup
	bool												bMouseOnGUI = false;
	size_t												m_hash_pickingLookup = _HashNotInitialize;
	size_t												m_hash_Mat_Picking;


	//RenderPass용 정수값
	std::unordered_map<size_t, uint16_t>				m_hRP_Shaders;
	std::unordered_map<size_t, uint8_t>					m_hRP_States;
	std::unordered_map<size_t, uint8_t>					m_hRP_Resources;
	std::vector<size_t>									m_resRP_Shaders;
	std::vector<RPStates>								m_resRP_States;
	std::vector<RPResources>							m_resRP_Resources;
	std::vector<RenderItem>								m_hRP_CommandQueue;		//수집후 sort
	
};

//SingletonClasses
#define _EngineSystem EngineSystem::GetInstance()

//템플릿 추론 명시
template void EngineSystem::Material_SetIL<Vertex_PC>(size_t hash_material, const std::wstring& vsName, const std::string& entryName, const std::string& target);
template void EngineSystem::Material_SetIL<Vertex_PT>(size_t hash_material, const std::wstring& vsName, const std::string& entryName, const std::string& target);
template void EngineSystem::Material_SetIL<Vertex_PTN>(size_t hash_material, const std::wstring& vsName, const std::string& entryName, const std::string& target);
template void EngineSystem::Material_SetIL<Vertex_PTNTB>(size_t hash_material, const std::wstring& vsName, const std::string& entryName, const std::string& target);

template size_t EngineSystem::CreateMeshFromGeometry<Vertex_PTN>(size_t hash_geometry);
template size_t EngineSystem::CreateMeshFromGeometry<Vertex_PTN_Skinned>(size_t hash_geometry);
template size_t EngineSystem::CreateMeshFromGeometry<Vertex_PTNTB>(size_t hash_geometry);
template size_t EngineSystem::CreateMeshFromGeometry<Vertex_PTNTB_Skinned>(size_t hash_geometry);
template size_t EngineSystem::CreateMeshFromGeometry<Vertex_PT>(const std::wstring& szName, const std::map<UINT, std::vector<Vertex_PTNTB_Skinned>>& verticesByMaterial, const std::map<UINT, std::vector<UINT>>& indicesByMaterial, const std::vector<std::vector<Vector3>>& pointsByMeshs);
template size_t EngineSystem::CreateMeshFromGeometry<Vertex_PTN>(const std::wstring& szName, const std::map<UINT, std::vector<Vertex_PTNTB_Skinned>>& verticesByMaterial, const std::map<UINT, std::vector<UINT>>& indicesByMaterial, const std::vector<std::vector<Vector3>>& pointsByMeshs);
template size_t EngineSystem::CreateMeshFromGeometry<Vertex_PTNTB>(const std::wstring& szName, const std::map<UINT, std::vector<Vertex_PTNTB_Skinned>>& verticesByMaterial, const std::map<UINT, std::vector<UINT>>& indicesByMaterial, const std::vector<std::vector<Vector3>>& pointsByMeshs);