#include "RenderSystem.h"
#include "Window.h"
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

RenderSystem::RenderSystem()
{
	std::cout << "RenderSystem" << " Class" << " 생성자 호출" << '\n';
}

RenderSystem::~RenderSystem()
{
	std::cout << "RenderSystem" << " Class" << "소멸자 호출" << '\n';
}

void RenderSystem::Init(HWND hWnd, UINT width, UINT height)
{
	m_iWidth = width;
	m_iHeight = height;
	m_pCDirect3D = new Direct3D();
	m_pCDirect3D->Init();

	m_pCSwapChain = new SwapChain();
	m_pCSwapChain->Init(m_pCDirect3D->GetDevice(), hWnd, width, height);
	
	m_pCDirect3D->SetViewportSize(m_iWidth, m_iHeight);
}

void RenderSystem::Frame()
{
	std::cout << "RenderSystem" << " Class" << " Frame 호출" << '\n';
	std::cout << "ElapsedTime : " << m_fElapsedtime << '\n';
	//RTV초기화
	m_pCSwapChain->ClearRenderTargetColor(m_pCDirect3D->GetDeviceContext(), 0, 0.3f, 0.4f, 1);

	//프레임에따른 변환
	//XMMATRIX matView = GetMat_ViewMatrix(XMFLOAT3(-300.0f, 500.0f, -1000.0f), XMFLOAT3(0.0f, 0.0f, 0.0f));
	XMMATRIX matView = _CameraSystem.GetCamera(0)->GetViewMatrix();

	//cc0.matProj = GetMat_Ortho(m_fWidth, m_fHeight, -4000.0f, 4000.0f);
	XMMATRIX matProj = GetMat_Perspective((float)m_iWidth, (float)m_iHeight, 75.0f, 0.1f, 4000.0f);
	for (const auto& iter : objs)
	{
		iter->Frame(m_fDeltatime);
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
		cc1.fTime = _DEGTORAD(m_fElapsedtime*360.0f);
		m_pCCBs[iter->m_IdxCBs[1]]->UpdateBufferData(m_pCDirect3D->GetDeviceContext(), &cc1);
		m_pCCBs[iter->m_IdxCBs[1]]->SetPS(m_pCDirect3D->GetDeviceContext(), 1);
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

void RenderSystem::Render()
{
	std::cout << "RenderSystem" << " Class" << " Render 호출" << '\n';

	for (const auto& iter : objs)
	{
		//m_pCDirect3D->DrawVertex_TriangleStrip(m_pCVertexBuffer->GetCountVertices(), 0);
		m_pCDirect3D->DrawIndex_TriagleList(m_pCIBs[iter->m_IdxIB]->GetCountIndices(), 0, 0);
	}
	
	m_pCSwapChain->Present(false);
}

void RenderSystem::Release()
{
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

	for (auto iter = objs.begin(); iter != objs.end();)
	{
		delete *iter;
		iter = objs.erase(iter);
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
	}
	if (m_pBlob_PS)
	{
		m_pBlob_PS->Release();
	}
}

VertexBuffer* RenderSystem::CreateVertexBuffer(void* vertices, UINT size_vertex, UINT size_vertices)
{
	VertexBuffer* pVertexBuffer = new VertexBuffer();
	pVertexBuffer->Init(m_pCDirect3D->GetDevice(), vertices, size_vertex, size_vertices);
	return pVertexBuffer;
}

InputLayout* RenderSystem::CreateInputLayout(D3D11_INPUT_ELEMENT_DESC* pInputElementDescs, UINT size_layout)
{
	InputLayout* pInputLayout = new InputLayout();
	pInputLayout->Init(m_pCDirect3D->GetDevice(), pInputElementDescs, size_layout, m_pBlob_VS->GetBufferPointer(), m_pBlob_VS->GetBufferSize());
	return pInputLayout;
}

IndexBuffer* RenderSystem::CreateIndexBuffer(void* indices, UINT size_indices)
{
	IndexBuffer* pIndexBuffer = new IndexBuffer();
	pIndexBuffer->Init(m_pCDirect3D->GetDevice(), indices, size_indices);
	return pIndexBuffer;
}

ConstantBuffer* RenderSystem::CreateConstantBuffer(void* data, UINT size_buffer)
{
	ConstantBuffer* pConstantBuffer = new ConstantBuffer();
	pConstantBuffer->Init(m_pCDirect3D->GetDevice(), data, size_buffer);
	return pConstantBuffer;
}

VertexShader* RenderSystem::CreateVertexShader(std::wstring shaderName, std::string entryName, std::string target)
{
	if(m_pBlob_VS)
		m_pBlob_VS->Release();
	m_pBlob_VS = CompileShader(shaderName, entryName, target);
	VertexShader* pVertexShader = new VertexShader();
	pVertexShader->Init(m_pCDirect3D->GetDevice(), m_pBlob_VS);
	return pVertexShader;
}

PixelShader* RenderSystem::CreatePixelShader(std::wstring shaderName, std::string entryName, std::string target)
{
	if(m_pBlob_PS)
		m_pBlob_PS->Release();
	m_pBlob_PS = CompileShader(shaderName, entryName, target);
	PixelShader* pPixelShader = new PixelShader();
	pPixelShader->Init(m_pCDirect3D->GetDevice(), m_pBlob_PS);
	return pPixelShader;
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