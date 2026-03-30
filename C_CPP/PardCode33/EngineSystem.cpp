#include "EngineSystem.h"

//Systems
#include "InputSystem.h"
#include "ResourceSystem.h"
#include "ECSSystem.h"
#include "CameraSystem.h"
#include "CollisionSystem.h"

//D3D Wrappers
#include "Direct3D.h"
#include "SwapChain.h"
#include "InputLayout.h"
#include "Buffers.h"
#include "States.h"
#include "Views.h"
#include "Shaders.h"

//Resources
#include "Texture.h"
#include "Mesh.h"
#include "Material.h"
#include "Assets.h"
#include "Geometry.h"
#include "Animation.h"

#include "TestBlockMacro.h"

//해시, 상수버퍼
size_t g_hash_cb_directionalLight;
size_t g_hash_cb_pointLight;
size_t g_hash_cb_spotLight;
size_t g_hash_cb_wvpitmat;
size_t g_hash_cb_time;
size_t g_hash_cb_campos;
size_t g_hash_cb_lightmat;
size_t g_hash_cb_bonemat;
size_t g_hash_cb_fog;
size_t g_hash_cb_debug_box;
size_t g_hash_cb_debug_sphere;
size_t g_hash_cb_cubemap;
size_t g_hash_cb_raytriangle;
size_t g_hash_stb_collisionResults;
size_t g_hash_sgb_collisionResults;

//해시, 디버그렌더
size_t g_hash_VS_Debug;
size_t g_hash_GS_Debug_Box;
size_t g_hash_PS_Debug_PC;
size_t g_hash_VS_Debug_Sphere;
size_t g_hash_HS_Debug_Sphere;
size_t g_hash_DS_Debug_Sphere;
size_t g_hash_PS_Picking;

EngineSystem::EngineSystem()
{
}

void EngineSystem::Init()
{
	//COM객체 사용을 위한 초기화, 텍스쳐사용에필요
	_ASEERTION_CREATE(CoInitialize(nullptr), "Coninitialize not successfully");

	m_pCDirect3D = new Direct3D();
	_ASEERTION_NULCHK(m_pCDirect3D, "D3D is nullptr");

	m_pCSwapChain = new SwapChain(m_pCDirect3D->GetDevice(), g_hWnd, g_iWidth, g_iHeight);
	_ASEERTION_NULCHK(m_pCSwapChain, "SC is nullptr");

	m_pCSamplers = new SamplerState(m_pCDirect3D->GetDevice());
	_ASEERTION_NULCHK(m_pCSamplers, "Samplers is nullptr");

	m_pCRasterizers = new RasterizerState(m_pCDirect3D->GetDevice());
	_ASEERTION_NULCHK(m_pCRasterizers, "Rasterizer is nullptr");

	m_pCDepthStencils = new DepthStencilState(m_pCDirect3D->GetDevice());
	_ASEERTION_NULCHK(m_pCDepthStencils, "DepthStencil is nullptr");

	m_pCBlends = new BlendState(m_pCDirect3D->GetDevice());
	_ASEERTION_NULCHK(m_pCDepthStencils, "DepthStencil is nullptr");

	m_vp_CubeMap.MinDepth = m_vp_BB.MinDepth = 0.0f;
	m_vp_CubeMap.MaxDepth = m_vp_BB.MaxDepth = 1.0f;

	//상수버퍼 기본세팅
	g_hash_cb_directionalLight	= _EngineSystem.CreateConstantBuffer(typeid(CB_DirectionalLight), sizeof(CB_DirectionalLight));
	g_hash_cb_pointLight		= _EngineSystem.CreateConstantBuffer(typeid(CB_PointLight), sizeof(CB_PointLight));
	g_hash_cb_spotLight			= _EngineSystem.CreateConstantBuffer(typeid(CB_SpotLight), sizeof(CB_SpotLight));
	g_hash_cb_wvpitmat			= _EngineSystem.CreateConstantBuffer(typeid(CB_WVPITMatrix), sizeof(CB_WVPITMatrix));
	g_hash_cb_time				= _EngineSystem.CreateConstantBuffer(typeid(CB_Time), sizeof(CB_Time));
	g_hash_cb_campos			= _EngineSystem.CreateConstantBuffer(typeid(CB_Campos), sizeof(CB_Campos));
	g_hash_cb_lightmat			= _EngineSystem.CreateConstantBuffer(typeid(CB_LightMatrix), sizeof(CB_LightMatrix));
	g_hash_cb_bonemat			= _EngineSystem.CreateConstantBuffer(typeid(CB_BoneMatrix), sizeof(CB_BoneMatrix));
	g_hash_cb_fog				= _EngineSystem.CreateConstantBuffer(typeid(CB_Fog), sizeof(CB_Fog));
	g_hash_cb_debug_box			= _EngineSystem.CreateConstantBuffer(typeid(CB_Debug_Box), sizeof(CB_Debug_Box));
	g_hash_cb_debug_sphere		= _EngineSystem.CreateConstantBuffer(typeid(CB_Debug_Sphere), sizeof(CB_Debug_Sphere));
	g_hash_cb_cubemap			= _EngineSystem.CreateConstantBuffer(typeid(CB_CubeMap), sizeof(CB_CubeMap));
	g_hash_cb_raytriangle		= _EngineSystem.CreateConstantBuffer(typeid(CB_RayTriangle), sizeof(CB_RayTriangle));

	

	//디버그용 셰이더
	g_hash_VS_Debug				= _EngineSystem.CreateVertexShader(L"VS_Debug.hlsl", "vsmain", "vs_5_0");
	g_hash_GS_Debug_Box			= _EngineSystem.CreateGeometryShader(L"GS_Debug_Box.hlsl", "gsmain", "gs_5_0");
	g_hash_PS_Debug_PC			= _EngineSystem.CreatePixelShader(L"PS_PC.hlsl", "psmain", "ps_5_0");
	g_hash_VS_Debug_Sphere		= _EngineSystem.CreateVertexShader(L"VS_Debug_Sphere.hlsl", "vsmain", "vs_5_0");
	g_hash_HS_Debug_Sphere		= _EngineSystem.CreateHullShader(L"HS_Debug_Sphere.hlsl", "hsmain", "hs_5_0");
	g_hash_DS_Debug_Sphere		= _EngineSystem.CreateDomainShader(L"DS_Debug_Sphere.hlsl", "dsmain", "ds_5_0");
	g_hash_PS_Picking			= _EngineSystem.CreatePixelShader(L"PS_Picking.hlsl", "psmain", "ps_5_0");

	//계산셰이더
	std::vector<STB_CollisionResults> tempData((UINT)1e5);
	g_hash_stb_collisionResults = _EngineSystem.CreateViews(L"CollisionResults", sizeof(STB_CollisionResults), tempData.size(), tempData.data());
	g_hash_sgb_collisionResults = _EngineSystem.CreateStagingBuffer(L"CollisionResults", sizeof(STB_CollisionResults), (UINT)1e5);
}

