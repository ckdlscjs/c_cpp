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
	m_dwCurTick = ::GetTickCount();
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

void RenderSystem::Render()
{
	std::cout << "Render : " << "RenderSystem" << " Class" << '\n';

	//프레임에따른 변환
	XMMATRIX matView = _CameraSystem.GetCamera(0)->GetViewMatrix();
	//XMMATRIX matView = GetMat_ViewMatrix(XMFLOAT3(-300.0f, 500.0f, -1000.0f), XMFLOAT3(0.0f, 0.0f, 0.0f));

	//cc0.matProj = GetMat_Ortho(m_fWidth, m_fHeight, -4000.0f, 4000.0f);
	_CameraSystem.GetCamera(0)->SetAsepectRatio((float)m_iWidth / (float)m_iHeight);
	_CameraSystem.GetCamera(0)->SetFOV(75.0f);
	_CameraSystem.GetCamera(0)->SetClipPlanes(0.1f, 4000.0f);
	XMMATRIX matProj = _CameraSystem.GetCamera(0)->GetProjMatrix();
	//XMMATRIX matProj = GetMat_Perspective((float)m_iWidth, (float)m_iHeight, 75.0f, 0.1f, 4000.0f);

	for (const auto& iter : objs)
	{
		m_pCVBs[iter->m_IdxVB]->SetVertexBuffer(m_pCDirect3D->GetDeviceContext());
		m_pCIBs[iter->m_IdxIB]->SetIndexBuffer(m_pCDirect3D->GetDeviceContext());
		m_pCILs[iter->m_IdxIL]->SetInputLayout(m_pCDirect3D->GetDeviceContext());
		m_pCVSs[iter->m_IdxVS]->SetVertexShader(m_pCDirect3D->GetDeviceContext());
		m_pCPSs[iter->m_IdxPS]->SetPixelShader(m_pCDirect3D->GetDeviceContext());
		//상수버퍼에 cc0(wvp mat), cc1(시간) 을 세팅한다
		Constant_wvp cc0;
		cc0.matWorld = GetMat_WorldMatrix(iter->m_vScale, iter->m_vRotate, iter->m_vTranslation);
		cc0.matView = matView;
		cc0.matProj = matProj;
		m_pCCBs[iter->m_IdxCBs[0]]->UpdateBufferData(m_pCDirect3D->GetDeviceContext(), &cc0);
		m_pCCBs[iter->m_IdxCBs[0]]->SetVS(m_pCDirect3D->GetDeviceContext(), 0);
		Constant_time cc1;
		cc1.fTime = _DEGTORAD(m_fElapsedtime * 360.0f);
		m_pCCBs[iter->m_IdxCBs[1]]->UpdateBufferData(m_pCDirect3D->GetDeviceContext(), &cc1);
		m_pCCBs[iter->m_IdxCBs[1]]->SetPS(m_pCDirect3D->GetDeviceContext(), 1);
		//m_pCTXs[iter->m_IdxTX]->SetVS(m_pCDirect3D->GetDeviceContext(), 0);
		m_pCSamplers->SetPS(m_pCDirect3D->GetDeviceContext(), m_pCTXs[iter->m_IdxTX]->GetSampler());
		m_pCTXs[iter->m_IdxTX]->SetPS(m_pCDirect3D->GetDeviceContext(), 0);
		//m_pCDirect3D->DrawVertex_TriangleStrip(m_pCVertexBuffer->GetCountVertices(), 0);
		m_pCDirect3D->DrawIndex_TriagleList(m_pCIBs[iter->m_IdxIB]->GetCountIndices(), 0, 0);
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

size_t RenderSystem::CreateConstantBuffer(void* data, UINT size_buffer)
{
	ConstantBuffer* pConstantBuffer = new ConstantBuffer(m_pCDirect3D->GetDevice(), data, size_buffer);
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

size_t RenderSystem::CreateTexture(const std::wstring& szFilePath, Samplers sampler)
{
	//이미 있는 리소스라면
	Texture* pTexture = _ResourceSystem.GetResource<Texture>(_ResourceSystem.HashFilePath(szFilePath));
	if (pTexture)
		return pTexture->GetTXIdx();

	/*
	WIC_FLAGS_NONE(0x0) :
	설명: 아무 플래그도 설정하지 않습니다.WIC가 파일의 메타데이터를 기반으로 최선의 판단을 내려 텍스처를 로드합니다.
	특별한 변환이나 처리가 필요 없는 대부분의 기본 텍스처 로딩에 사용됩니다.파일에 sRGB 정보가 있으면 일반적으로 sRGB 포맷으로 로드되고, 없으면 선형 포맷으로 로드됩니다.

	WIC_FLAGS_IGNORE_SRGB(0x20) :
	설명 : 이미지 파일에 sRGB 메타데이터가 있더라도 이를 무시하고 강제로 선형(Linear) 감마 공간으로 텍스처를 로드합니다.즉, 텍스처는 DXGI_FORMAT_R8G8B8A8_UNORM(sRGB 아님)과 같은 선형 포맷으로 GPU에 올라갑니다.
	데이터 텍스처 : 노멀 맵(Normal Map), 러프니스 맵(Roughness Map), 메탈니스 맵(Metallic Map), AO(Ambient Occlusion) 맵 등 색상 정보가 아닌 물리적 속성이나 벡터 데이터를 담는 텍스처를 로드할 때 필수적입니다.
	이들은 감마 보정이 적용되면 안 되기 때문입니다.
	자체적인 감마 관리 : 만약 셰이더에서 모든 감마 보정을 수동으로 처리하고 싶거나, 렌더 타겟 포맷(RTV)에서 sRGB를 지정하여 최종 출력 단계에서만 감마 보정을 하려는 경우에도 사용할 수 있습니다.

	WIC_FLAGS_FORCE_SRGB(0x40) :
	설명 : 파일의 메타데이터와 관계없이 강제로 sRGB 감마 공간으로 텍스처를 로드합니다.즉, 텍스처는 DXGI_FORMAT_R8G8B8A8_UNORM_SRGB와 같은 sRGB 포맷으로 GPU에 올라갑니다.
	알베도(Albedo) 또는 디퓨즈(Diffuse) 맵 : 물체의 기본 색상을 나타내는 텍스처는 일반적으로 sRGB 감마 공간에 있습니다.
	이 플래그를 사용하면 GPU가 셰이더에서 샘플링할 때 자동으로 선형 공간으로 변환해 주어 정확한 라이팅 계산이 가능해집니다.
	대부분의 ColorMap에 이 플래그를 사용하거나, WIC_FLAGS_DEFAULT_SRGB를 사용합니다.
	*/
	// imageData로부터 ID3D11Resource 객체를 생성한다
	// 라이브러리의 함수에 파일주소만을 넘겨 객체포인터를 받아와 이를 이용한다

	// DirectXTex의 함수를 이용하여 image_data로 리턴시킨다
	ScratchImage imageData;
	_ASEERTION_CREATE(DirectX::LoadFromWICFile(szFilePath.c_str(), WIC_FLAGS_NONE, nullptr, imageData), "LoadTexture not successfully");

	size_t idxTX = CreateTexture2D(&imageData, sampler);
	pTexture = _ResourceSystem.GetResource<Texture>(_ResourceSystem.CreateResourceFromFile<Texture>(szFilePath));
	pTexture->SetTXIdx(idxTX);
	return idxTX;
}

size_t RenderSystem::CreateTexture2D(const ScratchImage* resource, Samplers sampler)
{
	_ASEERTION_NULCHK(resource, "scratchImage is nullptr");
	Texture2D* pTexture2D = new Texture2D(m_pCDirect3D->GetDevice(), resource, sampler);
	_ASEERTION_NULCHK(pTexture2D, "TX is nullptr");
	m_pCTXs[m_lIdx_CTXs] = pTexture2D;
	return m_lIdx_CTXs++;
}

ID3DBlob* RenderSystem::CompileShader(std::wstring shaderName, std::string entryName, std::string target)
{
	ID3DBlob* pBlob;
	ID3DBlob* errBlob;
	HRESULT hResult;
	//compile Shader
	hResult = D3DCompileFromFile(shaderName.c_str(), nullptr, nullptr, entryName.c_str(), target.c_str(), NULL, NULL, &pBlob, &errBlob);
	if (FAILED(hResult))
	{
		OutputDebugStringA((char*)errBlob->GetBufferPointer());
		if (errBlob)
			errBlob->Release();
		_ASEERTION_CREATE(hResult, "CompileShader");
	}
	return pBlob;
}