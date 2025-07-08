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

RenderSystem::RenderSystem()
{
	std::cout << "RenderSystem" << " Class" << " 생성자 호출" << '\n';
}

RenderSystem::~RenderSystem()
{
	std::cout << "RenderSystem" << " Class" << "소멸자 호출" << '\n';
	Release();
}

void RenderSystem::Init(HWND hWnd, UINT width, UINT height)
{
	m_fWidth = width;
	m_fHeight = height;
	m_pCDirect3D = new Direct3D();
	m_pCDirect3D->Init();

	m_pCSwapChain = new SwapChain();
	m_pCSwapChain->Init(m_pCDirect3D->GetDevice(), hWnd, width, height);
	
	m_pCDirect3D->SetViewportSize(m_fWidth, m_fHeight);
}

void RenderSystem::Frame()
{
	std::cout << "RenderSystem" << " Class" << " Frame 호출" << '\n';
	//RTV초기화
	m_pCSwapChain->ClearRenderTargetColor(m_pCDirect3D->GetDeviceContext(), 0, 0.3f, 0.4f, 1);

	m_pCInputLayout->SetInputLayout(m_pCDirect3D->GetDeviceContext());

	m_pCVertexBuffer->SetVertexBuffer(m_pCDirect3D->GetDeviceContext());

	m_pCIndexBuffer->SetIndexBuffer(m_pCDirect3D->GetDeviceContext());

	m_pCVertexShader->SetVertexShader(m_pCDirect3D->GetDeviceContext());
	m_pCPixelShader->SetPixelShader(m_pCDirect3D->GetDeviceContext());

	/*10장, 사각형의 시간경과에따른 형태변환을위한 행렬*/
	Constant_wvp cc0;
	Constant_time cc1;
	cc1.fTime = ::GetTickCount();
	m_fDeltaPos += m_fDeltatime * 0.15f;
	if (m_fDeltaPos > 1.0f) m_fDeltaPos = 0.0f;
	m_fDeltaScale += m_fDeltatime / 0.15f;
	//축소~확대, 선형보간
	XMMATRIX matScale = GetMat_Scale(XMFLOAT3(200.0f, 200.0f, 200.0f));

	//회전, 선형보간
	XMMATRIX matRotate = GetMat_RotRollPitchYaw(XMFLOAT3(m_fDeltaScale*360.0f, m_fDeltaScale * 360.0f, m_fDeltaScale * 360.0f));

	//이동, 선형보간
	XMMATRIX matTrans = GetMat_Translation(lerp(XMFLOAT3(-1.5f*400.0f, -1.5f*400.0f, 0), XMFLOAT3(1.5f*400.0f, 1.5f*400.0f, 0), m_fDeltaPos));
	
	//변환행렬, 뷰는 단위로 투영은 직교투영을사용한다
	cc0.matWorld = matScale * matRotate * matTrans;
	cc0.matView = GetMat_ViewMatrix(XMFLOAT3(200.0f, 300.0f, -1000.0f), XMFLOAT3(0.0f, 0.0f, 0.0f));
	//cc0.matProj = GetMat_Ortho(m_fWidth, m_fHeight, -4000.0f, 4000.0f);
	cc0.matProj = GetMat_Perspective(m_fWidth, m_fHeight, 75.0f, 0.1f, 4000.0f);

	//상수버퍼에 cc0(wvp mat), cc1(시간) 을 세팅한다
	m_pCConstantBuffers[0]->UpdateBufferData(m_pCDirect3D->GetDeviceContext(), &cc0);
	m_pCConstantBuffers[0]->SetVS(m_pCDirect3D->GetDeviceContext(), 0);
	//m_pCConstantBuffers[0]->SetPS(m_pCDirect3D->GetDeviceContext(), 0);

	m_pCConstantBuffers[1]->UpdateBufferData(m_pCDirect3D->GetDeviceContext(), &cc1);
	//m_pCConstantBuffers[1]->SetVS(m_pCDirect3D->GetDeviceContext(), 1);
	m_pCConstantBuffers[1]->SetPS(m_pCDirect3D->GetDeviceContext(), 1);
}