EngineSystem::~EngineSystem()
{
	CoUninitialize();

	for (auto iter = m_pCVBs.begin(); iter != m_pCVBs.end();)
	{
		delete iter->second;
		iter = m_pCVBs.erase(iter);
	}

	for (auto iter = m_pCIBs.begin(); iter != m_pCIBs.end();)
	{
		delete iter->second;
		iter = m_pCIBs.erase(iter);
	}

	for (auto iter = m_pCILs.begin(); iter != m_pCILs.end();)
	{
		delete iter->second;
		iter = m_pCILs.erase(iter);
	}

	for (auto iter = m_pCVSs.begin(); iter != m_pCVSs.end();)
	{
		delete iter->second;
		iter = m_pCVSs.erase(iter);
	}

	for (auto iter = m_pCHSs.begin(); iter != m_pCHSs.end();)
	{
		delete iter->second;
		iter = m_pCHSs.erase(iter);
	}

	for (auto iter = m_pCDSs.begin(); iter != m_pCDSs.end();)
	{
		delete iter->second;
		iter = m_pCDSs.erase(iter);
	}

	for (auto iter = m_pCGSs.begin(); iter != m_pCGSs.end();)
	{
		delete iter->second;
		iter = m_pCGSs.erase(iter);
	}

	for (auto iter = m_pCPSs.begin(); iter != m_pCPSs.end();)
	{
		delete iter->second;
		iter = m_pCPSs.erase(iter);
	}

	for (auto iter = m_pCCSs.begin(); iter != m_pCCSs.end();)
	{
		delete iter->second;
		iter = m_pCCSs.erase(iter);
	}

	for (auto iter = m_pCCBs.begin(); iter != m_pCCBs.end();)
	{
		delete iter->second;
		iter = m_pCCBs.erase(iter);
	}

	for (auto iter = m_pCSTBs.begin(); iter != m_pCSTBs.end();)
	{
		delete iter->second;
		iter = m_pCSTBs.erase(iter);
	}

	for (auto iter = m_pCSGBs.begin(); iter != m_pCSGBs.end();)
	{
		delete iter->second;
		iter = m_pCSGBs.erase(iter);
	}

	for (auto iter = m_pCSRVs.begin(); iter != m_pCSRVs.end();)
	{
		delete iter->second;
		iter = m_pCSRVs.erase(iter);
	}

	for (auto iter = m_pCDSVs.begin(); iter != m_pCDSVs.end();)
	{
		delete iter->second;
		iter = m_pCDSVs.erase(iter);
	}

	for (auto iter = m_pCRTVs.begin(); iter != m_pCRTVs.end();)
	{
		delete iter->second;
		iter = m_pCRTVs.erase(iter);
	}

	for (auto iter = m_pCUAVs.begin(); iter != m_pCUAVs.end();)
	{
		delete iter->second;
		iter = m_pCUAVs.erase(iter);
	}

	if (m_pCSamplers)
	{
		delete m_pCSamplers;
		m_pCSamplers = nullptr;
	}

	if (m_pCRasterizers)
	{
		delete m_pCRasterizers;
		m_pCRasterizers = nullptr;
	}

	if (m_pCDepthStencils)
	{
		delete m_pCDepthStencils;
		m_pCDepthStencils = nullptr;
	}

	if (m_pCBlends)
	{
		delete m_pCBlends;
		m_pCBlends = nullptr;
	}

	if (m_pCSwapChain)
	{
		delete m_pCSwapChain;
		m_pCSwapChain = nullptr;
	}

	if (m_pCDirect3D)
	{
		delete m_pCDirect3D;
		m_pCDirect3D = nullptr;
	}
}

ID3D11Device* EngineSystem::GetD3DDevice() const
{
	return m_pCDirect3D ? m_pCDirect3D->GetDevice() : nullptr;
}

