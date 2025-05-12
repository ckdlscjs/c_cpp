#include "GraphicEngine.h"
#include "Window.h"
#include "Direct3D.h"
#include "SwapChain.h"
#include "VertexBuffer.h"
#include "InputLayout.h"
#include "VertexShader.h"

GraphicEngine::GraphicEngine()
{
	std::cout << "GraphicEngine" << " Class" << " 생성자 호출" << '\n';
}

GraphicEngine::~GraphicEngine()
{
	std::cout << "GraphicEngine" << " Class" << "소멸자 호출" << '\n';
	Release();
}

void GraphicEngine::Init(Window* pWindow)
{
	m_pCDirect3D = new Direct3D();
	m_pCDirect3D->Init();

	m_pCSwapChain = new SwapChain();
	RECT rect = pWindow->GetClientWindowRect();
	m_pCSwapChain->Init(m_pCDirect3D->GetDevice(), pWindow->GetHwnd(), rect.right - rect.left, rect.bottom - rect.top);
	
	//렌더링을위한 정점객체 세팅및 VertexBuffer생성
	XMFLOAT3 vertices[] =
	{
		{-0.5f, -0.5f, 0.0f},
		{-0.5f, 0.5f, 0.0f},
		{0.5f, -0.5, 0.0f},
		{0.5f, 0.5f, 0.0f},
	};
	UINT size_vertices = ARRAYSIZE(vertices);
	m_pCVertexBuffer = new VertexBuffer();
	m_pCVertexBuffer->Init(m_pCDirect3D->GetDevice(), vertices, sizeof(XMFLOAT3), size_vertices);

	//compile vs

	m_pCVertexShader = new VertexShader();
	m_pBlob_VS = CompileShader(L"VertexShader.hlsl", "vsmain", "vs_5_0");
	m_pCVertexShader->Init(m_pCDirect3D->GetDevice(), m_pBlob_VS);
	//compile ps
	m_pBlob_PS = CompileShader(L"PixelShader.hlsl", "psmain", "ps_5_0");
	HRESULT hResult;
	//create ps
	hResult = m_pCDirect3D->GetDevice()->CreatePixelShader(m_pBlob_PS->GetBufferPointer(), m_pBlob_PS->GetBufferSize(), nullptr, &m_pPS);
	_ASEERTION_CREATE(hResult, "PixelShader");

	//입력배치 객체 생성 및 초기화
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		//SEMANTIC NAME, SEMANTIC INDEX, FORMAT, INPUT SLOT, ALIGNED BYTE OFFSET, INPUT SLOT CLASS, INSTANCE DATA STEP RATE, 
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	m_pCInputLayout = new InputLayout();
	m_pCInputLayout->Init(m_pCDirect3D->GetDevice(), layout, ARRAYSIZE(layout), m_pBlob_VS->GetBufferPointer(), m_pBlob_VS->GetBufferSize());

	//뷰포트세팅
	m_pCDirect3D->SetViewportSize(rect.right - rect.left, rect.bottom - rect.top);
}

void GraphicEngine::Frame()
{
	std::cout << "GraphicEngine" << " Class" << " Frame 호출" << '\n';
	//렌더타겟 초기화 및 출력병합기에 렌더타겟뷰 세팅
	m_pCSwapChain->ClearRenderTargetColor(m_pCDirect3D->GetDeviceContext(), 0, 0.3f, 0.4f, 1);

	m_pCInputLayout->SetInputLayout(m_pCDirect3D->GetDeviceContext());

	m_pCVertexBuffer->SetVertexBuffer(m_pCDirect3D->GetDeviceContext());

	m_pCVertexShader->SetVertexShader(m_pCDirect3D->GetDeviceContext());
	m_pCDirect3D->GetDeviceContext()->PSSetShader(m_pPS, nullptr, 0);
}

void GraphicEngine::Render()
{
	std::cout << "GraphicEngine" << " Class" << " Render 호출" << '\n';
	m_pCDirect3D->DrawVertex_TriangleStrip(m_pCVertexBuffer->GetCountVertices(), 0);
	m_pCSwapChain->Present(false);
}

void GraphicEngine::Release()
{
	delete m_pCInputLayout;
	m_pCInputLayout = nullptr;
	
	delete m_pCVertexBuffer;
	m_pCVertexBuffer = nullptr;

	delete m_pCSwapChain;
	m_pCSwapChain = nullptr;
	
	delete m_pCDirect3D;
	m_pCDirect3D = nullptr;

	delete m_pCVertexShader;
	m_pCVertexShader = nullptr;

	m_pBlob_VS->Release();
	m_pBlob_PS->Release();
	m_pPS->Release();
}

ID3DBlob* GraphicEngine::CompileShader(std::wstring shaderName, std::string entryName, std::string target)
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
GraphicEngine& GraphicEngine::Get()
{
	static GraphicEngine engine;
	return engine;
}
