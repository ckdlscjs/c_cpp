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

	OnResize(m_iWidth, m_iHeight);
}

void RenderSystem::Frame(float deltatime)
{
	std::cout << "Frame : " << "RenderSystem" << " Class" << '\n';
	std::cout << "deltatime : " << deltatime << '\n';
	objs[0]->m_vPosition = _CameraSystem.GetCamera(0)->GetPosition();
	for (int i = 1; i < objs.size(); i++)
	{
		objs[i]->Frame(deltatime);
	}
}

void RenderSystem::PreRender()
{
	std::cout << "PreRender : " << "RenderSystem" << " Class" << '\n';
	//RTV초기화
	m_pCSwapChain->ClearRenderTargetColor(m_pCDirect3D->GetDeviceContext(), 0, 0.3f, 0.4f, 1);
}

void RenderSystem::Render()
{
	std::cout << "Render : " << "RenderSystem" << " Class" << '\n';

	//프레임에따른 변환
	Matrix4x4 matView = _CameraSystem.GetCamera(0)->GetViewMatrix();
	Matrix4x4 matProj = _CameraSystem.GetCamera(0)->GetProjMatrix();
	

	for (int i = 0; i < _RenderSystem.objs.size(); i++)
	{
		auto iter = _RenderSystem.objs[i];
		if (!iter->bRenderable) continue;
		for (const auto hash : iter->m_hashMeshes)
		{
			size_t idxVB = _ResourceSystem.GetResource<Mesh>(hash)->GetIdx_VB();
			size_t idxIB = _ResourceSystem.GetResource<Mesh>(hash)->GetIdx_IB();
			size_t idxIL = _ResourceSystem.GetResource<Mesh>(hash)->GetIdx_IL();
			size_t idxVS = _ResourceSystem.GetResource<Mesh>(hash)->GetIdx_VS();
			size_t idxPS = i == 0 ? 2 : _ResourceSystem.GetResource<Mesh>(hash)->GetIdx_PS();

			m_pCVBs[idxVB]->SetVertexBuffer(m_pCDirect3D->GetDeviceContext());
			m_pCIBs[idxIB]->SetIndexBuffer(m_pCDirect3D->GetDeviceContext());
			m_pCILs[idxIL]->SetInputLayout(m_pCDirect3D->GetDeviceContext());
			m_pCVSs[idxVS]->SetVertexShader(m_pCDirect3D->GetDeviceContext());
			m_pCPSs[idxPS]->SetPixelShader(m_pCDirect3D->GetDeviceContext());

			//DirectionalLight
			m_pCCBs[iter->m_IdxCBs[0]]->UpdateBufferData(m_pCDirect3D->GetDeviceContext(), _LightSystem.GetLight(0)->GetConstant());
			m_pCCBs[iter->m_IdxCBs[0]]->SetPS(m_pCDirect3D->GetDeviceContext(), 0);

			//PointLight
			m_pCCBs[iter->m_IdxCBs[1]]->UpdateBufferData(m_pCDirect3D->GetDeviceContext(), _LightSystem.GetLight(1)->GetConstant());
			m_pCCBs[iter->m_IdxCBs[1]]->SetPS(m_pCDirect3D->GetDeviceContext(), 1);

			//SpotLight
			m_pCCBs[iter->m_IdxCBs[2]]->UpdateBufferData(m_pCDirect3D->GetDeviceContext(), _LightSystem.GetLight(2)->GetConstant());
			m_pCCBs[iter->m_IdxCBs[2]]->SetPS(m_pCDirect3D->GetDeviceContext(), 2);

			//상수버퍼에 cc0(wvp mat), cc1(시간) 을 세팅한다
			CB_WVPITMatrix cc0;
			cc0.matWorld = GetMat_WorldMatrix(iter->m_vScale, iter->m_vRotate, iter->m_vPosition);
			cc0.matView = matView;
			cc0.matProj = matProj;
			cc0.matInvTrans = GetMat_InverseTranspose(cc0.matWorld);
			m_pCCBs[iter->m_IdxCBs[3]]->UpdateBufferData(m_pCDirect3D->GetDeviceContext(), &cc0);
			m_pCCBs[iter->m_IdxCBs[3]]->SetVS(m_pCDirect3D->GetDeviceContext(), 3);

			Constant_time cc1;
			cc1.fTime = _DEGTORAD(1.0f * 360.0f);
			m_pCCBs[iter->m_IdxCBs[4]]->UpdateBufferData(m_pCDirect3D->GetDeviceContext(), &cc1);
			m_pCCBs[iter->m_IdxCBs[4]]->SetPS(m_pCDirect3D->GetDeviceContext(), 4);

			CB_Campos cc2;
			cc2.camPos = _CameraSystem.GetCamera(0)->GetPosition();
			m_pCCBs[iter->m_IdxCBs[5]]->UpdateBufferData(m_pCDirect3D->GetDeviceContext(), &cc2);
			m_pCCBs[iter->m_IdxCBs[5]]->SetPS(m_pCDirect3D->GetDeviceContext(), 5);

			size_t idxTX = _ResourceSystem.GetResource<Texture>(iter->m_hashTextures.back())->GetIdx_TX();
			//m_pCTXs[iter->m_IdxTX]->SetVS(m_pCDirect3D->GetDeviceContext(), 0);
			m_pCSamplers->SetPS(m_pCDirect3D->GetDeviceContext(), m_pCTXs[idxTX]->GetSampler());
			m_pCTXs[idxTX]->SetPS(m_pCDirect3D->GetDeviceContext(), 0);
			if(i == 0)
				m_pCRSStaets->SetRS(m_pCDirect3D->GetDeviceContext(), 2);
			else
				m_pCRSStaets->SetRS(m_pCDirect3D->GetDeviceContext(), 0);
			//m_pCDirect3D->DrawVertex_TriangleStrip(m_pCVertexBuffer->GetCountVertices(), 0);
		
			m_pCDirect3D->DrawIndex_TriagleList(m_pCIBs[idxIB]->GetCountIndices(), 0, 0);
		}
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
	
	if (m_pBlob_VS)
	{
		m_pBlob_VS->Release();
		m_pBlob_VS = nullptr;
	}
	if (m_pBlob_PS)
	{
		m_pBlob_PS->Release();
		m_pBlob_PS = nullptr;
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

size_t RenderSystem::CreateVertexBuffer(void* vertices, UINT size_vertex, UINT size_vertices)
{
	VertexBuffer* pVertexBuffer = new VertexBuffer(m_pCDirect3D->GetDevice(), vertices, size_vertex, size_vertices);
	_ASEERTION_NULCHK(pVertexBuffer, "VB is nullptr");
	m_pCVBs[m_lIdx_CVBs] = pVertexBuffer;
	return m_lIdx_CVBs++;
}

size_t RenderSystem::CreateInputLayout(D3D11_INPUT_ELEMENT_DESC* pInputElementDescs, UINT size_layout)
{
	InputLayout* pInputLayout = new InputLayout(m_pCDirect3D->GetDevice(), pInputElementDescs, size_layout, m_pBlob_VS->GetBufferPointer(), m_pBlob_VS->GetBufferSize());
	_ASEERTION_NULCHK(pInputLayout, "IL is nullptr");
	m_pCILs[m_lIdx_CILs] = pInputLayout;
	return m_lIdx_CILs++;
}

size_t RenderSystem::CreateIndexBuffer(void* indices, UINT size_indices)
{
	IndexBuffer* pIndexBuffer = new IndexBuffer(m_pCDirect3D->GetDevice(), indices, size_indices);
	_ASEERTION_NULCHK(pIndexBuffer, "IB is nullptr");
	m_pCIBs[m_lIdx_CIBs] = pIndexBuffer;
	return m_lIdx_CIBs++;
}

size_t RenderSystem::CreateConstantBuffer(UINT size_buffer, void* data)
{
	ConstantBuffer* pConstantBuffer = new ConstantBuffer(m_pCDirect3D->GetDevice(), size_buffer, data);
	_ASEERTION_NULCHK(pConstantBuffer, "CB is nullptr");
	m_pCCBs[m_lIdx_CCBs] = pConstantBuffer;
	return m_lIdx_CCBs++;
}

size_t RenderSystem::CreateVertexShader(std::wstring shaderName, std::string entryName, std::string target)
{
	if(m_pBlob_VS)
		m_pBlob_VS->Release();
	m_pBlob_VS = CompileShader(shaderName, entryName, target);
	_ASEERTION_NULCHK(m_pBlob_VS, "blob is nullptr");

	VertexShader* pVertexShader = new VertexShader(m_pCDirect3D->GetDevice(), m_pBlob_VS);
	_ASEERTION_NULCHK(pVertexShader, "VS is nullptr");
	m_pCVSs[m_lIdx_CVSs] = pVertexShader;
	return m_lIdx_CVSs++;
}

size_t RenderSystem::CreatePixelShader(std::wstring shaderName, std::string entryName, std::string target)
{
	if(m_pBlob_PS)
		m_pBlob_PS->Release();
	m_pBlob_PS = CompileShader(shaderName, entryName, target);
	_ASEERTION_NULCHK(m_pBlob_PS, "blob is nullptr");

	PixelShader* pPixelShader = new PixelShader(m_pCDirect3D->GetDevice(), m_pBlob_PS);
	_ASEERTION_NULCHK(pPixelShader, "PS is nullptr");
	m_pCPSs[m_lIdx_CPSs] = pPixelShader;
	return m_lIdx_CPSs++;
}

size_t RenderSystem::CreateTexture(const std::wstring& szFilePath, DirectX::WIC_FLAGS flag, Samplers sampler)
{
	//이미 있는 리소스라면
	size_t hash = HashFilePath(szFilePath);
	Texture* pTexture = _ResourceSystem.GetResource<Texture>(HashFilePath(szFilePath));
	if (pTexture)
		return hash;

	// DirectXTex의 함수를 이용하여 image_data로 리턴시킨다
	pTexture = _ResourceSystem.CreateResourceFromFile<Texture>(szFilePath, flag);

	// imageData로부터 ID3D11Resource 객체를 생성한다
	size_t idxTX = CreateTexture2D(pTexture->GetImage(), sampler);
	pTexture->SetIdx_TX(idxTX);
	return hash;
}

size_t RenderSystem::CreateTexture2D(const ScratchImage* resource, Samplers sampler)
{
	_ASEERTION_NULCHK(resource, "scratchImage is nullptr");
	Texture2D* pTexture2D = new Texture2D(m_pCDirect3D->GetDevice(), resource, sampler);
	_ASEERTION_NULCHK(pTexture2D, "TX is nullptr");
	m_pCTXs[m_lIdx_CTXs] = pTexture2D;
	return m_lIdx_CTXs++;
}

size_t RenderSystem::CreateMesh(const std::wstring& szFilePath, Colliders collider)
{
	//이미 있는 리소스라면
	size_t hash = HashFilePath(szFilePath);
	Mesh* pMesh = _ResourceSystem.GetResource<Mesh>(hash);
	if (pMesh)
		return hash;

	pMesh = _ResourceSystem.CreateResourceFromFile<Mesh>(szFilePath);
	pMesh->SetIdx_Collider(_CollisionSystem.CreateCollider(szFilePath, pMesh->GetPoss(), collider));
	pMesh->SetIdx_VB(CreateVertexBuffer(pMesh->GetVertices(), sizeof(Vertex_PTN), (UINT)pMesh->GetVerticesSize()));
	pMesh->SetIdx_IB(CreateIndexBuffer(pMesh->GetIndices(), (UINT)pMesh->GetIndicesSize()));
	pMesh->SetIdx_VS(CreateVertexShader(L"VertexShaderPTN.hlsl", "vsmain", "vs_5_0"));
	pMesh->SetIdx_PS(CreatePixelShader(L"PixelShaderPTN.hlsl", "psmain", "ps_5_0"));
	pMesh->SetIdx_IL(CreateInputLayout(InputLayout_VertexPTN, size_InputLayout_VertexPTN)); 
	return hash;
}

ID3DBlob* RenderSystem::CompileShader(std::wstring shaderName, std::string entryName, std::string target)
{
	ID3DBlob* pBlob;
	ID3DBlob* errBlob;
	HRESULT hResult;
	DWORD dwShaderFlags = D3DCOMPILE_DEBUG;
	//compile Shader
	hResult = D3DCompileFromFile(shaderName.c_str(), nullptr, nullptr, entryName.c_str(), target.c_str(), dwShaderFlags, NULL, &pBlob, &errBlob);
	if (FAILED(hResult))
	{
		OutputDebugStringA((char*)errBlob->GetBufferPointer());
		if (errBlob)
			errBlob->Release();
		_ASEERTION_CREATE(hResult, "CompileShader");
	}
	return pBlob;
}