ID3D11DeviceContext* EngineSystem::GetD3DDeviceContext() const
{
	return m_pCDirect3D ? m_pCDirect3D->GetDeviceContext() : nullptr;
}

IDXGISwapChain* EngineSystem::GetSwapChain() const
{
	return m_pCSwapChain ? m_pCSwapChain->GetSwapChain() : nullptr;
}

// 생성한 ScratchImage로부터 ID3D11Resource 객체를 생성한다
size_t EngineSystem::CreateTexture(const std::wstring& szFilePath, ScratchImage&& image)
{
	Texture* pTexture = _ResourceSystem.CreateResource<Texture>(szFilePath, std::move(image));
	ID3D11Resource* pBuffer = CreateD3DBuffer(pTexture->GetImage());
	pTexture->SetSRV(CreateShaderResourceView(Hasing_wstring(szFilePath + L"VW"), pBuffer, _TEX));
	pBuffer->Release(); //refCnt감소
	return pTexture->GetHash();
}

size_t EngineSystem::CreateTexture(const std::wstring& szFilePath)
{
	Texture* pTexture = _ResourceSystem.CreateResource<Texture>(szFilePath);
	ID3D11Resource* pBuffer = CreateD3DBuffer(pTexture->GetImage());
	pTexture->SetSRV(CreateShaderResourceView(Hasing_wstring(szFilePath + L"VW"), pBuffer, _TEX));
	pBuffer->Release(); //refCnt감소
	return pTexture->GetHash();
}

const std::vector<size_t>& EngineSystem::CreateColliders(size_t hash_mesh, E_Collider collider)
{
	BaseMesh* pMesh = _ResourceSystem.GetResource<BaseMesh>(hash_mesh);
	if (pMesh->GetCLs().empty())
	{
		if (pMesh->GetPointsByBones().size())
		{
			for (UINT idx = 0; idx < pMesh->GetPointsByBones().size(); idx++)
				pMesh->SetCL(_CollisionSystem.CreateCollider(pMesh->GetPath() + std::to_wstring(idx), &pMesh->GetPointsByBones()[idx], collider));
		}
		else
		{
			for (UINT idx = 0; idx < pMesh->GetPointsByMeshs().size(); idx++)
				pMesh->SetCL(_CollisionSystem.CreateCollider(pMesh->GetPath() + std::to_wstring(idx), &pMesh->GetPointsByMeshs()[idx], collider));
		}
	}
	return pMesh->GetCLs();
}

size_t EngineSystem::CreateComputeVertices(size_t hash_mesh)
{
	BaseMesh* pMesh = _ResourceSystem.GetResource<BaseMesh>(hash_mesh);
	//Compute_SRV Collision을 위한 Buffer/View자원 구성
	std::vector<STB_CollisionVertices> datas;
	auto Indices = pMesh->GetIndicies();
	for (UINT idx = 0; idx < Indices.size(); idx++)
	{
		auto pos = pMesh->GetPosition(idx);
		auto bw = pMesh->GetBW(idx);
		auto bones = bw.first;
		auto weights = bw.second;
		datas.push_back({ pos, bones, weights });
	}
	pMesh->SetSTB(CreateViews(pMesh->GetPath() + _tocw(typeid(STB_CollisionVertices).name()), sizeof(STB_CollisionVertices), Indices.size(), datas.data()));
	return pMesh->GetSTB();
}

size_t EngineSystem::CreateRenderAsset(const std::wstring& szName, const std::vector<Mesh_Material>& hashs)
{
	RenderAsset* pAsset = _ResourceSystem.CreateResource<RenderAsset>(szName);
	pAsset->m_hMeshMats = hashs;
	return pAsset->GetHash();
}

size_t EngineSystem::CreateComputeAsset(const std::wstring& szName, const std::vector<size_t>& hashs)
{
	ComputeAsset* pAsset = _ResourceSystem.CreateResource<ComputeAsset>(szName);
	pAsset->m_hComputeMats = hashs;
	return pAsset->GetHash();
}

size_t EngineSystem::CreateMaterial(const std::wstring& szFilePath)
{
	Material* pMaterial = _ResourceSystem.CreateResource<Material>(szFilePath);
	return pMaterial->GetHash();
}

std::vector<size_t> EngineSystem::CreateMaterials(const std::wstring& szFilePath, std::map<UINT, MTL_TEXTURES>& texturesByMaterial)
{
	std::vector<size_t> hashs_material;
	for (auto& matInfo : texturesByMaterial)
	{
		const std::wstring& matName = szFilePath + _tomw(matInfo.second.szMatName);
		Material* pMaterial = _ResourceSystem.CreateResource<Material>(matName);
		size_t matHash = pMaterial->GetHash();
		UINT materialFlag = 0;
		//Parsing Texture
		if (matInfo.second.type_textures_image.size())
		{
			//생성된스크래치이미지가있을경우
			for (auto& type_textures_image : matInfo.second.type_textures_image)
			{
				E_Texture curType = ConvETexture(type_textures_image.first);
				materialFlag |= (UINT)curType;
				std::vector<TX_HASH> tx_hashs;
				for (auto& scratchImage : type_textures_image.second)
				{
					tx_hashs.push_back({ curType, _EngineSystem.CreateTexture(matName + GetTexType(curType),  std::move(scratchImage)) });
				}
				_EngineSystem.Material_SetTextures(matHash, tx_hashs);
			}
		}
		else
		{
			//경로만있을경우
			for (const auto& type_textures_path : matInfo.second.type_textures_path)
			{
				E_Texture curType = ConvETexture(type_textures_path.first);
				materialFlag |= (UINT)curType;
				std::vector<TX_HASH> tx_hashs;
				for (const auto& tex_path : type_textures_path.second)
				{
					tx_hashs.push_back({ curType, _EngineSystem.CreateTexture(_tomw(tex_path)) });
				}
				_EngineSystem.Material_SetTextures(matHash, tx_hashs);
			}
		}

		//Set Use Shaders
		Material_SetShaders(matHash, materialFlag);

		hashs_material.push_back(matHash);
	}
	return hashs_material;
}

