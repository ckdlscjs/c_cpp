#include "AppWindow.h"
#include "RenderSystem.h"
#include "InputSystem.h"
#include "TempObj.h"			//임시오브젝트
#include "CameraSystem.h"
#include "FirstPersonCamera.h"
#include "ResourceSystem.h"
#include "ImguiSystem.h"


AppWindow::AppWindow()
{
	std::cout << "Initialize : " << "AppWindow" << " Class" << '\n';
	_InputSystem;
	_ResourceSystem;
	_RenderSystem;
	_ImguiSystem;
	_CameraSystem;
}

AppWindow::~AppWindow()
{
	std::cout << "Release : " << "AppWindow" << " Class" << '\n';
}

void AppWindow::OnCreate()
{
	std::cout << "OnCreate" << '\n';
	_InputSystem.Init();
	_ResourceSystem.Init();
	_RenderSystem.Init(m_hWnd, m_iWidth, m_iHeight);
	_ImguiSystem.Init(m_hWnd, _RenderSystem.GetD3DDevice(), _RenderSystem.GetD3DDeviceContext());
	_CameraSystem.Init();

	_CameraSystem.AddCamera(new FirstPersonCamera());
	_CameraSystem.GetCamera(0)->SetPosition({ 0.0f, 500.0f, -1000.0f });
	//렌더링을위한 정점객체
	/*
	* 0 1  4 5
	* 2 3  6 7
	*/
	XMFLOAT3 posList[] =
	{
		{-0.5f	, 0.5f	, -0.5f	},		
		{0.5f	, 0.5f	, -0.5f	},
		{-0.5f	, -0.5f	, -0.5f	},
		{0.5f	, -0.5f	, -0.5f	},
		{-0.5f	, 0.5f	, 0.5f	},
		{0.5f	, 0.5f	, 0.5f	},
		{-0.5f	, -0.5f	, 0.5f	},
		{0.5f	, -0.5f	, 0.5f	},
	};
	XMFLOAT4 colorList[] =
	{
		{1, 0, 0, 1.0f},
		{0, 1, 0, 1.0f},
		{0, 0, 1, 1.0f},
		{1, 1, 0, 1.0f},
		{1, 0, 0, 1.0f},
		{0, 1, 0, 1.0f},
		{0, 0, 1, 1.0f},
		{1, 1, 0, 1.0f},
	};
	XMFLOAT2 texcoordList[]
	{
		{0.0f, 0.0f},
		{1.0f, 0.0f},
		{0.0f, 1.0f},
		{1.0f, 1.0f},
	};
	Vertex_PCT vertices[] =
	{
		//Front
		{posList[0], colorList[0], texcoordList[0]},	//0
		{posList[1], colorList[1], texcoordList[1]},	//1
		{posList[2], colorList[2], texcoordList[2]},	//2
		{posList[3], colorList[3], texcoordList[3]},	//3

		//Right
		{posList[1], colorList[1], texcoordList[0]},	//4
		{posList[5], colorList[5], texcoordList[1]},	//5
		{posList[3], colorList[3], texcoordList[2]},	//6
		{posList[7], colorList[7], texcoordList[3]},	//7

		//Rear
		{posList[5], colorList[5], texcoordList[0]},	//8
		{posList[4], colorList[4], texcoordList[1]},	//9
		{posList[7], colorList[7], texcoordList[2]},	//10
		{posList[6], colorList[6], texcoordList[3]},	//11

		//Left
		{posList[4], colorList[4], texcoordList[0]},	//12
		{posList[0], colorList[0], texcoordList[1]},	//13
		{posList[6], colorList[6], texcoordList[2]},	//14
		{posList[2], colorList[2], texcoordList[3]},	//15

		//Up
		{posList[4], colorList[4], texcoordList[0]},	//16
		{posList[5], colorList[5], texcoordList[1]},	//17
		{posList[0], colorList[0], texcoordList[2]},	//18
		{posList[1], colorList[1], texcoordList[3]},	//19

		//Bottom
		{posList[2], colorList[2], texcoordList[0]},	//20
		{posList[3], colorList[3], texcoordList[1]},	//21
		{posList[6], colorList[6], texcoordList[2]},	//22
		{posList[7], colorList[7], texcoordList[3]},	//23
	};
	//인덱스버퍼의정점들
	UINT indices[] =
	{
		//front
		0,1,2,
		2,1,3,
		
		//right
		4,5,6,
		6,5,7,

		//back
		8,9,10,
		10,9,11,

		//left
		12,13,14,
		14,13,15,

		//up
		16,17,18,
		18,17,19,

		//bottom
		20,21,22,
		22,21,23,
	};
	//입력배치 객체 생성 및 초기화
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		//SEMANTIC NAME, SEMANTIC INDEX, FORMAT, INPUT SLOT, ALIGNED BYTE OFFSET, INPUT SLOT CLASS, INSTANCE DATA STEP RATE, 
		{"POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 0,	D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 12,	D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,		0, 28,	D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	_RenderSystem.objs.push_back(new TempObj());
	TempObj* obj = _RenderSystem.objs.back();
	obj->m_IdxVB = _RenderSystem.CreateVertexBuffer(vertices, sizeof(Vertex_PCT), ARRAYSIZE(vertices));
	obj->m_IdxIB = _RenderSystem.CreateIndexBuffer(indices, ARRAYSIZE(indices));
	obj->m_IdxVS = _RenderSystem.CreateVertexShader(L"VertexShader.hlsl", "vsmain", "vs_5_0");
	obj->m_IdxPS = _RenderSystem.CreatePixelShader(L"PixelShader.hlsl", "psmain", "ps_5_0");
	obj->m_IdxIL = _RenderSystem.CreateInputLayout(layout, ARRAYSIZE(layout));
	Constant_wvp cc0;
	obj->m_IdxCBs.push_back(_RenderSystem.CreateConstantBuffer(&cc0, sizeof(Constant_wvp)));
	Constant_time cc1;
	obj->m_IdxCBs.push_back(_RenderSystem.CreateConstantBuffer(&cc1, sizeof(Constant_time)));

	obj->m_IdxTX = _RenderSystem.CreateTexture(L"../Assets/Textures/butter.dds", Samplers::WRAP_LINEAR);
}

void AppWindow::OnUpdate()
{
	std::cout << "OnUpdate" << '\n';
	_InputSystem.Frame();
	_ImguiSystem.Frame();
	_CameraSystem.Frame(0.033f);
	_RenderSystem.Frame();

	_RenderSystem.PreRender();
	_RenderSystem.Render();
	_ImguiSystem.Render();
	_RenderSystem.PostRender();
	std::cout << '\n';
}

void AppWindow::OnDestroy()
{
	std::cout << "OnDestroy" << '\n';
	m_bIsRun = false;
	_ResourceSystem.Release();
	_CameraSystem.Release();
	_RenderSystem.Release();
	_InputSystem.Release();
	_ImguiSystem.Release();
}
