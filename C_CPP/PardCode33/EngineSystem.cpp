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

	for (auto iter = m_pCCBs.begin(); iter != m_pCCBs.end();)
	{
		delete iter->second;
		iter = m_pCCBs.erase(iter);
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
	return m_pCDirect3D->GetDevice();
}

ID3D11DeviceContext* EngineSystem::GetD3DDeviceContext() const
{
	return m_pCDirect3D->GetDeviceContext();
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

size_t EngineSystem::CreateRenderAsset(const std::wstring& szName, const std::vector<Mesh_Material>& hashs)
{
	RenderAsset* pRenderAsset = _ResourceSystem.CreateResource<RenderAsset>(szName);
	pRenderAsset->m_hMeshMats = hashs;
	return pRenderAsset->GetHash();
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
	pMesh->SetVB(CreateVertexBuffer(szName + szTypename + L"VB", pMesh->GetVertices(), sizeof(T), (UINT)pMesh->GetVerticesSize()));
	pMesh->SetIB(CreateIndexBuffer(szName + szTypename + L"IB", (UINT*)pMesh->GetIndicies().data(), (UINT)pMesh->GetIndicies().size()));
	return pMesh->GetHash();
}

template<typename T>
size_t EngineSystem::CreateMeshFromGeometry(size_t hash_geometry)
{
	Geometry* pGeometry = _ResourceSystem.GetResource<Geometry>(hash_geometry);
	std::wstring szPath = pGeometry->GetPath();
	std::wstring szTypename = _tomw(typeid(T).name());
	Mesh<T>* pMesh = _ResourceSystem.CreateResource<Mesh<T>>(szPath + szTypename + L"Mesh", pGeometry->GetVertices(), pGeometry->GetIndices(), pGeometry->GetPointsByMeshs(), pGeometry->GetPointsByBones());
	pMesh->SetVB(CreateVertexBuffer(szPath + szTypename + L"VB", pMesh->GetVertices(), sizeof(T), (UINT)pMesh->GetVerticesSize()));
	pMesh->SetIB(CreateIndexBuffer(szPath + szTypename + L"IB", (UINT*)pMesh->GetIndicies().data(), (UINT)pMesh->GetIndicies().size()));
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

void EngineSystem::Material_SetTextures(size_t hash_material, const std::vector<TX_HASH>& textures)
{
	Material* pMaterial = _ResourceSystem.GetResource<Material>(hash_material);
	for (const auto& iter : textures)
		pMaterial->SetTexture(iter);
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

size_t EngineSystem::CreateVertexBuffer(const std::wstring& szName, void* vertices, UINT size_vertex, UINT size_vertices)
{
	size_t hash = Hasing_wstring(szName);
	if (m_pCVBs.find(hash) != m_pCVBs.end()) return hash;
	VertexBuffer* pVertexBuffer = new VertexBuffer(m_pCDirect3D->GetDevice(), vertices, size_vertex, size_vertices);
	_ASEERTION_NULCHK(pVertexBuffer, "VB is nullptr");
	m_pCVBs[hash] = pVertexBuffer;
	return hash;
}

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

size_t EngineSystem::CreateIndexBuffer(const std::wstring& szName, void* indices, UINT size_indices)
{
	size_t hash = Hasing_wstring(szName);
	if (m_pCIBs.find(hash) != m_pCIBs.end()) return hash;
	IndexBuffer* pIndexBuffer = new IndexBuffer(m_pCDirect3D->GetDevice(), indices, size_indices);
	_ASEERTION_NULCHK(pIndexBuffer, "IB is nullptr");
	m_pCIBs[hash] = pIndexBuffer;
	return hash;
}

//void EngineSystem::CreateCubeMapTexture(int iSize)
//{
//	auto hashs = CreateCubeMapViews(iSize, iSize);
//	for (int i = 0; i < 6; i++)
//		m_hash_RTV_CubeMap[i] = hashs[i];
//	m_hash_SRV_CubeMap = hashs[6];
//	m_hash_DSV_CubeMap = hashs[7];
//}
//
//// 그림자매핑에 필요한 자원들을 구성한다
//size_t EngineSystem::CreateShadowMapTexture(const int width, const int height)
//{
//	size_t hash;
//	HRESULT hResult;
//
//	//그림자맵에 사용할 텍스쳐자원을 생성한다
//	ID3D11Texture2D* pBuffer;
//	D3D11_TEXTURE2D_DESC tex_desc = GetTex2DDesc(E_ResourcesUsage::DSV, width, height);
//	hResult = m_pCDirect3D->GetDevice()->CreateTexture2D(&tex_desc, nullptr, &pBuffer);
//	_ASEERTION_CREATE(hResult, "ShadowMapTex");
//
//	//렌더링에 사용할 DSV자원을 생성한다
//	hash = Hasing_wstring(L"ShadowMap");
//
//	D3D11_DEPTH_STENCIL_VIEW_DESC desc_dsv;
//	ZeroMemory(&desc_dsv, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
//	desc_dsv.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
//	desc_dsv.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
//	desc_dsv.Texture2D.MipSlice = 0;
//	DepthStencilView* pDSV;
//	pDSV = new DepthStencilView(m_pCDirect3D->GetDevice(), pBuffer, desc_dsv);
//	m_pCDSVs[hash] = pDSV;
//
//	//렌더링에 사용할 SRV자원을 생성한다
//	D3D11_SHADER_RESOURCE_VIEW_DESC desc_srv;
//	desc_srv.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
//	desc_srv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
//	desc_srv.Texture2D.MostDetailedMip = 0;
//	desc_srv.Texture2D.MipLevels = 1;
//
//	ShaderResourceView* pSRV;
//	pSRV = new ShaderResourceView(m_pCDirect3D->GetDevice(), pBuffer, desc_srv);
//	m_pCSRVs[hash] = pSRV;
//
//	pBuffer->Release();//사용을 끝낸 refCount감소
//	return hash;
//}
//
//size_t EngineSystem::CreateShaderResourceView(const std::wstring& szName, const ScratchImage* resource)
//{
//	size_t hash = Hasing_wstring(szName);
//	if (m_pCSRVs.find(hash) != m_pCSRVs.end()) return hash;
//	_ASEERTION_NULCHK(resource, "scratchImage is nullptr");
//	ShaderResourceView* pSRV = new ShaderResourceView(m_pCDirect3D->GetDevice(), resource);
//	_ASEERTION_NULCHK(pSRV, "VW is nullptr");
//	m_pCSRVs[hash] = pSRV;
//	return hash;
//}
//
////백버퍼 렌더타겟뷰
//size_t EngineSystem::CreateRenderTargetView(const std::wstring& szName)
//{
//	size_t hash = Hasing_wstring(szName);
//	RenderTargetView* pRTV;
//	if (m_pCRTVs.find(hash) != m_pCRTVs.end())
//	{
//		pRTV = m_pCRTVs[hash];
//		pRTV->Resize(m_pCDirect3D->GetDevice(), m_pCSwapChain->GetSwapChain());
//	}
//	else
//	{
//		pRTV = new RenderTargetView(m_pCDirect3D->GetDevice(), m_pCSwapChain->GetSwapChain());
//		m_pCRTVs[hash] = pRTV;
//	}
//	return hash;
//}
//
////사이즈지정 렌더타겟뷰
//size_t EngineSystem::CreateRenderTargetView(const std::wstring& szName, UINT width, UINT height)
//{
//	size_t hash = Hasing_wstring(szName);
//	HRESULT hResult;
//	// 렌더 타겟 텍스처 설명을 초기화합니다.
//	ID3D11Texture2D* pBuffer;
//	D3D11_TEXTURE2D_DESC desc_tex = GetTex2DDesc(E_ResourcesUsage::RTV, width, height);
//
//	// 렌더 타겟 텍스처를 만듭니다.
//	hResult = m_pCDirect3D->GetDevice()->CreateTexture2D(&desc_tex, NULL, &pBuffer);
//	_ASEERTION_CREATE(hResult, "Buffer");
//
//	D3D11_RENDER_TARGET_VIEW_DESC desc_rtv;
//	desc_rtv.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
//	desc_rtv.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
//	desc_rtv.Texture2D.MipSlice = 0;
//
//	RenderTargetView* pRTV;
//	if (m_pCRTVs.find(hash) != m_pCRTVs.end())
//	{
//		pRTV = m_pCRTVs[hash];
//		pRTV->Resize(m_pCDirect3D->GetDevice(), pBuffer, desc_rtv);
//	}
//	else
//	{
//		pRTV = new RenderTargetView(m_pCDirect3D->GetDevice(), pBuffer, desc_rtv);
//		m_pCRTVs[hash] = pRTV;
//	}
//
//	D3D11_SHADER_RESOURCE_VIEW_DESC desc_srv;
//	desc_srv.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
//	desc_srv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
//	desc_srv.Texture2D.MostDetailedMip = 0;
//	desc_srv.Texture2D.MipLevels = 1;
//	ShaderResourceView* pSRV;
//	if (m_pCSRVs.find(hash) != m_pCSRVs.end())
//	{
//		pSRV = m_pCSRVs[hash];
//		pSRV->Resize(m_pCDirect3D->GetDevice(), pBuffer, desc_srv);
//	}
//	else
//	{
//		pSRV = new ShaderResourceView(m_pCDirect3D->GetDevice(), pBuffer, desc_srv);
//		m_pCSRVs[hash] = pSRV;
//	}
//
//	pBuffer->Release();	//사용을 끝낸 refCount감소
//	return hash;
//}
//
//size_t EngineSystem::CreateDepthStencilView(const std::wstring& szName, UINT width, UINT height)
//{
//	size_t hash = Hasing_wstring(szName);
//	//뎁스스텐실뷰에 사용할 텍스쳐버퍼 디스크립션
//	HRESULT hResult;
//	ID3D11Texture2D* pBuffer;
//	D3D11_TEXTURE2D_DESC tex_desc = GetTex2DDesc(E_ResourcesUsage::DSV, width, height);
//
//	hResult = m_pCDirect3D->GetDevice()->CreateTexture2D(&tex_desc, nullptr, &pBuffer);
//	_ASEERTION_CREATE(hResult, "DepthBuffer");
//
//	D3D11_DEPTH_STENCIL_VIEW_DESC desc_dsv;
//	ZeroMemory(&desc_dsv, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
//	desc_dsv.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;													//깊이맵은 D24_UNROM_S8_UINT가정석
//	desc_dsv.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
//	desc_dsv.Texture2D.MipSlice = 0;
//	DepthStencilView* pDSV;
//	if (m_pCDSVs.find(hash) != m_pCDSVs.end())
//	{
//		pDSV = m_pCDSVs[hash];
//		pDSV->Resize(m_pCDirect3D->GetDevice(), pBuffer, desc_dsv);
//	}
//	else
//	{
//		pDSV = new DepthStencilView(m_pCDirect3D->GetDevice(), pBuffer, desc_dsv);
//		m_pCDSVs[hash] = pDSV;
//	}
//
//	D3D11_SHADER_RESOURCE_VIEW_DESC desc_srv;
//	desc_srv.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
//	desc_srv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
//	desc_srv.Texture2D.MostDetailedMip = 0;
//	desc_srv.Texture2D.MipLevels = 1;
//	ShaderResourceView* pSRV;
//	if (m_pCSRVs.find(hash) != m_pCSRVs.end())
//	{
//		pSRV = m_pCSRVs[hash];
//		pSRV->Resize(m_pCDirect3D->GetDevice(), pBuffer, desc_srv);
//	}
//	else
//	{
//		pSRV = new ShaderResourceView(m_pCDirect3D->GetDevice(), pBuffer, desc_srv);
//		m_pCSRVs[hash] = pSRV;
//	}
//
//	pBuffer->Release();	//사용을 끝낸 refCount감소
//	return hash;
//}
//
//size_t EngineSystem::CreateUnorderedAccessView(const std::wstring& szName)
//{
//	size_t hash = Hasing_wstring(szName);
//	//UAV에 사용할 설정들
//	HRESULT hResult;
//	ID3D11Buffer* pBuffer;
//
//	D3D11_UNORDERED_ACCESS_VIEW_DESC desc_uav;
//	ZeroMemory(&desc_uav, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
//	desc_uav.Format = DXGI_FORMAT_UNKNOWN;
//	desc_uav.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
//	UnorderedAccessView* pUAV;
//	if (m_pCUAVs.find(hash) != m_pCUAVs.end())
//	{
//		pUAV = m_pCUAVs[hash];
//		pUAV->Resize(m_pCDirect3D->GetDevice(), pBuffer, desc_uav);
//	}
//	else
//	{
//		pUAV = new UnorderedAccessView(m_pCDirect3D->GetDevice(), pBuffer, desc_uav);
//		m_pCUAVs[hash] = pUAV;
//	}
//
//	D3D11_SHADER_RESOURCE_VIEW_DESC desc_srv;
//	desc_srv.Format = DXGI_FORMAT_UNKNOWN;
//	desc_srv.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
//	desc_srv.Texture2D.MipLevels = 1;
//	ShaderResourceView* pSRV;
//	if (m_pCSRVs.find(hash) != m_pCSRVs.end())
//	{
//		pSRV = m_pCSRVs[hash];
//		pSRV->Resize(m_pCDirect3D->GetDevice(), pBuffer, desc_srv);
//	}
//	else
//	{
//		pSRV = new ShaderResourceView(m_pCDirect3D->GetDevice(), pBuffer, desc_srv);
//		m_pCSRVs[hash] = pSRV;
//	}
//	return hash;
//}
//
//
//std::vector<size_t>  EngineSystem::CreateCubeMapViews(const int width, const int height)
//{
//	// 렌더타겟은 백버퍼와다르게해상도가 다르므로 뷰포트를 해당크기에맞게끔지정해 드로우콜에이용해야한다
//
//	//6(RTV), 1(SRV), 1(DSV)
//	std::vector<size_t> hashs;
//	size_t hash;
//	HRESULT hResult;
//
//	//큐브맵에 사용할 텍스쳐자원을 생성한다
//	ID3D11Texture2D* pBuffer;
//	D3D11_TEXTURE2D_DESC tex_desc = GetTex2DDesc(E_ResourcesUsage::RTV_CubeMap, width, height);
//	hResult = m_pCDirect3D->GetDevice()->CreateTexture2D(&tex_desc, nullptr, &pBuffer);
//	_ASEERTION_CREATE(hResult, "CubeMapTex");
//
//	//RTV를 생성한다(6면)
//	D3D11_RENDER_TARGET_VIEW_DESC desc_rtv;
//	desc_rtv.Format = tex_desc.Format;
//	desc_rtv.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
//	desc_rtv.Texture2DArray.ArraySize = 1;
//	desc_rtv.Texture2DArray.MipSlice = 0;
//
//	for (int i = 0; i < 6; i++)
//	{
//		desc_rtv.Texture2DArray.FirstArraySlice = i;
//		hash = Hasing_wstring(L"CubeMapRTV" + std::to_wstring(i));
//		RenderTargetView* pRTV;
//		pRTV = new RenderTargetView(m_pCDirect3D->GetDevice(), pBuffer, desc_rtv);
//		m_pCRTVs[hash] = pRTV;
//		hashs.push_back(hash);
//	}
//
//	//렌더링에 사용할 SRV자원을 생성한다
//	D3D11_SHADER_RESOURCE_VIEW_DESC desc_srv;
//	desc_srv.Format = tex_desc.Format;
//	desc_srv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
//	desc_srv.TextureCube.MostDetailedMip = 0;
//	desc_srv.TextureCube.MipLevels = -1;
//
//	hash = Hasing_wstring(L"CubeMapSRV");
//	ShaderResourceView* pSRV;
//	pSRV = new ShaderResourceView(m_pCDirect3D->GetDevice(), pBuffer, desc_srv);
//	m_pCSRVs[hash] = pSRV;
//	hashs.push_back(hash);
//
//	pBuffer->Release();	//사용을 끝낸 refCount감소
//
//	//큐브맵에 사용할 텍스쳐자원을 생성한다(DSV)
//	tex_desc = GetTex2DDesc(E_ResourcesUsage::DSV, width, height);
//	hResult = m_pCDirect3D->GetDevice()->CreateTexture2D(&tex_desc, nullptr, &pBuffer);
//	_ASEERTION_CREATE(hResult, "CubeMapTex");
//
//	//렌더링에 사용할 DSV자원을 생성한다
//	hash = Hasing_wstring(L"CubeMapDSV");
//	D3D11_DEPTH_STENCIL_VIEW_DESC desc_dsv;
//	ZeroMemory(&desc_dsv, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
//	desc_dsv.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;													//깊이맵은 D24_UNROM_S8_UINT가정석
//	desc_dsv.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
//	desc_dsv.Texture2D.MipSlice = 0;
//	DepthStencilView* pDSV;
//	pDSV = new DepthStencilView(m_pCDirect3D->GetDevice(), pBuffer, desc_dsv);
//	m_pCDSVs[hash] = pDSV;
//	hashs.push_back(hash);
//
//	pBuffer->Release();//사용을 끝낸 refCount감소
//
//	return hashs;
//}
//
////기하셰이더를 이용한 입방체맵의 생성
//std::vector<size_t> EngineSystem::CreateCubeMapView(const int width, const int height)
//{
//	// 렌더타겟은 백버퍼와다르게해상도가 다르므로 뷰포트를 해당크기에맞게끔지정해 드로우콜에이용해야한다
//	//1(RTV), 1(SRV), 1(DSV)
//	std::vector<size_t> hashs;
//	size_t hash;
//	HRESULT hResult;
//
//	//큐브맵에 사용할 텍스쳐자원을 생성한다
//	ID3D11Texture2D* pBuffer;
//	D3D11_TEXTURE2D_DESC tex_desc = GetTex2DDesc(E_ResourcesUsage::RTV_CubeMap, width, height);
//	hResult = m_pCDirect3D->GetDevice()->CreateTexture2D(&tex_desc, nullptr, &pBuffer);
//	_ASEERTION_CREATE(hResult, "CubeMapRTV,SRV Tex");
//
//	//RTV를 생성한다(6면)
//	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
//	rtvDesc.Format = tex_desc.Format;
//	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
//	rtvDesc.Texture2DArray.FirstArraySlice = 0;
//	rtvDesc.Texture2DArray.ArraySize = 6;
//	rtvDesc.Texture2DArray.MipSlice = 0;
//	hash = Hasing_wstring(L"CubeMapRTV");
//	RenderTargetView* pRTV;
//	pRTV = new RenderTargetView(m_pCDirect3D->GetDevice(), pBuffer, rtvDesc);
//	m_pCRTVs[hash] = pRTV;
//	hashs.push_back(hash);
//
//	//렌더링에 사용할 SRV자원을 생성한다
//	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
//	srvDesc.Format = tex_desc.Format;
//	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
//	srvDesc.TextureCube.MostDetailedMip = 0;
//	srvDesc.TextureCube.MipLevels = -1;
//
//	hash = Hasing_wstring(L"CubeMapSRV");
//	ShaderResourceView* pSRV;
//	pSRV = new ShaderResourceView(m_pCDirect3D->GetDevice(), pBuffer, srvDesc);
//	m_pCSRVs[hash] = pSRV;
//	hashs.push_back(hash);
//
//	pBuffer->Release();	//사용을 끝낸 refCount감소
//
//	//큐브맵에 사용할 텍스쳐자원을 생성한다(DSV)
//	tex_desc = GetTex2DDesc(E_ResourcesUsage::DSV_CubeMap, width, height);
//	hResult = m_pCDirect3D->GetDevice()->CreateTexture2D(&tex_desc, nullptr, &pBuffer);
//	_ASEERTION_CREATE(hResult, "CubeMapDSV Tex");
//
//	//렌더링에 사용할 DSV자원을 생성한다
//	hash = Hasing_wstring(L"CubeMapDSV");
//	D3D11_DEPTH_STENCIL_VIEW_DESC desc_dsv;
//	ZeroMemory(&desc_dsv, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
//	desc_dsv.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;													//깊이맵은 D24_UNROM_S8_UINT가정석
//	desc_dsv.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
//	desc_dsv.Texture2DArray.ArraySize = 6;
//	desc_dsv.Texture2D.MipSlice = 0;
//	DepthStencilView* pDSV;
//	pDSV = new DepthStencilView(m_pCDirect3D->GetDevice(), pBuffer, desc_dsv);
//	m_pCDSVs[hash] = pDSV;
//	hashs.push_back(hash);
//
//	pBuffer->Release();//사용을 끝낸 refCount감소
//
//	return hashs;
//}

void EngineSystem::OnResize(UINT width, UINT height)
{
	if (!m_pCDirect3D) return;
	if (width == 0 || height == 0) return;
	g_iWidth = width;
	g_iHeight = height;

	//refCount를 전부 해제시킨다, SwapChain의 ResizeBuffer시 기존버퍼에대한 모든 RefCount가 초기화되어 Comptr객체에서 해제된다
	if (m_hash_RTV_BB && m_pCRTVs.find(m_hash_RTV_BB) != m_pCRTVs.end()) m_pCRTVs[m_hash_RTV_BB]->GetView()->Release();
	if (m_hash_RTV_0 && m_pCRTVs.find(m_hash_RTV_0) != m_pCRTVs.end()) m_pCRTVs[m_hash_RTV_0]->GetView()->Release();
	if (m_hash_RTV_0 && m_pCSRVs.find(m_hash_RTV_0) != m_pCSRVs.end()) m_pCSRVs[m_hash_RTV_0]->GetView()->Release();
	if (m_hash_DSV_0 && m_pCDSVs.find(m_hash_DSV_0) != m_pCDSVs.end()) m_pCDSVs[m_hash_DSV_0]->GetView()->Release();
	if (m_hash_DSV_0 && m_pCSRVs.find(m_hash_DSV_0) != m_pCSRVs.end()) m_pCSRVs[m_hash_DSV_0]->GetView()->Release();

	m_pCSwapChain->GetSwapChain()->ResizeBuffers(1, g_iWidth, g_iHeight, DXGI_FORMAT_UNKNOWN, 0);
	//백버퍼
	HRESULT hResult;
	ID3D11Texture2D* pBuffer;
	hResult = m_pCSwapChain->GetSwapChain()->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBuffer);
	_ASEERTION_CREATE(hResult, "BackBuffer");
	m_hash_RTV_BB = CreateRenderTargetView(Hasing_wstring(L"BackBufferRTV"), pBuffer, _BB);
	pBuffer->Release();

	//RTT
	m_hash_RTV_0 = CreateViews(L"RTV0", _Target_ResourceView, g_iWidth, g_iHeight);
	m_hash_DSV_0 = CreateViews(L"DSV0", _Target_ShadowMap, g_iWidth, g_iHeight);
	SetViewportSize(&m_vp_BB, g_iWidth, g_iHeight);
}

void EngineSystem::SetViewportSize(D3D11_VIEWPORT* pViewport, UINT iWidth, UINT iHeight)
{
	pViewport->Width = iWidth;
	pViewport->Height = iHeight;
}

void EngineSystem::ClearRenderTargetView(size_t hashRTV, float r, float g, float b, float a)
{
	_ASEERTION_NULCHK(m_pCRTVs.find(hashRTV) != m_pCRTVs.end(), "NotExist");
	FLOAT clearColor[] = { r, g, b, a };
	m_pCDirect3D->GetDeviceContext()->ClearRenderTargetView(m_pCRTVs[hashRTV]->GetView(), clearColor);
}

void EngineSystem::ClearDepthStencilView(size_t hashDSV)
{
	_ASEERTION_NULCHK(m_pCDSVs.find(hashDSV) != m_pCDSVs.end(), "NotExist");
	m_pCDirect3D->GetDeviceContext()->ClearDepthStencilView(m_pCDSVs[hashDSV]->GetView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void EngineSystem::GenerateMipMaps(size_t hashSRV)
{
	_ASEERTION_NULCHK(m_pCSRVs.find(hashSRV) != m_pCSRVs.end(), "NotExist");
	m_pCDirect3D->GetDeviceContext()->GenerateMips(m_pCSRVs[hashSRV]->GetView());
}

void EngineSystem::UpdateConstantBuffer(size_t hashCB, void* pData)
{
	_ASEERTION_NULCHK(m_pCCBs.find(hashCB) != m_pCCBs.end(), "NotExist");
	m_pCCBs[hashCB]->UpdateBufferData(m_pCDirect3D->GetDeviceContext(), pData);
}

void EngineSystem::SetIA_Topology(D3D_PRIMITIVE_TOPOLOGY topology)
{
	m_pCDirect3D->GetDeviceContext()->IASetPrimitiveTopology(topology);
}

void EngineSystem::SetIA_InputLayout(size_t hashIL)
{
	auto ptr = m_pCILs.find(hashIL) != m_pCILs.end() ? m_pCILs[hashIL]->GetInputLayout() : nullptr;
	m_pCDirect3D->GetDeviceContext()->IASetInputLayout(ptr);
}

void EngineSystem::SetIA_VertexBuffer(size_t hashVB, UINT offset)
{
	auto ptr = m_pCVBs.find(hashVB) != m_pCVBs.end() ? m_pCVBs[hashVB]->GetBuffer() : nullptr;
	UINT stride = m_pCVBs.find(hashVB) != m_pCVBs.end() ? m_pCVBs[hashVB]->GetVertexSize() : 0;
	m_pCDirect3D->GetDeviceContext()->IASetVertexBuffers(0, 1, &ptr, &stride, &offset);
}

void EngineSystem::SetIA_IndexBuffer(size_t hashIB, UINT offset)
{
	auto ptr = m_pCIBs.find(hashIB) != m_pCIBs.end() ? m_pCIBs[hashIB]->GetBuffer() : nullptr;
	m_pCDirect3D->GetDeviceContext()->IASetIndexBuffer(ptr, DXGI_FORMAT_R32_UINT, offset); //4바이트, 32비트를의미
}

void EngineSystem::SetVS_Shader(size_t hashVS)
{
	auto ptr = m_pCVSs.find(hashVS) != m_pCVSs.end() ? m_pCVSs[hashVS]->GetShader() : nullptr;
	m_pCDirect3D->GetDeviceContext()->VSSetShader(ptr, nullptr, 0);
}

void EngineSystem::SetVS_ShaderResourceView(size_t hashSRV, UINT startIdx)
{
	auto ptr = m_pCSRVs.find(hashSRV) != m_pCSRVs.end() ? m_pCSRVs[hashSRV]->GetView() : nullptr;
	int isize = m_pCSRVs.find(hashSRV) != m_pCSRVs.end() ? 1 : 0;
	m_pCDirect3D->GetDeviceContext()->VSSetShaderResources(startIdx, isize, &ptr);
}

void EngineSystem::SetVS_ConstantBuffer(size_t hashCB, UINT startIdx)
{
	auto ptr = m_pCCBs.find(hashCB) != m_pCCBs.end() ? m_pCCBs[hashCB]->GetBuffer() : nullptr;
	int isize = m_pCCBs.find(hashCB) != m_pCCBs.end() ? 1 : 0;
	m_pCDirect3D->GetDeviceContext()->VSSetConstantBuffers(startIdx, isize, &ptr);
}

void EngineSystem::SetVS_SamplerState(E_Sampler eSampler, UINT startIdx)
{
	_ASEERTION_NULCHK(m_pCSamplers->GetState(eSampler), "NotExist");
	auto pSampler = m_pCSamplers->GetState(eSampler);
	m_pCDirect3D->GetDeviceContext()->VSSetSamplers(startIdx, 1, &pSampler);
}

void EngineSystem::SetHS_Shader(size_t hashHS)
{
	auto ptr = m_pCHSs.find(hashHS) != m_pCHSs.end() ? m_pCHSs[hashHS]->GetShader() : nullptr;
	m_pCDirect3D->GetDeviceContext()->HSSetShader(ptr, nullptr, 0);
}

void EngineSystem::SetHS_ConstantBuffer(size_t hashCB, UINT startIdx)
{
	auto ptr = m_pCCBs.find(hashCB) != m_pCCBs.end() ? m_pCCBs[hashCB]->GetBuffer() : nullptr;
	int isize = m_pCCBs.find(hashCB) != m_pCCBs.end() ? 1 : 0;
	m_pCDirect3D->GetDeviceContext()->HSSetConstantBuffers(startIdx, isize, &ptr);
}

void EngineSystem::SetDS_Shader(size_t hashDS)
{
	auto ptr = m_pCDSs.find(hashDS) != m_pCDSs.end() ? m_pCDSs[hashDS]->GetShader() : nullptr;
	m_pCDirect3D->GetDeviceContext()->DSSetShader(ptr, nullptr, 0);
}

void EngineSystem::SetDS_ConstantBuffer(size_t hashCB, UINT startIdx)
{
	auto ptr = m_pCCBs.find(hashCB) != m_pCCBs.end() ? m_pCCBs[hashCB]->GetBuffer() : nullptr;
	int isize = m_pCCBs.find(hashCB) != m_pCCBs.end() ? 1 : 0;
	m_pCDirect3D->GetDeviceContext()->DSSetConstantBuffers(startIdx, isize, &ptr);
}

void EngineSystem::SetGS_Shader(size_t hashGS)
{
	auto ptr = m_pCGSs.find(hashGS) != m_pCGSs.end() ? m_pCGSs[hashGS]->GetShader() : nullptr;
	m_pCDirect3D->GetDeviceContext()->GSSetShader(ptr, nullptr, 0);
}

void EngineSystem::SetGS_ConstantBuffer(size_t hashCB, UINT startIdx)
{
	auto ptr = m_pCCBs.find(hashCB) != m_pCCBs.end() ? m_pCCBs[hashCB]->GetBuffer() : nullptr;
	int isize = m_pCCBs.find(hashCB) != m_pCCBs.end() ? 1 : 0;
	m_pCDirect3D->GetDeviceContext()->GSSetConstantBuffers(startIdx, isize, &ptr);
}

void EngineSystem::SetPS_SamplerState(E_Sampler eSampler, UINT startIdx)
{
	_ASEERTION_NULCHK(m_pCSamplers->GetState(eSampler), "NotExist");
	auto pSampler = m_pCSamplers->GetState(eSampler);
	m_pCDirect3D->GetDeviceContext()->PSSetSamplers(startIdx, 1, &pSampler);
}

void EngineSystem::SetPS_Shader(size_t hashPS)
{
	auto ptr = m_pCPSs.find(hashPS) != m_pCPSs.end() ? m_pCPSs[hashPS]->GetShader() : nullptr;
	m_pCDirect3D->GetDeviceContext()->PSSetShader(ptr, nullptr, 0);
}

void EngineSystem::SetPS_ShaderResourceView(size_t hashSRV, UINT startIdx)
{
	auto ptr = m_pCSRVs.find(hashSRV) != m_pCSRVs.end() ? m_pCSRVs[hashSRV]->GetView() : nullptr;
	int isize = m_pCSRVs.find(hashSRV) != m_pCSRVs.end() ? 1 : 0;
	m_pCDirect3D->GetDeviceContext()->PSSetShaderResources(startIdx, isize, &ptr);
}

void EngineSystem::SetPS_ConstantBuffer(size_t hashCB, UINT startIdx)
{
	auto ptr = m_pCCBs.find(hashCB) != m_pCCBs.end() ? m_pCCBs[hashCB]->GetBuffer() : nullptr;
	int isize = m_pCCBs.find(hashCB) != m_pCCBs.end() ? 1 : 0;
	m_pCDirect3D->GetDeviceContext()->PSSetConstantBuffers(startIdx, isize, &ptr);
}

void EngineSystem::SetRS_RasterizerState(E_RSState eRSState)
{
	_ASEERTION_NULCHK(m_pCRasterizers->GetState(eRSState), "NotExist");
	auto pState = m_pCRasterizers->GetState(eRSState);
	m_pCDirect3D->GetDeviceContext()->RSSetState(pState);
}

void EngineSystem::SetRS_Viewport(D3D11_VIEWPORT* pViewport)
{
	m_pCDirect3D->GetDeviceContext()->RSSetViewports(1, pViewport);
}

void EngineSystem::SetOM_RenderTargets(std::vector<size_t> hashRtvs, size_t hashDSV)
{
	std::vector<ID3D11RenderTargetView*> rtvs;
	for (UINT i = 0; i < hashRtvs.size(); i++)
	{
		_ASEERTION_NULCHK(m_pCRTVs.find(hashRtvs[i]) != m_pCRTVs.end(), "NotExist");
		rtvs.push_back(m_pCRTVs[hashRtvs[i]]->GetView());
	}
	auto pdsv = m_pCDSVs.find(hashDSV) != m_pCDSVs.end() ? m_pCDSVs[hashDSV]->GetView() : nullptr;
	m_pCDirect3D->GetDeviceContext()->OMSetRenderTargets(rtvs.size(), rtvs.empty() ? nullptr : rtvs.data(), pdsv);
}

void EngineSystem::SetOM_DepthStenilState(E_DSState eDSState, UINT stencilRef)
{
	_ASEERTION_NULCHK(m_pCDepthStencils->GetState(eDSState), "NotExist");
	auto pState = m_pCDepthStencils->GetState(eDSState);
	m_pCDirect3D->GetDeviceContext()->OMSetDepthStencilState(pState, stencilRef);
}

void EngineSystem::SetOM_BlendState(E_BSState eBSState, const FLOAT* blendFactor, UINT sampleMask)
{
	_ASEERTION_NULCHK(m_pCBlends->GetState(eBSState), "NotExist");
	auto pState = m_pCBlends->GetState(eBSState);
	m_pCDirect3D->GetDeviceContext()->OMSetBlendState(pState, blendFactor, sampleMask);
}

void EngineSystem::Draw_Vertices(UINT vertexCount, UINT startIdx)
{
	m_pCDirect3D->GetDeviceContext()->Draw(vertexCount, startIdx);
}

void EngineSystem::Draw_Indicies(UINT indexCount, UINT startIdx, INT vertexOffset)
{
	m_pCDirect3D->GetDeviceContext()->DrawIndexed(indexCount, startIdx, vertexOffset);
}

void EngineSystem::SwapchainPresent(bool vsync)
{
	m_pCSwapChain->GetSwapChain()->Present(vsync, NULL);
}

size_t EngineSystem::CreateConstantBuffer(const type_info& typeinfo, UINT size_buffer, void* data)
{
	size_t hash = typeinfo.hash_code();
	if (m_pCCBs.find(hash) != m_pCCBs.end()) return hash;
	ConstantBuffer* pConstantBuffer = new ConstantBuffer(m_pCDirect3D->GetDevice(), data, size_buffer);
	_ASEERTION_NULCHK(pConstantBuffer, "CB is nullptr");
	m_pCCBs[hash] = pConstantBuffer;
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
	desc.Format = DXGI_FORMAT_B8G8R8A8_TYPELESS;
	desc.CPUAccessFlags = 0;
	
	switch (bindFlags)
	{
		case _Target_ResourceView :
		{
			desc.Format = DXGI_FORMAT_R16G16B16A16_TYPELESS;
		}break;

		case _Target_ShadowMap :
		{
			desc.Format = DXGI_FORMAT_R24G8_TYPELESS;
		}break;

		case _Target_Compute:
		{
			desc.Format = DXGI_FORMAT_UNKNOWN;
		}break;

		case _Target_Cubemap:
		{
			desc.MipLevels = 0;
			desc.ArraySize = 6;
			desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
			desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS | D3D11_RESOURCE_MISC_TEXTURECUBE;
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

		case _Target_ShadowMap:
		{
			desc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
			desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		}break;

		case _Target_Compute:
		{
			desc.Format = DXGI_FORMAT_UNKNOWN;
			desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
		}break;

		case _Target_Cubemap:
		{
			desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
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
		case _BB :
		{
			desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		}break;

		case _RTV:
		case _Target_ResourceView:
		{
			desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		}break;

		case _Target_Compute:
		{
			desc.Format = DXGI_FORMAT_UNKNOWN;
		}break;

		case _Target_Cubemap:
		{
			desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
			desc.Texture2DArray.ArraySize = 6;
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
		case _Target_ShadowMap:
		{
			desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		}break;

		case _Target_Cubemap:
		{
			desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
			desc.Texture2DArray.ArraySize = 6;
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
	{

	}

	pBuffer->Release();
	return hash;
}

size_t EngineSystem::CreateVertexShader(std::wstring shaderName, std::string entryName, std::string target)
{
	size_t hash = Hasing_wstring(shaderName);
	if (m_pCVSs.find(hash) != m_pCVSs.end()) return hash;

	VertexShader* pVertexShader = new VertexShader(m_pCDirect3D->GetDevice(), CompileShader(shaderName, entryName, target));
	_ASEERTION_NULCHK(pVertexShader, "VS is nullptr");
	m_pCVSs[hash] = pVertexShader;
	return hash;
}

size_t EngineSystem::CreateHullShader(std::wstring shaderName, std::string entryName, std::string target)
{
	size_t hash = Hasing_wstring(shaderName);
	if (m_pCHSs.find(hash) != m_pCHSs.end()) return hash;

	HullShader* pHullShader = new HullShader(m_pCDirect3D->GetDevice(), CompileShader(shaderName, entryName, target));
	_ASEERTION_NULCHK(pHullShader, "GS is nullptr");
	m_pCHSs[hash] = pHullShader;
	return hash;
}

size_t EngineSystem::CreateDomainShader(std::wstring shaderName, std::string entryName, std::string target)
{
	size_t hash = Hasing_wstring(shaderName);
	if (m_pCDSs.find(hash) != m_pCDSs.end()) return hash;

	DomainShader* pDomainShader = new DomainShader(m_pCDirect3D->GetDevice(), CompileShader(shaderName, entryName, target));
	_ASEERTION_NULCHK(pDomainShader, "GS is nullptr");
	m_pCDSs[hash] = pDomainShader;
	return hash;
}

size_t EngineSystem::CreateGeometryShader(std::wstring shaderName, std::string entryName, std::string target)
{
	size_t hash = Hasing_wstring(shaderName);
	if (m_pCGSs.find(hash) != m_pCGSs.end()) return hash;

	GeometryShader* pGeometryShader = new GeometryShader(m_pCDirect3D->GetDevice(), CompileShader(shaderName, entryName, target));
	_ASEERTION_NULCHK(pGeometryShader, "GS is nullptr");
	m_pCGSs[hash] = pGeometryShader;
	return hash;
}

size_t EngineSystem::CreatePixelShader(std::wstring shaderName, std::string entryName, std::string target)
{
	size_t hash = Hasing_wstring(shaderName);
	if (m_pCPSs.find(hash) != m_pCPSs.end()) return hash;

	PixelShader* pPixelShader = new PixelShader(m_pCDirect3D->GetDevice(), CompileShader(shaderName, entryName, target));
	_ASEERTION_NULCHK(pPixelShader, "PS is nullptr");
	m_pCPSs[hash] = pPixelShader;
	return hash;
}