size_t EngineSystem::CreateGeometry(const std::wstring& szFilePath)
{
	Geometry* pGeometry = _ResourceSystem.CreateResource<Geometry>(szFilePath);
	return pGeometry->GetHash();
}

std::vector<size_t> EngineSystem::CreateMaterialsFromGeometry(size_t hash_geometry)
{
	Geometry* pGeometry = _ResourceSystem.GetResource<Geometry>(hash_geometry);
	std::wstring szPath = pGeometry->GetPath();
	return CreateMaterials(szPath + L"Material", pGeometry->GetTextures());
}

size_t EngineSystem::CreateAnimaitonFromGeometry(size_t hash_geometry)
{
	Geometry* pGeometry = _ResourceSystem.GetResource<Geometry>(hash_geometry);
	std::wstring szPath = pGeometry->GetPath();
	Animation* pAnimation = _ResourceSystem.CreateResource<Animation>(szPath + L"Animation", pGeometry->GetBones(), pGeometry->GetBonesHierarchy(), pGeometry->GetAnimationClip());
	return pAnimation->GetHash();
}

template<typename T>
size_t EngineSystem::CreateMeshFromGeometry(const std::wstring& szName, const std::map<UINT, std::vector<Vertex_PTNTB_Skinned>>& verticesByMaterial, const std::map<UINT, std::vector<UINT>>& indicesByMaterial, const std::vector<std::vector<Vector3>>& pointsByMeshs)
{
	std::wstring szTypename = _tomw(typeid(T).name());
	Mesh<T>* pMesh = _ResourceSystem.CreateResource<Mesh<T>>(szName + szTypename + L"Mesh", verticesByMaterial, indicesByMaterial, pointsByMeshs, std::vector<std::vector<Vector3>>());
	pMesh->SetVB(CreateVertexBuffer(szName + szTypename + L"VB", sizeof(T), (UINT)pMesh->GetVerticesSize(), pMesh->GetVertices()));
	pMesh->SetIB(CreateIndexBuffer(szName + szTypename + L"IB", (UINT)pMesh->GetIndicies().size(), (UINT*)pMesh->GetIndicies().data()));
	return pMesh->GetHash();
}

template<typename T>
size_t EngineSystem::CreateMeshFromGeometry(size_t hash_geometry)
{
	Geometry* pGeometry = _ResourceSystem.GetResource<Geometry>(hash_geometry);
	std::wstring szPath = pGeometry->GetPath();
	std::wstring szTypename = _tomw(typeid(T).name());
	Mesh<T>* pMesh = _ResourceSystem.CreateResource<Mesh<T>>(szPath + szTypename + L"Mesh", pGeometry->GetVertices(), pGeometry->GetIndices(), pGeometry->GetPointsByMeshs(), pGeometry->GetPointsByBones());
	pMesh->SetVB(CreateVertexBuffer(szPath + szTypename + L"VB", sizeof(T), (UINT)pMesh->GetVerticesSize(), pMesh->GetVertices()));
	pMesh->SetIB(CreateIndexBuffer(szPath + szTypename + L"IB", (UINT)pMesh->GetIndicies().size(), (UINT*)pMesh->GetIndicies().data()));
	return pMesh->GetHash();
}

void EngineSystem::Material_SetShaders(size_t hash_material, const UINT flag)
{
	_EngineSystem.Material_SetVS(hash_material, L"VS_PTN.hlsl");
	_EngineSystem.Material_SetPS(hash_material, L"PS_PTN.hlsl");
	_EngineSystem.Material_SetIL<Vertex_PTN>(hash_material, L"VS_PTN.hlsl");
	if (flag == 1)
	{
		_EngineSystem.Material_SetVS(hash_material, L"VS_PTNTB.hlsl");
		_EngineSystem.Material_SetPS(hash_material, L"PS_PTNTB.hlsl");
		_EngineSystem.Material_SetIL<Vertex_PTNTB>(hash_material, L"VS_PTNTB.hlsl");
	}
	if (flag == 2)
	{
		_EngineSystem.Material_SetVS(hash_material, L"VS_PTN_Skinned.hlsl");
		_EngineSystem.Material_SetPS(hash_material, L"PS_PTN.hlsl");
		_EngineSystem.Material_SetIL<Vertex_PTN_Skinned>(hash_material, L"VS_PTN_Skinned.hlsl");
	}
	if (flag == 3)
	{
		_EngineSystem.Material_SetVS(hash_material, L"VS_PTNTB_Skinned.hlsl");
		_EngineSystem.Material_SetPS(hash_material, L"PS_PTNTB.hlsl");
		_EngineSystem.Material_SetIL<Vertex_PTNTB_Skinned>(hash_material, L"VS_PTNTB_Skinned.hlsl");
	}
	/*
	* 텍스쳐 분류에따른 쉐이더선택
	*/
	if (flag == ((UINT)E_Texture::Diffuse | (UINT)E_Texture::Normal | (UINT)E_Texture::Specular))
	{

	}
}

