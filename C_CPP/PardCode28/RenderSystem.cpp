#include "RenderSystem.h"
#include "Direct3D.h"
#include "SwapChain.h"
#include "VertexBuffer.h"
#include "InputLayout.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "ConstantBuffer.h"
#include "IndexBuffer.h"
#include "TempObj.h"
#include "CameraSystem.h"
#include "FirstPersonCamera.h"
#include "Texture2D.h"
#include "ResourceSystem.h"
#include "Texture.h"
#include "SamplerState.h"
#include "RasterizerState.h"
#include "Mesh.h"
#include "LightSystem.h"
#include "Light.h"
#include "CollisionSystem.h"
#include "Material.h"

#include "TestBlockMacro.h"

size_t g_hash_cbdirectionalLight = typeid(CB_DirectionalLight).hash_code();
size_t g_hash_cbpointlight = typeid(CB_PointLight).hash_code();
size_t g_hash_cbspotlight = typeid(CB_SpotLight).hash_code();
size_t g_hash_cbwvpitmat = typeid(CB_WVPITMatrix).hash_code();
size_t g_hash_cbtime = typeid(CB_Time).hash_code();
size_t g_hash_cbcampos = typeid(CB_Campos).hash_code();

RenderSystem::RenderSystem()
{
}

RenderSystem::~RenderSystem()
{
}

void RenderSystem::Init(HWND hWnd, UINT width, UINT height)
{
	//COM객체 사용을 위한 초기화, 텍스쳐사용에필요
	_ASEERTION_CREATE(CoInitialize(nullptr), "Coninitialize not successfully");
	
	m_iWidth = width;
	m_iHeight = height;
	m_pCDirect3D = new Direct3D();
	_ASEERTION_NULCHK(m_pCDirect3D, "D3D is nullptr");

	m_pCSwapChain = new SwapChain(m_pCDirect3D->GetDevice(), hWnd, width, height);
	_ASEERTION_NULCHK(m_pCSwapChain, "SC is nullptr");
	
	m_pCSamplers = new SamplerState(m_pCDirect3D->GetDevice());
	_ASEERTION_NULCHK(m_pCSamplers, "Samplers is nullptr");

	m_pCRSStaets = new RasterizerState(m_pCDirect3D->GetDevice());
	_ASEERTION_NULCHK(m_pCRSStaets, "Samplers is nullptr");


	//상수버퍼 기본세팅
	_RenderSystem.CreateConstantBuffer(typeid(CB_DirectionalLight), sizeof(CB_DirectionalLight));
	_RenderSystem.CreateConstantBuffer(typeid(CB_PointLight), sizeof(CB_PointLight));
	_RenderSystem.CreateConstantBuffer(typeid(CB_SpotLight), sizeof(CB_SpotLight));
	_RenderSystem.CreateConstantBuffer(typeid(CB_WVPITMatrix), sizeof(CB_WVPITMatrix));
	_RenderSystem.CreateConstantBuffer(typeid(CB_Time), sizeof(CB_Time));
	_RenderSystem.CreateConstantBuffer(typeid(CB_Campos), sizeof(CB_Campos));

	OnResize(m_iWidth, m_iHeight);
}

void RenderSystem::Frame(float deltatime)
{
	std::cout << "Frame : " << "RenderSystem" << " Class" << '\n';
	std::cout << "deltatime : " << deltatime << '\n';

	Vector3& dir = static_cast<DirectionalLight*>(_LightSystem.GetLight(0))->m_vDirection;
	dir = dir * GetMat_RotYaw(deltatime * 30.0f);

	Vector3& pos = static_cast<PointLight*>(_LightSystem.GetLight(1))->m_vPosition;
	pos = pos * GetMat_RotYaw(deltatime * 50.0f);

	SkyObj->m_vPosition = _CameraSystem.GetCamera(0)->GetPosition();
	
	for (const auto& iter : objs)
	{
		iter->Frame(deltatime);
	}
}