void RenderSystem::Render()
{
	std::cout << "RenderSystem" << " Class" << " Render 호출" << '\n';
	//m_pCDirect3D->DrawVertex_TriangleStrip(m_pCVertexBuffer->GetCountVertices(), 0);
	m_pCDirect3D->DrawIndex_TriagleList(m_pCIndexBuffer->GetCountIndices(), 0, 0);
	m_pCSwapChain->Present(false);

	m_lOldtime = m_lNewtime;
	m_lNewtime = ::GetTickCount();
	m_fDeltatime = (m_lOldtime) ? ((m_lNewtime - m_lOldtime) / 1000.0f) : 0;
	std::cout << m_fDeltaScale << '\n';
}

void RenderSystem::Release()
{
	if (m_pCInputLayout)
	{
		delete m_pCInputLayout;
		m_pCInputLayout = nullptr;
	}
	
	if (m_pCVertexBuffer)
	{
		delete m_pCVertexBuffer;
		m_pCVertexBuffer = nullptr;
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
	if (m_pCVertexShader)
	{
		delete m_pCVertexShader;
		m_pCVertexShader = nullptr;
	}
	
	if (m_pBlob_PS)
	{
		m_pBlob_PS->Release();
	}
	if (m_pCPixelShader)
	{
		delete m_pCPixelShader;
		m_pCPixelShader = nullptr;
	}
	
	if (m_pCConstantBuffers.size())
	{
		for (int i = 0; i < m_pCConstantBuffers.size(); i++)
			delete m_pCConstantBuffers[i];
	}

	if (m_pCIndexBuffer)
	{
		delete m_pCIndexBuffer;
		m_pCIndexBuffer = nullptr;
	}
}

VertexBuffer* RenderSystem::CreateVertexBuffer(void* vertices, UINT size_vertex, UINT size_vertices)
{
	m_pCVertexBuffer = new VertexBuffer();
	m_pCVertexBuffer->Init(m_pCDirect3D->GetDevice(), vertices, size_vertex, size_vertices);
	return m_pCVertexBuffer;
}

InputLayout* RenderSystem::CreateInputLayout(D3D11_INPUT_ELEMENT_DESC* pInputElementDescs, UINT size_layout)
{
	m_pCInputLayout = new InputLayout();
	m_pCInputLayout->Init(m_pCDirect3D->GetDevice(), pInputElementDescs, size_layout, m_pBlob_VS->GetBufferPointer(), m_pBlob_VS->GetBufferSize());
	return m_pCInputLayout;
}

IndexBuffer* RenderSystem::CreateIndexBuffer(void* indices, UINT size_indices)
{
	m_pCIndexBuffer = new IndexBuffer();
	m_pCIndexBuffer->Init(m_pCDirect3D->GetDevice(), indices, size_indices);
	return m_pCIndexBuffer;
}

ConstantBuffer* RenderSystem::CreateConstantBuffer()
{
	Constant_wvp cc0;
	m_pCConstantBuffers.push_back(new ConstantBuffer());
	m_pCConstantBuffers.back()->Init(m_pCDirect3D->GetDevice(), &cc0, sizeof(Constant_wvp));

	Constant_time cc1;
	cc1.fTime = 0;
	m_pCConstantBuffers.push_back(new ConstantBuffer());
	m_pCConstantBuffers.back()->Init(m_pCDirect3D->GetDevice(), &cc1, sizeof(Constant_time));
	return nullptr;
}

VertexShader* RenderSystem::CreateVertexShader(std::wstring shaderName, std::string entryName, std::string target)
{
	if(m_pBlob_VS)
		m_pBlob_VS->Release();
	m_pBlob_VS = CompileShader(shaderName, entryName, target);
	m_pCVertexShader = new VertexShader();
	m_pCVertexShader->Init(m_pCDirect3D->GetDevice(), m_pBlob_VS);
	return m_pCVertexShader;
}

PixelShader* RenderSystem::CreatePixelShader(std::wstring shaderName, std::string entryName, std::string target)
{
	if(m_pBlob_PS)
		m_pBlob_PS->Release();
	m_pBlob_PS = CompileShader(shaderName, entryName, target);
	m_pCPixelShader = new PixelShader();
	m_pCPixelShader->Init(m_pCDirect3D->GetDevice(), m_pBlob_PS);
	return m_pCPixelShader;
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


//싱글톤객체, static선언으로 컴파일타임에 객체가 생성된다
RenderSystem& RenderSystem::Get()
{
	static RenderSystem engine;
	return engine;
}