void EngineSystem::Material_SetVS(size_t hash_material, const std::wstring& vsName)
{
	Material* pMaterial = _ResourceSystem.GetResource<Material>(hash_material);
	pMaterial->SetVS(CreateVertexShader(vsName, "vsmain", "vs_5_0"));
}

template<typename T>
void EngineSystem::Material_SetIL(size_t hash_material, const std::wstring& vsName)
{
	Material* pMaterial = _ResourceSystem.GetResource<Material>(hash_material);
	pMaterial->SetIL(CreateInputLayout(vsName + L"IL", Traits_InputLayout<T>::GetLayout(), Traits_InputLayout<T>::GetSize(), m_pCVSs[CreateVertexShader(vsName, "vsmain", "vs_5_0")]->GetBlob()));
}

void EngineSystem::Material_SetHS(size_t hash_material, const std::wstring& hsName)
{
	Material* pMaterial = _ResourceSystem.GetResource<Material>(hash_material);
	pMaterial->SetHS(CreateHullShader(hsName, "gsmain", "gs_5_0"));
}

void EngineSystem::Material_SetDS(size_t hash_material, const std::wstring& dsName)
{
	Material* pMaterial = _ResourceSystem.GetResource<Material>(hash_material);
	pMaterial->SetDS(CreateDomainShader(dsName, "dsmain", "ds_5_0"));
}

void EngineSystem::Material_SetGS(size_t hash_material, const std::wstring& gsName)
{
	Material* pMaterial = _ResourceSystem.GetResource<Material>(hash_material);
	pMaterial->SetGS(CreateGeometryShader(gsName, "gsmain", "gs_5_0"));
}

void EngineSystem::Material_SetPS(size_t hash_material, const std::wstring& psName)
{
	Material* pMaterial = _ResourceSystem.GetResource<Material>(hash_material);
	pMaterial->SetPS(CreatePixelShader(psName, "psmain", "ps_5_0"));
}

void EngineSystem::Material_SetCS(size_t hash_material, const std::wstring& csName)
{
	Material* pMaterial = _ResourceSystem.GetResource<Material>(hash_material);
	pMaterial->SetCS(CreateComputeShader(csName, "csmain", "cs_5_0"));
}

void EngineSystem::Material_SetTextures(size_t hash_material, const std::vector<TX_HASH>& textures)
{
	Material* pMaterial = _ResourceSystem.GetResource<Material>(hash_material);
	for (const auto& iter : textures)
		pMaterial->SetTexture(iter);
}

/////////////////////////////
//Create APIResources
/////////////////////////////
size_t EngineSystem::CreateInputLayout(const std::wstring& szName, D3D11_INPUT_ELEMENT_DESC* pInputElementDescs, UINT size_layout, ID3DBlob* vsBlob)
{
	size_t hash = Hasing_wstring(szName);
	if (m_pCILs.find(hash) != m_pCILs.end()) return hash;
	_ASEERTION_NULCHK(vsBlob, "Blob is nullptr");
	InputLayout* pInputLayout = new InputLayout(m_pCDirect3D->GetDevice(), pInputElementDescs, size_layout, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize());
	_ASEERTION_NULCHK(pInputLayout, "IL is nullptr");
	m_pCILs[hash] = pInputLayout;
	return hash;
}

size_t EngineSystem::CreateVertexBuffer(const std::wstring& szName, UINT stride, UINT count, void* iTriangleCount)
{
	size_t hash = Hasing_wstring(szName);
	if (m_pCVBs.find(hash) != m_pCVBs.end()) return hash;
	VertexBuffer* pBuffer = new VertexBuffer(m_pCDirect3D->GetDevice(), iTriangleCount, stride, count);
	_ASEERTION_NULCHK(pBuffer, "VB is nullptr");
	m_pCVBs[hash] = pBuffer;
	return hash;
}

size_t EngineSystem::CreateIndexBuffer(const std::wstring& szName, UINT stride, void* indices)
{
	size_t hash = Hasing_wstring(szName);
	if (m_pCIBs.find(hash) != m_pCIBs.end()) return hash;
	IndexBuffer* pBuffer = new IndexBuffer(m_pCDirect3D->GetDevice(), indices, stride);
	_ASEERTION_NULCHK(pBuffer, "IB is nullptr");
	m_pCIBs[hash] = pBuffer;
	return hash;
}

size_t EngineSystem::CreateConstantBuffer(const type_info& typeinfo, UINT stride, void* data)
{
	size_t hash = typeinfo.hash_code();
	if (m_pCCBs.find(hash) != m_pCCBs.end()) return hash;
	ConstantBuffer* pBuffer = new ConstantBuffer(m_pCDirect3D->GetDevice(), data, stride);
	_ASEERTION_NULCHK(pBuffer, "CB is nullptr");
	m_pCCBs[hash] = pBuffer;
	return hash;
}

size_t EngineSystem::CreateStructBuffer(const std::wstring& szName, UINT stride, UINT count, void* data)
{
	size_t hash = Hasing_wstring(szName);
	if (m_pCSTBs.find(hash) != m_pCSTBs.end()) return hash;
	StructBuffer* pBuffer = new StructBuffer(m_pCDirect3D->GetDevice(), data, stride, count);
	_ASEERTION_NULCHK(pBuffer, "STB is nullptr");
	m_pCSTBs[hash] = pBuffer;
	return hash;
}