void RenderSystem::PreRender()
{
	std::cout << "PreRender : " << "RenderSystem" << " Class" << '\n';
	//RTV초기화
	m_pCSwapChain->ClearRenderTargetColor(m_pCDirect3D->GetDeviceContext(), 0, 0.3f, 0.4f, 1);
}

void RenderSystem::Render(float deltatime)
{
	std::cout << "Render : " << "RenderSystem" << " Class" << '\n';

	//프레임에따른 변환
	Matrix4x4 matWorld = _CameraSystem.GetCamera(0)->GetWorldMatrix();
	Matrix4x4 matView = _CameraSystem.GetCamera(0)->GetViewMatrix();
	Matrix4x4 matProj = _CameraSystem.GetCamera(0)->GetProjMatrix();
	{
		//SKYOBJ
		{
			//DirectionalLight
			m_pCCBs[g_hash_cbdirectionalLight]->UpdateBufferData(m_pCDirect3D->GetDeviceContext(), _LightSystem.GetLight(0)->GetConstant());
			m_pCCBs[g_hash_cbdirectionalLight]->SetPS(m_pCDirect3D->GetDeviceContext(), 0);

			//PointLight
			m_pCCBs[g_hash_cbpointlight]->UpdateBufferData(m_pCDirect3D->GetDeviceContext(), _LightSystem.GetLight(1)->GetConstant());
			m_pCCBs[g_hash_cbpointlight]->SetPS(m_pCDirect3D->GetDeviceContext(), 1);

			//SpotLight
			m_pCCBs[g_hash_cbspotlight]->UpdateBufferData(m_pCDirect3D->GetDeviceContext(), _LightSystem.GetLight(2)->GetConstant());
			m_pCCBs[g_hash_cbspotlight]->SetPS(m_pCDirect3D->GetDeviceContext(), 2);

			//상수버퍼에 cc0(wvp mat), cc1(시간) 을 세팅한다
			CB_WVPITMatrix cc0;
			cc0.matWorld = GetMat_WorldMatrix(SkyObj->m_vScale, SkyObj->m_vRotate, SkyObj->m_vPosition);
			cc0.matView = matView;
			cc0.matProj = matProj;
			cc0.matInvTrans = GetMat_InverseTranspose(cc0.matWorld);
			m_pCCBs[g_hash_cbwvpitmat]->UpdateBufferData(m_pCDirect3D->GetDeviceContext(), &cc0);
			m_pCCBs[g_hash_cbwvpitmat]->SetVS(m_pCDirect3D->GetDeviceContext(), 3);

			CB_Time cc1;
			cc1.fTime = deltatime;
			m_pCCBs[g_hash_cbtime]->UpdateBufferData(m_pCDirect3D->GetDeviceContext(), &cc1);
			m_pCCBs[g_hash_cbtime]->SetPS(m_pCDirect3D->GetDeviceContext(), 4);

			CB_Campos cc2;
			cc2.vPosition = _CameraSystem.GetCamera(0)->GetPosition();
			m_pCCBs[g_hash_cbcampos]->UpdateBufferData(m_pCDirect3D->GetDeviceContext(), &cc2);
			m_pCCBs[g_hash_cbcampos]->SetPS(m_pCDirect3D->GetDeviceContext(), 5);

			m_pCSamplers->SetPS(m_pCDirect3D->GetDeviceContext(), E_Samplers::WRAP_LINEAR);
			m_pCRSStaets->SetRS(m_pCDirect3D->GetDeviceContext(), E_Rasterizers::SOLID_CULLFRONT_CW);
		}

		for (UINT i = 0; i < SkyObj->m_Mesh_Material.size(); i++)
		{
			auto& iter = SkyObj->m_Mesh_Material[i];
			Mesh<Vertex_PTN>* pMesh = _ResourceSystem.GetResource<Mesh<Vertex_PTN>>(iter.hash_mesh);
			m_pCVBs[pMesh->GetVB()]->SetVertexBuffer(m_pCDirect3D->GetDeviceContext());
			m_pCIBs[pMesh->GetIB()]->SetIndexBuffer(m_pCDirect3D->GetDeviceContext());

			Material* pMaterial = _ResourceSystem.GetResource<Material>(iter.hash_material);
			m_pCILs[pMaterial->GetIL()]->SetInputLayout(m_pCDirect3D->GetDeviceContext());
			m_pCVSs[pMaterial->GetVS()]->SetVertexShader(m_pCDirect3D->GetDeviceContext());
			m_pCPSs[pMaterial->GetPS()]->SetPixelShader(m_pCDirect3D->GetDeviceContext());

			const std::vector<size_t>* texs = pMaterial->GetTextures();
			int cnt = 0;
			for (int idxTex = 0; idxTex < (UINT)E_Textures::count; idxTex++)
			{
				for (const auto& hashTx : texs[idxTex])
				{
					m_pCTXs[_ResourceSystem.GetResource<Texture>(hashTx)->GetTX()]->SetPS(m_pCDirect3D->GetDeviceContext(), cnt++);
				}
			}
			m_pCDirect3D->DrawIndex_TriagleList(pMesh->GetRendIndices()[i].count, pMesh->GetRendIndices()[i].idx, 0);
		}
	}

	//OBJS
	{
		{
			//DirectionalLight
			m_pCCBs[g_hash_cbdirectionalLight]->UpdateBufferData(m_pCDirect3D->GetDeviceContext(), _LightSystem.GetLight(0)->GetConstant());
			m_pCCBs[g_hash_cbdirectionalLight]->SetPS(m_pCDirect3D->GetDeviceContext(), 0);

			//PointLight
			m_pCCBs[g_hash_cbpointlight]->UpdateBufferData(m_pCDirect3D->GetDeviceContext(), _LightSystem.GetLight(1)->GetConstant());
			m_pCCBs[g_hash_cbpointlight]->SetPS(m_pCDirect3D->GetDeviceContext(), 1);

			//SpotLight
			m_pCCBs[g_hash_cbspotlight]->UpdateBufferData(m_pCDirect3D->GetDeviceContext(), _LightSystem.GetLight(2)->GetConstant());
			m_pCCBs[g_hash_cbspotlight]->SetPS(m_pCDirect3D->GetDeviceContext(), 2);

			CB_Time cc1;
			cc1.fTime = deltatime;
			m_pCCBs[g_hash_cbtime]->UpdateBufferData(m_pCDirect3D->GetDeviceContext(), &cc1);
			m_pCCBs[g_hash_cbtime]->SetPS(m_pCDirect3D->GetDeviceContext(), 4);

			CB_Campos cc2;
			cc2.vPosition = _CameraSystem.GetCamera(0)->GetPosition();
			m_pCCBs[g_hash_cbcampos]->UpdateBufferData(m_pCDirect3D->GetDeviceContext(), &cc2);
			m_pCCBs[g_hash_cbcampos]->SetPS(m_pCDirect3D->GetDeviceContext(), 5);

			m_pCSamplers->SetPS(m_pCDirect3D->GetDeviceContext(), E_Samplers::WRAP_LINEAR);
			m_pCRSStaets->SetRS(m_pCDirect3D->GetDeviceContext(), E_Rasterizers::SOLID_CULLBACK_CW);
		}
#ifndef _TESTBLOCK
		for (const auto& obj : objs)
		{
			if (!obj->bRenderable) continue;
			//상수버퍼에 cc0(wvp mat), cc1(시간) 을 세팅한다
			CB_WVPITMatrix cc0;
			cc0.matWorld = GetMat_WorldMatrix(obj->m_vScale, obj->m_vRotate, obj->m_vPosition);
			cc0.matView = matView;
			cc0.matProj = matProj;
			cc0.matInvTrans = GetMat_InverseTranspose(cc0.matWorld);
			m_pCCBs[g_hash_cbwvpitmat]->UpdateBufferData(m_pCDirect3D->GetDeviceContext(), &cc0);
			m_pCCBs[g_hash_cbwvpitmat]->SetVS(m_pCDirect3D->GetDeviceContext(), 3);

			for (UINT i = 0; i < obj->m_Mesh_Material.size(); i++)
			{
				auto& iter = obj->m_Mesh_Material[i];
				Mesh<Vertex_PTN>* pMesh = _ResourceSystem.GetResource<Mesh<Vertex_PTN>>(iter.hash_mesh);
				m_pCVBs[pMesh->GetVB()]->SetVertexBuffer(m_pCDirect3D->GetDeviceContext());
				m_pCIBs[pMesh->GetIB()]->SetIndexBuffer(m_pCDirect3D->GetDeviceContext());

				Material* pMaterial = _ResourceSystem.GetResource<Material>(iter.hash_material);
				m_pCILs[pMaterial->GetIL()]->SetInputLayout(m_pCDirect3D->GetDeviceContext());
				m_pCVSs[pMaterial->GetVS()]->SetVertexShader(m_pCDirect3D->GetDeviceContext());
				m_pCPSs[pMaterial->GetPS()]->SetPixelShader(m_pCDirect3D->GetDeviceContext());

				const std::vector<size_t>* texs = pMaterial->GetTextures();
				int cnt = 0;
				for (int idxTex = 0; idxTex < (UINT)E_Textures::count; idxTex++)
				{
					for (const auto& hashTx : texs[idxTex])
					{
						m_pCTXs[_ResourceSystem.GetResource<Texture>(hashTx)->GetTX()]->SetPS(m_pCDirect3D->GetDeviceContext(), cnt++);
					}
				}
				m_pCDirect3D->DrawIndex_TriagleList(pMesh->GetRendIndices()[i].count, pMesh->GetRendIndices()[i].idx, 0);
			}
		}
#endif 

#ifdef _TESTBLOCK
		for (int i = 0; i < objs.size(); i++)
		{
			const auto& obj = objs[i];
			if (!obj->bRenderable) continue;
			//상수버퍼에 cc0(wvp mat), cc1(시간) 을 세팅한다
			CB_WVPITMatrix cc0;
			/*objs[0]->m_vPosition = matWorld[3].ToVector3() + GetAxesUpFromWorld(matWorld) * -100.0f + GetAxesForwardFromWorld(matWorld) * 500.0f;
			Matrix4x4 objMat = GetMat_WorldMatrix(obj->m_vScale, obj->m_vRotate, obj->m_vPosition);
			matWorld[3] = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
			matWorld *= GetMat_Scale(objs[0]->m_vScale);
			matWorld[3] = objMat[3];
			cc0.matWorld = i == 0 ? matWorld : objMat;*/
			cc0.matWorld = GetMat_WorldMatrix(obj->m_vScale, obj->m_vRotate, obj->m_vPosition);
			cc0.matView = matView;
			cc0.matProj = matProj;
			cc0.matInvTrans = GetMat_InverseTranspose(cc0.matWorld);
			m_pCCBs[g_hash_cbwvpitmat]->UpdateBufferData(m_pCDirect3D->GetDeviceContext(), &cc0);
			m_pCCBs[g_hash_cbwvpitmat]->SetVS(m_pCDirect3D->GetDeviceContext(), 3);

			for (UINT i = 0; i < obj->m_Mesh_Material.size(); i++)
			{
				auto& iter = obj->m_Mesh_Material[i];
				//지정핸들링필요
				Mesh<Vertex_PTN>* pMesh = _ResourceSystem.GetResource<Mesh<Vertex_PTN>>(iter.hash_mesh);	
				m_pCVBs[pMesh->GetVB()]->SetVertexBuffer(m_pCDirect3D->GetDeviceContext());
				m_pCIBs[pMesh->GetIB()]->SetIndexBuffer(m_pCDirect3D->GetDeviceContext());

				Material* pMaterial = _ResourceSystem.GetResource<Material>(iter.hash_material);
				m_pCILs[pMaterial->GetIL()]->SetInputLayout(m_pCDirect3D->GetDeviceContext());
				m_pCVSs[pMaterial->GetVS()]->SetVertexShader(m_pCDirect3D->GetDeviceContext());
				m_pCPSs[pMaterial->GetPS()]->SetPixelShader(m_pCDirect3D->GetDeviceContext());

				const std::vector<size_t>* texs = pMaterial->GetTextures();
				int cnt = 0;
				for (int idxTex = 0; idxTex < (UINT)E_Textures::count; idxTex++)
				{
					for (const auto& hashTx : texs[idxTex])
					{
						m_pCTXs[_ResourceSystem.GetResource<Texture>(hashTx)->GetTX()]->SetPS(m_pCDirect3D->GetDeviceContext(), cnt++);
					}
				}
				m_pCDirect3D->DrawIndex_TriagleList(pMesh->GetRendIndices()[i].count, pMesh->GetRendIndices()[i].idx, 0);
			}
		}
#endif 
	}
}

