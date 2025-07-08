#include "AppWindow.h"
#include "RenderSystem.h"

AppWindow::AppWindow()
{
	std::cout << "AppWindow" << " Class" << " ������ ȣ��" << '\n';
}

AppWindow::~AppWindow()
{
	std::cout << "AppWindow" << " Class" << " �Ҹ��� ȣ��" << '\n';
}

void AppWindow::OnCreate()
{
	std::cout << "OnCreate" << '\n';
	_RenderSystem.Init(m_hWnd, m_iWidth, m_iHeight);

	//������������ ������ü
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
	//�ε���������������
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
	//�Է¹�ġ ��ü ���� �� �ʱ�ȭ
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		//SEMANTIC NAME, SEMANTIC INDEX, FORMAT, INPUT SLOT, ALIGNED BYTE OFFSET, INPUT SLOT CLASS, INSTANCE DATA STEP RATE, 
		{"POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 0,	D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 12,	D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	_RenderSystem.CreateVertexBuffer(vertices, sizeof(Vertex_PC), ARRAYSIZE(vertices));
	_RenderSystem.CreateIndexBuffer(indices, ARRAYSIZE(indices));
	_RenderSystem.CreateVertexShader(L"VertexShader.hlsl", "vsmain", "vs_5_0");
	_RenderSystem.CreatePixelShader(L"PixelShader.hlsl", "psmain", "ps_5_0");
	_RenderSystem.CreateInputLayout(layout, ARRAYSIZE(layout));
	_RenderSystem.CreateConstantBuffer();
}

void AppWindow::OnUpdate()
{
	std::cout << "OnUpdate" << '\n';
	_RenderSystem.Frame();
	_RenderSystem.Render();
}

void AppWindow::OnDestroy()
{
	std::cout << "OnDestroy" << '\n';
	//GraphicEngine::Get().Release();
	m_bIsRun = false;
}