size_t EngineSystem::CreateStagingBuffer(const std::wstring& szName, UINT stride, UINT count)
{
	size_t hash = Hasing_wstring(szName);
	if (m_pCSGBs.find(hash) != m_pCSGBs.end()) return hash;
	StagingBuffer* pBuffer = new StagingBuffer(m_pCDirect3D->GetDevice(), stride, count);
	_ASEERTION_NULCHK(pBuffer, "SGB is nullptr");
	m_pCSGBs[hash] = pBuffer;
	return hash;
}

ID3DBlob* EngineSystem::CompileShader(std::wstring shaderName, std::string entryName, std::string target)
{
	ID3DBlob* pBlob;
	ID3DBlob* errBlob;
	HRESULT hResult;
	DWORD dwShaderFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
	//compile Shader
	//D3D_COMPILE_STANDARD_FILE_INCLUDE, include를 위해 추가
	hResult = D3DCompileFromFile(shaderName.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, entryName.c_str(), target.c_str(), dwShaderFlags, NULL, &pBlob, &errBlob);
	if (FAILED(hResult))
	{
		OutputDebugStringA((char*)errBlob->GetBufferPointer());
		if (errBlob)
			errBlob->Release();
		_ASEERTION_CREATE(hResult, "CompileShader");
	}
	return pBlob;
}

size_t EngineSystem::CreateVertexShader(std::wstring shaderName, std::string entryName, std::string target)
{
	size_t hash = Hasing_wstring(shaderName);
	if (m_pCVSs.find(hash) != m_pCVSs.end()) return hash;

	VertexShader* pShader = new VertexShader(m_pCDirect3D->GetDevice(), CompileShader(shaderName, entryName, target));
	_ASEERTION_NULCHK(pShader, "VS is nullptr");
	m_pCVSs[hash] = pShader;
	return hash;
}

size_t EngineSystem::CreateHullShader(std::wstring shaderName, std::string entryName, std::string target)
{
	size_t hash = Hasing_wstring(shaderName);
	if (m_pCHSs.find(hash) != m_pCHSs.end()) return hash;

	HullShader* pShader = new HullShader(m_pCDirect3D->GetDevice(), CompileShader(shaderName, entryName, target));
	_ASEERTION_NULCHK(pShader, "GS is nullptr");
	m_pCHSs[hash] = pShader;
	return hash;
}

size_t EngineSystem::CreateDomainShader(std::wstring shaderName, std::string entryName, std::string target)
{
	size_t hash = Hasing_wstring(shaderName);
	if (m_pCDSs.find(hash) != m_pCDSs.end()) return hash;

	DomainShader* pShader = new DomainShader(m_pCDirect3D->GetDevice(), CompileShader(shaderName, entryName, target));
	_ASEERTION_NULCHK(pShader, "GS is nullptr");
	m_pCDSs[hash] = pShader;
	return hash;
}

size_t EngineSystem::CreateGeometryShader(std::wstring shaderName, std::string entryName, std::string target)
{
	size_t hash = Hasing_wstring(shaderName);
	if (m_pCGSs.find(hash) != m_pCGSs.end()) return hash;

	GeometryShader* pShader = new GeometryShader(m_pCDirect3D->GetDevice(), CompileShader(shaderName, entryName, target));
	_ASEERTION_NULCHK(pShader, "GS is nullptr");
	m_pCGSs[hash] = pShader;
	return hash;
}

size_t EngineSystem::CreatePixelShader(std::wstring shaderName, std::string entryName, std::string target)
{
	size_t hash = Hasing_wstring(shaderName);
	if (m_pCPSs.find(hash) != m_pCPSs.end()) return hash;

	PixelShader* pShader = new PixelShader(m_pCDirect3D->GetDevice(), CompileShader(shaderName, entryName, target));
	_ASEERTION_NULCHK(pShader, "PS is nullptr");
	m_pCPSs[hash] = pShader;
	return hash;
}

size_t EngineSystem::CreateComputeShader(std::wstring shaderName, std::string entryName, std::string target)
{
	size_t hash = Hasing_wstring(shaderName);
	if (m_pCCSs.find(hash) != m_pCCSs.end()) return hash;

	ComputeShader* pShader = new ComputeShader(m_pCDirect3D->GetDevice(), CompileShader(shaderName, entryName, target));
	_ASEERTION_NULCHK(pShader, "CS is nullptr");
	m_pCCSs[hash] = pShader;
	return hash;
}

ID3D11Resource* EngineSystem::CreateD3DBuffer(const ScratchImage* image)
{
	ID3D11Resource* pBuffer;
	HRESULT hResult;
	hResult = DirectX::CreateTexture(m_pCDirect3D->GetDevice(), image->GetImages(), image->GetImageCount(), image->GetMetadata(), &pBuffer);
	_ASEERTION_CREATE(hResult, "Texture not create successfully");
	return pBuffer;
}

ID3D11Resource* EngineSystem::CreateD3DBuffer(const std::wstring& szName, UINT stride, UINT count, void* data)
{
	size_t hash = CreateStructBuffer(szName, stride, count, data);
	return m_pCSTBs[hash]->GetBuffer();
}