void RenderSystem::PostRender()
{
	std::cout << "PostRender : " << "RenderSystem" << " Class" << '\n';
	m_pCSwapChain->Present(false);
}

void RenderSystem::Release()
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

	for (auto iter = m_pCTXs.begin(); iter != m_pCTXs.end();)
	{
		delete iter->second;
		iter = m_pCTXs.erase(iter);
	}

	if (SkyObj)
		delete SkyObj;

	for (auto iter = objs.begin(); iter != objs.end();)
	{
		delete *iter;
		iter = objs.erase(iter);
	}

	if (m_pCSamplers)
	{
		delete m_pCSamplers;
		m_pCSamplers = nullptr;
	}

	if (m_pCRSStaets)
	{
		delete m_pCRSStaets;
		m_pCRSStaets = nullptr;
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

ID3D11Device* RenderSystem::GetD3DDevice() const
{
	return m_pCDirect3D->GetDevice();
}

ID3D11DeviceContext* RenderSystem::GetD3DDeviceContext() const
{
	return m_pCDirect3D->GetDeviceContext();
}

void RenderSystem::OnResize(UINT width, UINT height)
{
	if (!m_pCDirect3D) return;
	if (width == 0 || height == 0) return;
	m_iWidth = width;
	m_iHeight = height;

	m_pCSwapChain->ResizeBuffers(1, width, height, DXGI_FORMAT_UNKNOWN, 0);
	m_pCSwapChain->CreateRenderTargetView(m_pCDirect3D->GetDevice());
	m_pCSwapChain->CreateDepthStencilView(m_pCDirect3D->GetDevice(), width, height);
	m_pCDirect3D->SetViewportSize(width, height);
}
size_t RenderSystem::CreateVertexBuffer(const std::wstring& szName, void* vertices, UINT size_vertex, UINT size_vertices)
{
	size_t hash = HashingFile(szName);
	if (m_pCVBs.find(hash) != m_pCVBs.end()) return hash;
	VertexBuffer* pVertexBuffer = new VertexBuffer(m_pCDirect3D->GetDevice(), vertices, size_vertex, size_vertices);
	_ASEERTION_NULCHK(pVertexBuffer, "VB is nullptr");
	m_pCVBs[hash] = pVertexBuffer;
	return hash;
}

size_t RenderSystem::CreateInputLayout(const std::wstring& szName, D3D11_INPUT_ELEMENT_DESC* pInputElementDescs, UINT size_layout, ID3DBlob* vsBlob)
{
	size_t hash = HashingFile(szName);
	if (m_pCILs.find(hash) != m_pCILs.end()) return hash;
	_ASEERTION_NULCHK(vsBlob, "Blob is nullptr");
	InputLayout* pInputLayout = new InputLayout(m_pCDirect3D->GetDevice(), pInputElementDescs, size_layout, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize());
	_ASEERTION_NULCHK(pInputLayout, "IL is nullptr");
	m_pCILs[hash] = pInputLayout;
	return hash;
}

size_t RenderSystem::CreateIndexBuffer(const std::wstring& szName, void* indices, UINT size_indices)
{
	size_t hash = HashingFile(szName);
	if (m_pCIBs.find(hash) != m_pCIBs.end()) return hash;
	IndexBuffer* pIndexBuffer = new IndexBuffer(m_pCDirect3D->GetDevice(), indices, size_indices);
	_ASEERTION_NULCHK(pIndexBuffer, "IB is nullptr");
	m_pCIBs[hash] = pIndexBuffer;
	return hash;
}

size_t RenderSystem::CreateTexture2D(const std::wstring& szName, const ScratchImage* resource)
{
	size_t hash = HashingFile(szName);
	if (m_pCTXs.find(hash) != m_pCTXs.end()) return hash;
	_ASEERTION_NULCHK(resource, "scratchImage is nullptr");
	Texture2D* pTexture2D = new Texture2D(m_pCDirect3D->GetDevice(), resource);
	_ASEERTION_NULCHK(pTexture2D, "TX is nullptr");
	m_pCTXs[hash] = pTexture2D;
	return hash;
}

size_t RenderSystem::CreateConstantBuffer(const type_info& typeinfo, UINT size_buffer, void* data)
{
	size_t hash = typeinfo.hash_code();
	if (m_pCCBs.find(hash) != m_pCCBs.end()) return hash;
	ConstantBuffer* pConstantBuffer = new ConstantBuffer(m_pCDirect3D->GetDevice(), size_buffer, data);
	_ASEERTION_NULCHK(pConstantBuffer, "CB is nullptr");
	m_pCCBs[hash] = pConstantBuffer;
	return hash;
}

size_t RenderSystem::CreateVertexShader(std::wstring shaderName, std::string entryName, std::string target)
{
	size_t hash = HashingFile(shaderName);
	if (m_pCVSs.find(hash) != m_pCVSs.end()) return hash;

	VertexShader* pVertexShader = new VertexShader(m_pCDirect3D->GetDevice(), CompileShader(shaderName, entryName, target));
	_ASEERTION_NULCHK(pVertexShader, "VS is nullptr");
	m_pCVSs[hash] = pVertexShader;
	return hash;
}

size_t RenderSystem::CreatePixelShader(std::wstring shaderName, std::string entryName, std::string target)
{
	size_t hash = HashingFile(shaderName);
	if (m_pCPSs.find(hash) != m_pCPSs.end()) return hash;

	PixelShader* pPixelShader = new PixelShader(m_pCDirect3D->GetDevice(), CompileShader(shaderName, entryName, target));
	_ASEERTION_NULCHK(pPixelShader, "PS is nullptr");
	m_pCPSs[hash] = pPixelShader;
	return hash;
}

size_t RenderSystem::CreateTexture(const std::wstring& szFilePath, DirectX::WIC_FLAGS flag)
{
	// DirectXTex의 함수를 이용하여 image_data로 리턴, imageData로부터 ID3D11Resource 객체를 생성한다
	Texture* pTexture = _ResourceSystem.CreateResourceFromFile<Texture>(szFilePath, flag);
	pTexture->SetTX(CreateTexture2D(szFilePath + L"TX", pTexture->GetImage()));
	return HashingFile(szFilePath);
}
size_t RenderSystem::CreateTexture(const std::wstring& szFilePath, DirectX::DDS_FLAGS flag)
{
	// DirectXTex의 함수를 이용하여 image_data로 리턴, imageData로부터 ID3D11Resource 객체를 생성한다
	Texture* pTexture = _ResourceSystem.CreateResourceFromFile<Texture>(szFilePath, flag);
	pTexture->SetTX(CreateTexture2D(szFilePath + L"TX", pTexture->GetImage()));
	return HashingFile(szFilePath);
}

//추론명시
template size_t RenderSystem::CreateMesh<Vertex_PTN>(const std::wstring& szFilePath, E_Colliders collider);
template size_t RenderSystem::CreateMesh<Vertex_PTNTB>(const std::wstring& szFilePath, E_Colliders collider);
template size_t RenderSystem::CreateMaterial<Vertex_PTN>(const std::wstring& szFilePath, const std::wstring& vsName, const std::wstring& psName);
template size_t RenderSystem::CreateMaterial<Vertex_PTNTB>(const std::wstring& szFilePath, const std::wstring& vsName, const std::wstring& psName);
template std::vector<size_t> RenderSystem::CreateMaterials<Vertex_PTN>(const std::wstring& szFilePath, const std::vector<std::wstring>& vss, const std::vector<std::wstring>& pss);
template std::vector<size_t> RenderSystem::CreateMaterials<Vertex_PTNTB>(const std::wstring& szFilePath, const std::vector<std::wstring>& vss, const std::vector<std::wstring>& pss);
template<typename T>
size_t RenderSystem::CreateMesh(const std::wstring& szFilePath, E_Colliders collider)
{
	Mesh<T>* pMesh = _ResourceSystem.CreateResourceFromFile<Mesh<T>>(szFilePath);
	pMesh->SetVB(CreateVertexBuffer(szFilePath + L"VB", pMesh->GetVertices(), sizeof(T), (UINT)pMesh->GetVerticesSize()));
	pMesh->SetIB(CreateIndexBuffer(szFilePath + L"IB", pMesh->GetIndices(), (UINT)pMesh->GetIndicesSize()));
	for (UINT idx = 0; idx < pMesh->GetPoints().size(); idx++)
		pMesh->SetCL(_CollisionSystem.CreateCollider(szFilePath + std::to_wstring(idx), &pMesh->GetPoints()[idx], collider));
	return HashingFile(szFilePath);
}
template<typename T>
size_t RenderSystem::CreateMaterial(const std::wstring& szFilePath, const std::wstring& vsName, const std::wstring& psName)
{
	Material* pMaterial = _ResourceSystem.CreateResourceFromFile<Material>(szFilePath);
	pMaterial->SetVS(CreateVertexShader(vsName, "vsmain", "vs_5_0"));
	pMaterial->SetPS(CreatePixelShader(psName, "psmain", "ps_5_0"));
	pMaterial->SetIL(CreateInputLayout(vsName + L"IL", Traits_InputLayout<T>::GetLayout(), Traits_InputLayout<T>::GetSize(), m_pCVSs[CreateVertexShader(vsName, "vsmain", "vs_5_0")]->GetBlob()));
	return HashingFile(szFilePath);
}
template<typename T>
std::vector<size_t> RenderSystem::CreateMaterials(const std::wstring& szFilePath, const std::vector<std::wstring>& VSs, const std::vector<std::wstring>& PSs)
{
	std::vector<size_t> rets;
	std::vector<Material*> materials = _ResourceSystem.CreateResourcesFromFile<Material>(szFilePath);
	_ASEERTION_NULCHK(materials.size() == VSs.size(), "material and shader counts not equal");
	for (UINT i = 0; i < materials.size(); i++)
	{
		materials[i]->SetVS(CreateVertexShader(VSs[i], "vsmain", "vs_5_0"));
		materials[i]->SetPS(CreatePixelShader(PSs[i], "psmain", "ps_5_0"));
		materials[i]->SetIL(CreateInputLayout(VSs[i] + L"IL", Traits_InputLayout<T>::GetLayout(), Traits_InputLayout<T>::GetSize(), m_pCVSs[CreateVertexShader(VSs[i], "vsmain", "vs_5_0")]->GetBlob()));
		rets.push_back(materials[i]->GetHash());
	}
	return rets;
}

void RenderSystem::Material_SetTextures(size_t hash_material, const std::vector<TX_HASH>& textures)
{
	Material* pMaterial = _ResourceSystem.GetResource<Material>(hash_material);
	for (const auto& iter : textures)
		pMaterial->SetTexture(iter);
}

ID3DBlob* RenderSystem::CompileShader(std::wstring shaderName, std::string entryName, std::string target)
{
	ID3DBlob* pBlob;
	ID3DBlob* errBlob;
	HRESULT hResult;
	DWORD dwShaderFlags = D3DCOMPILE_DEBUG;
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

