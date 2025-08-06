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
#include "Mesh.h"
#include "LightSystem.h"
#include "Light.h"

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

	OnResize(m_iWidth, m_iHeight);
}

void RenderSystem::Frame()
{
	std::cout << "Frame : " << "RenderSystem" << " Class" << '\n';
	std::cout << "ElapsedTime : " << m_fElapsedtime << '\n';
	
	for (const auto& iter : objs)
	{
		iter->Frame(m_fDeltatime);
	}

	//시간경과, 추후 Timer클래스로분할
	m_dwCurTick = (DWORD)::GetTickCount64();
	if(m_dwOldTick <= 0)
		m_dwOldTick = m_dwCurTick;
	DWORD dwElaspsed = m_dwCurTick - m_dwOldTick;
	m_fDeltatime = (float)dwElaspsed * 0.001f;
	m_fElapsedtime += m_fDeltatime;
	m_dwOldTick = m_dwCurTick;
}

void RenderSystem::PreRender()
{
	std::cout << "PreRender : " << "RenderSystem" << " Class" << '\n';
	//RTV초기화
	m_pCSwapChain->ClearRenderTargetColor(m_pCDirect3D->GetDeviceContext(), 0, 0.3f, 0.4f, 1);
}
#define __OPTIMIZED
void RenderSystem::Render()
{
	std::cout << "Render : " << "RenderSystem" << " Class" << '\n';

	//프레임에따른 변환
	Matrix4x4 matView = _CameraSystem.GetCamera(0)->GetViewMatrix();
	//XMMATRIX matView = GetMat_ViewMatrix(XMFLOAT3(-300.0f, 500.0f, -1000.0f), XMFLOAT3(0.0f, 0.0f, 0.0f));

	//cc0.matProj = GetMat_Ortho(m_fWidth, m_fHeight, -4000.0f, 4000.0f);
	_CameraSystem.GetCamera(0)->SetAsepectRatio((float)m_iWidth / (float)m_iHeight);
	_CameraSystem.GetCamera(0)->SetFOV(75.0f);
	_CameraSystem.GetCamera(0)->SetClipPlanes(0.1f, 4000.0f);
	Matrix4x4 matProj = _CameraSystem.GetCamera(0)->GetProjMatrix();
	//XMMATRIX matProj = GetMat_Perspective((float)m_iWidth, (float)m_iHeight, 75.0f, 0.1f, 4000.0f);

	for (const auto& iter : objs)
	{
		for (const auto hash : iter->m_hashMeshes)
		{
			size_t idxVB = _ResourceSystem.GetResource<Mesh>(hash)->GetIdx_VB();
			size_t idxIB = _ResourceSystem.GetResource<Mesh>(hash)->GetIdx_IB();
			size_t idxIL = _ResourceSystem.GetResource<Mesh>(hash)->GetIdx_IL();
			size_t idxVS = _ResourceSystem.GetResource<Mesh>(hash)->GetIdx_VS();
			size_t idxPS = _ResourceSystem.GetResource<Mesh>(hash)->GetIdx_PS();
			
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
			cc1.fTime = _DEGTORAD(m_fElapsedtime * 360.0f);
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

	// 1. 렌더 타겟 뷰와 깊이 스텐실 뷰를 해제
	// m_pRTV->Release();
	// m_pDSV->Release();

	// 2. 스왑체인 버퍼 크기 재조정
	// m_pCSwapChain->ResizeBuffers(1, width, height, DXGI_FORMAT_UNKNOWN, 0);

	// 3. 새로운 렌더 타겟 뷰 생성 (위의 SwapChain이 캡슐화한 로직)
	// m_pCSwapChain->CreateNewRTV();

	// 4. 새로운 깊이/스텐실 뷰 생성
	// m_pCDirect3D->CreateDepthStencilView();

	m_pCDirect3D->SetViewportSize(m_iWidth, m_iHeight);
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
	size_t hash = _ResourceSystem.HashFilePath(szFilePath);
	Texture* pTexture = _ResourceSystem.GetResource<Texture>(_ResourceSystem.HashFilePath(szFilePath));
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

size_t RenderSystem::CreateMesh(const std::wstring& szFilePath)
{
	//이미 있는 리소스라면
	size_t hash = _ResourceSystem.HashFilePath(szFilePath);
	Mesh* pMesh = _ResourceSystem.GetResource<Mesh>(hash);
	if (pMesh)
		return hash;

	pMesh = _ResourceSystem.CreateResourceFromFile<Mesh>(szFilePath);
	pMesh->SetIdx_VB(CreateVertexBuffer(pMesh->GetVertices(), sizeof(Vertex_PTN), pMesh->GetVerticesSize()));
	pMesh->SetIdx_IB(CreateIndexBuffer(pMesh->GetIndices(), pMesh->GetIndicesSize()));
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