ID3D11Texture2D* EngineSystem::CreateD3DBuffer(UINT bindFlags, UINT width, UINT height)
{
	HRESULT hResult;
	// 렌더 타겟 텍스처 설명을 초기화합니다.
	ID3D11Texture2D* pBuffer;
	D3D11_TEXTURE2D_DESC desc;
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.MiscFlags = 0;
	desc.BindFlags = 0;
	// 비트 연산을 통해 BindFlags 설정
	if (bindFlags & _RTV) desc.BindFlags |= D3D11_BIND_RENDER_TARGET;
	if (bindFlags & _SRV) desc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
	if (bindFlags & _DSV) desc.BindFlags |= D3D11_BIND_DEPTH_STENCIL;
	if (bindFlags & _UAV) desc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;

	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.CPUAccessFlags = 0;

	switch (bindFlags)
	{
		case _Target_Compute_Tex:
		case _Target_ResourceView:
		{
			desc.Format = DXGI_FORMAT_R16G16B16A16_TYPELESS;

		}break;

		case _Target_DepthView:
		{
			desc.Format = DXGI_FORMAT_R24G8_TYPELESS;
		}break;

		case _Target_Cubemap_ResourceView:
		{
			desc.MipLevels = 0;
			desc.ArraySize = 6;
			desc.Format = DXGI_FORMAT_R16G16B16A16_TYPELESS;
			desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
			desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS | D3D11_RESOURCE_MISC_TEXTURECUBE;
		}break;

		case _Target_Cubemap_DepthView:
		{
			desc.MipLevels = 0;
			desc.ArraySize = 6;
			desc.Format = DXGI_FORMAT_R24G8_TYPELESS;
			desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
			desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
		}break;
	}

	// 렌더 타겟 텍스처를 만듭니다.
	hResult = m_pCDirect3D->GetDevice()->CreateTexture2D(&desc, NULL, &pBuffer);
	_ASEERTION_CREATE(hResult, "CreateResource");
	return pBuffer;
}

size_t EngineSystem::CreateShaderResourceView(size_t hash, ID3D11Resource* pBuffer, UINT bindFlags)
{
	D3D11_SHADER_RESOURCE_VIEW_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	desc.Texture2D.MostDetailedMip = 0;
	desc.Texture2D.MipLevels = 1;
	switch (bindFlags)
	{
		case _TEX:
		{
			D3D11_TEXTURE2D_DESC texDesc;
			static_cast<ID3D11Texture2D*>(pBuffer)->GetDesc(&texDesc);
			desc.Format = texDesc.Format;
			desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		}break;

		case _Target_ResourceView:
		{
			desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
			desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		}break;

		case _Target_DepthView:
		{
			desc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
			desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		}break;

		case _Target_Compute_Buffer:
		{
			desc.Format = DXGI_FORMAT_UNKNOWN;
			desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;

			//버퍼의 원소 카운팅을 구성한다
			D3D11_BUFFER_DESC bufferDesc;
			ID3D11Buffer* pBuf = static_cast<ID3D11Buffer*>(pBuffer);
			pBuf->GetDesc(&bufferDesc);

			// 원소 개수 = 전체바이트 / Stride
			desc.BufferEx.FirstElement = 0;
			desc.BufferEx.NumElements = bufferDesc.ByteWidth / bufferDesc.StructureByteStride;
			desc.BufferEx.Flags = 0;
		}break;

		case _Target_Cubemap_ResourceView:
		{
			desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
			desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
			desc.Texture2D.MostDetailedMip = 0;
			desc.Texture2D.MipLevels = -1;
		}break;

		case _Target_Cubemap_DepthView:
		{
			desc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
			desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
			desc.Texture2D.MostDetailedMip = 0;
			desc.Texture2D.MipLevels = -1;
		}break;
	}

	ShaderResourceView* pSRV;
	if (m_pCSRVs.find(hash) != m_pCSRVs.end())
	{
		pSRV = m_pCSRVs[hash];
		pSRV->Resize(m_pCDirect3D->GetDevice(), pBuffer, desc);
	}
	else
	{
		pSRV = new ShaderResourceView(m_pCDirect3D->GetDevice(), pBuffer, desc);
		m_pCSRVs[hash] = pSRV;
	}
	return hash;
}

size_t EngineSystem::CreateRenderTargetView(size_t hash, ID3D11Resource* pBuffer, UINT bindFlags)
{
	D3D11_RENDER_TARGET_VIEW_DESC desc;
	desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	desc.Texture2DArray.ArraySize = 1;
	desc.Texture2D.MipSlice = 0;

	switch (bindFlags)
	{
		case _BB:
		{
			desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		}break;

		case _RTV:
		case _Target_ResourceView:
		{
			desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		}break;

		case _Target_Compute_Buffer:
		{
			desc.Format = DXGI_FORMAT_UNKNOWN;
		}break;

		case _Target_Cubemap_ResourceView:
		{
			desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
			desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
			desc.Texture2DArray.FirstArraySlice = 0;
			desc.Texture2DArray.ArraySize = 6;
			desc.Texture2DArray.MipSlice = 0;
		}break;
	}

	RenderTargetView* pRTV;
	if (m_pCRTVs.find(hash) != m_pCRTVs.end())
	{
		pRTV = m_pCRTVs[hash];
		pRTV->Resize(m_pCDirect3D->GetDevice(), pBuffer, desc);
	}
	else
	{
		pRTV = new RenderTargetView(m_pCDirect3D->GetDevice(), pBuffer, desc);
		m_pCRTVs[hash] = pRTV;
	}
	return hash;
}

