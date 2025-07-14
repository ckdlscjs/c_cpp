#include "AppWindow.h"
#include "RenderSystem.h"
#include "InputSystem.h"
#include "TempObj.h"			//임시오브젝트

AppWindow::AppWindow()
{
	std::cout << "AppWindow" << " Class" << " 생성자 호출" << '\n';
}

AppWindow::~AppWindow()
{
	std::cout << "AppWindow" << " Class" << " 소멸자 호출" << '\n';
}

void AppWindow::OnCreate()
{
	std::cout << "OnCreate" << '\n';
	_RenderSystem.Init(m_hWnd, m_iWidth, m_iHeight);

	//렌더링을위한 정점객체
	Vertex_PC vertices[] =
	{
		{{-0.5f, 0.5f, -0.5f}		,{1, 0, 0, 1.0f}},
		{{0.5f, 0.5f, -0.5f}		,{0, 1, 0, 1.0f}},
		{{-0.5f, -0.5f, -0.5f}		,{0, 0, 1, 1.0f}},
		{{0.5f, -0.5f, -0.5f}		,{1, 1, 0, 1.0f}},
		{{-0.5f, 0.5f, 0.5f}		,{1, 0, 0, 1.0f}},
		{{0.5f, 0.5f, 0.5f}			,{0, 1, 0, 1.0f}},
		{{-0.5f, -0.5f, 0.5f}		,{0, 0, 1, 1.0f}},
		{{0.5f, -0.5f, 0.5f}		,{1, 1, 0, 1.0f}},
	};
	//인덱스버퍼의정점들
	UINT indices[] =
	{
		//front
		0,1,2,
		2,1,3,
		
		//right
		1,5,3,
		3,5,7,

		//back
		5,4,7,
		7,4,6,

		//left
		4,0,6,
		6,0,2,

		//up
		4,5,0,
		0,5,1,

		//bottom
		2,3,6,
		6,3,7,
	};
	//입력배치 객체 생성 및 초기화
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		//SEMANTIC NAME, SEMANTIC INDEX, FORMAT, INPUT SLOT, ALIGNED BYTE OFFSET, INPUT SLOT CLASS, INSTANCE DATA STEP RATE, 
		{"POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 0,	D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 12,	D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	_RenderSystem.objs.push_back(new TempObj());
	TempObj* obj = _RenderSystem.objs.back();
	size_t resourecidx = 0;
	_RenderSystem.m_pCVBs[resourecidx] = _RenderSystem.CreateVertexBuffer(vertices, sizeof(Vertex_PC), ARRAYSIZE(vertices));
	_RenderSystem.m_pCIBs[resourecidx] = _RenderSystem.CreateIndexBuffer(indices, ARRAYSIZE(indices));
	_RenderSystem.m_pCVSs[resourecidx] = _RenderSystem.CreateVertexShader(L"VertexShader.hlsl", "vsmain", "vs_5_0");
	_RenderSystem.m_pCPSs[resourecidx] = _RenderSystem.CreatePixelShader(L"PixelShader.hlsl", "psmain", "ps_5_0");
	_RenderSystem.m_pCILs[resourecidx] = _RenderSystem.CreateInputLayout(layout, ARRAYSIZE(layout));
	Constant_wvp cc0;
	_RenderSystem.m_pCCBs[resourecidx] = _RenderSystem.CreateConstantBuffer(&cc0, sizeof(Constant_wvp));
	Constant_time cc1;
	_RenderSystem.m_pCCBs[resourecidx+1] = _RenderSystem.CreateConstantBuffer(&cc1, sizeof(Constant_time));
	obj->m_IdxVB = obj->m_IdxIB = obj->m_IdxVS = obj->m_IdxPS = obj->m_IdxIL = resourecidx;
	obj->m_IdxCBs.push_back(resourecidx++); 
	obj->m_IdxCBs.push_back(resourecidx++);
}

void AppWindow::OnUpdate()
{
	std::cout << "OnUpdate" << '\n';
	_InputSystem.Frame();
	_RenderSystem.Frame();
	_RenderSystem.Render();
	std::cout << '\n';
}

void AppWindow::OnDestroy()
{
	std::cout << "OnDestroy" << '\n';
	_RenderSystem.Release();
	_InputSystem.Release();
	m_bIsRun = false;
}