size_t EngineSystem::CreateDepthStencilView(size_t hash, ID3D11Resource* pBuffer, UINT bindFlags)
{
	D3D11_DEPTH_STENCIL_VIEW_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;													//깊이맵은 D24_UNROM_S8_UINT가정석
	desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	desc.Texture2D.MipSlice = 0;

	switch (bindFlags)
	{
		case _DSV:
		case _Target_DepthView:
		{
			desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		}break;

		case _Target_Cubemap_DepthView:
		{
			desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
			desc.Texture2DArray.FirstArraySlice = 0;
			desc.Texture2DArray.ArraySize = 6;
			desc.Texture2DArray.MipSlice = 0;
		}break;
	}

	DepthStencilView* pDSV;
	if (m_pCDSVs.find(hash) != m_pCDSVs.end())
	{
		pDSV = m_pCDSVs[hash];
		pDSV->Resize(m_pCDirect3D->GetDevice(), pBuffer, desc);
	}
	else
	{
		pDSV = new DepthStencilView(m_pCDirect3D->GetDevice(), pBuffer, desc);
		m_pCDSVs[hash] = pDSV;
	}
	return hash;
}

size_t EngineSystem::CreateUnorderedAccessView(size_t hash, ID3D11Resource* pBuffer, UINT bindFlags)
{
	D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
	
	switch (bindFlags)
	{
		case _Target_Compute_Tex :
		{
			desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
			desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		}break;

		case _Target_Compute_Buffer :
		{
			desc.Format = DXGI_FORMAT_UNKNOWN;
			desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
			//버퍼의 원소 카운팅을 구성한다
			D3D11_BUFFER_DESC bufferDesc;
			ID3D11Buffer* pBuf = static_cast<ID3D11Buffer*>(pBuffer);
			pBuf->GetDesc(&bufferDesc);

			// 원소 개수 = 전체바이트 / Stride
			desc.Buffer.FirstElement = 0;
			desc.Buffer.NumElements = bufferDesc.ByteWidth / bufferDesc.StructureByteStride;;
			desc.Buffer.Flags = 0;
		}break;
	}

	UnorderedAccessView* pUAV;
	if (m_pCUAVs.find(hash) != m_pCUAVs.end())
	{
		pUAV = m_pCUAVs[hash];
		pUAV->Resize(m_pCDirect3D->GetDevice(), pBuffer, desc);
	}
	else
	{
		pUAV = new UnorderedAccessView(m_pCDirect3D->GetDevice(), pBuffer, desc);
		m_pCUAVs[hash] = pUAV;
	}
	return hash;
}

size_t EngineSystem::CreateViews(const std::wstring& szName, UINT bindFlags, UINT width, UINT height)
{
	size_t hash = Hasing_wstring(szName);
	ID3D11Resource* pBuffer = CreateD3DBuffer(bindFlags, width, height);

	if (bindFlags & _SRV)
		CreateShaderResourceView(hash, pBuffer, bindFlags);
	if (bindFlags & _RTV)
		CreateRenderTargetView(hash, pBuffer, bindFlags);
	if (bindFlags & _DSV)
		CreateDepthStencilView(hash, pBuffer, bindFlags);
	if (bindFlags & _UAV)
		CreateUnorderedAccessView(hash, pBuffer, bindFlags);

	pBuffer->Release();
	return hash;
}

size_t EngineSystem::CreateViews(const std::wstring& szName, UINT stride, UINT count, void* data)
{
	size_t hash = Hasing_wstring(szName);
	ID3D11Resource* pBuffer = CreateD3DBuffer(szName, stride, count, data);
	CreateShaderResourceView(hash, pBuffer, _Target_Compute_Buffer);
	CreateUnorderedAccessView(hash, pBuffer, _Target_Compute_Buffer);
	return hash;
}

void EngineSystem::GenerateMipMaps(size_t hashSRV)
{
	_ASEERTION_NULCHK(m_pCSRVs.find(hashSRV) != m_pCSRVs.end(), "NotExist");
	m_pCDirect3D->GetDeviceContext()->GenerateMips(m_pCSRVs[hashSRV]->GetView());
}

void EngineSystem::UpdateConstantBuffer(size_t hashCB, void* pData)
{
	auto pBuffer = _EngineSystem.GetCB(hashCB);
	_ASEERTION_NULCHK(pBuffer, "NotExist");
	pBuffer->UpdateBufferData(_EngineSystem.GetD3DDeviceContext(), pData);
}

void EngineSystem::CopyResource(ID3D11Resource* pSrc, ID3D11Resource* pDst)
{
	m_pCDirect3D->GetDeviceContext()->CopyResource(pDst, pSrc);
}

//Cpu에서 Gpu자원에 접근하기위한 Map/Unmap
void EngineSystem::MappedBuffer(ID3D11Resource* pResource, D3D11_MAPPED_SUBRESOURCE* mappedResource)
{
	HRESULT hResult = m_pCDirect3D->GetDeviceContext()->Map(pResource, 0, D3D11_MAP_READ, 0, mappedResource);
	_ASEERTION_CREATE(hResult, "Mapped Not Successfully");
}

void EngineSystem::UnMappedBuffer(ID3D11Resource* pResource)
{
	m_pCDirect3D->GetDeviceContext()->Unmap(pResource, 0);
}


