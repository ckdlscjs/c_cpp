#include "stdafx.h"
#include "SystemClass.h"
#include "InputClass.h"
#include "GraphicsClass.h"

SystemClass::SystemClass()
{
	
}

SystemClass::SystemClass(const SystemClass& _systemClass)
{
}

SystemClass::~SystemClass()
{
}

bool SystemClass::Initialize()
{
	//윈도우의 가로, 세로 변수 초기화
	int screenWidth = 0;
	int screenHeight = 0;

	//윈도우 생성, 초기화 및 띄우기(ShowWindow함수)
	InitializeWindows(screenWidth, screenHeight);

	//입력클래스 객체 생성 및 초기화
	m_Input = new InputClass;
	if (!m_Input) return false;
	m_Input->Initialize();

	//그래픽클래스 객체 생성 및 초기화
	m_Graphics = new GraphicsClass;
	if (!m_Graphics) return false;
	
	return m_Graphics->Initialize(screenWidth, screenHeight, m_hwnd);
}

void SystemClass::Shutdown()
{
	// m_Graphics 객체 반환
	if (m_Graphics)
	{
		m_Graphics->Shutdown();
		delete m_Graphics;
		m_Graphics = nullptr;
	}

	// m_Input 객체 반환
	if (m_Input)
	{
		delete m_Input;
		m_Input = nullptr;
	}

	// Window 종료 처리
	ShutdownWindows();
}

void SystemClass::Run()
{
	// 메시지 구조체 생성 및 초기화
	MSG msg;
	memset(&msg, 0, sizeof(MSG));

	// 종료메시지를 받을 때 까지 메시지를 루프한다
	while (true)
	{
		// 윈도우 메시지를 처리한다
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			// 메시지가 종료메시지일경우 종료한다
			if (msg.message == WM_QUIT) break;
			// 윈도우 메시지 큐에 데이터가 WM_QUIT이외의 다른 입력등이 들어왔을때 수행한다
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// 메시지인터럽트가 발생하지 않으면 frame을 수행, 즉 메시지 큐가 없을때 수행된다(게임로직, 충돌, 렌더링 등..)
			if (!Frame()) break;
		}
	}
}

LRESULT SystemClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch (umsg)
	{
		// 키보드가 눌렸다면
		case WM_KEYDOWN :
		{
			m_Input->KeyDown(wparam);
			return 0;
		}
		
		// 키보드가 떨어졌는가 처리
		case WM_KEYUP:
		{
			m_Input->KeyUp(wparam);
			return 0;
		}

		//이외의 메시지는 기본메시지 처리로 넘긴다
		default :
		{
			return DefWindowProc(hwnd, umsg, wparam, lparam);
		}
	}
}

bool SystemClass::Frame()
{
	//ESC가 눌리면 종료
	if(m_Input->IsKeyDown(VK_ESCAPE))
		return false;
	return m_Graphics->Frame();
}

void SystemClass::InitializeWindows(int& _screenWidth, int& _screenHeight)
{
	// 정적객체 핸들을 현재 객체로 지정한다
	ApplicationHandle = this;

	// 해당 프로그램의 인스턴스를 가져와 지정한다
	m_hInstance = GetModuleHandle(NULL);

	// 해당 프로그램의 이름을 지정한다
	m_applicationName = L"RastekTek02";

	// windows 클래스를 설정한다
	WNDCLASSEX wc;
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;				// 창 스타일, 수평/수직 리사이즈 시 다시 그리기, 고유 DC 사용
	wc.lpfnWndProc = WndProc;									// 윈도우 메시지를 처리할 함수 지정, 정적전역함수 WndProc을 사용
	wc.cbClsExtra = 0;											// 클래스 메모리와 윈도우 메모리의 추가 공간 (사용 안 함)
	wc.cbWndExtra = 0;
	wc.hInstance = m_hInstance;									// 현재 실행 중인 인스턴스 핸들
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);						// 아이콘 설정 (윈도우 왼쪽 상단 및 Alt+Tab 아이콘)
	wc.hIconSm = wc.hIcon;				
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);					// 기본 마우스 커서 설정 (화살표)
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);		// 창의 배경 브러시 (검정색)
	wc.lpszMenuName = NULL;										// 메뉴 리소스 이름 (없음)
	wc.lpszClassName = m_applicationName;						// 윈도우 클래스 이름 (등록 시 사용)
	wc.cbSize = sizeof(WNDCLASSEX);								// 구조체의크기설정

	// windows class를 등록합니다
	RegisterClassEx(&wc);

	// 모니터 화면의 해상도를 읽어와 참조로 넘긴 변수에 사이즈를 넘겨준다
	_screenWidth = GetSystemMetrics(SM_CXSCREEN);
	_screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// 윈도우의 시작 위치(왼쪽위)
	int posX = 0;
	int posY = 0;

	// GraphicsClass에서 전역변수로 선언되어있는 FULL_SCREEN변수의 상태에 따라 화면을 설정한다
	if (FULL_SCREEN)
	{
		// 전체화면 상태라면 창 사이즈를 데스크톱 해상도로 지정하고 색상을 32BIT으로 지정한다
		DEVMODE dmScreenSettings;
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));						//same zeromemory
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);							//구조체의 크기설정
		dmScreenSettings.dmPelsWidth = (unsigned long)_screenWidth;			
		dmScreenSettings.dmPelsHeight = (unsigned long)_screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);
	}
	else
	{
		// 전체화면 상태가 아닐경우 800*600해상도로 지정해준다
		_screenWidth = 800;
		_screenHeight = 600;

		// 윈도우가 가운데에 올 수 있도록 윈도우의 좌측위 위치를 창 크기(800, 600)에 비례해 좌측상단위치를조정한다
		posX = (GetSystemMetrics(SM_CXSCREEN) - _screenWidth) / 2;	//(1920 - 800) / 2; 
		posY = (GetSystemMetrics(SM_CYSCREEN) - _screenHeight) / 2; //(1080 - 600) / 2;

		// 윈도우객체를 생성하고 핸들을 구한다
		m_hwnd = CreateWindowEx
		(
			WS_EX_APPWINDOW,									// 확장 스타일
			m_applicationName,									// 등록된 클래스 이름
			m_applicationName,									// 윈도우 이름
			WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,		// 윈도우 스타일
			posX,												//좌측 상단
			posY, 
			_screenWidth,										// 창 크기
			_screenHeight, 
			NULL,												// 부모 윈도우 핸들(없으면 null)
			NULL,												// 메뉴 핸들(없으면 null)
			m_hInstance,										// 현재 어플리케이션 인스턴스 핸들
			NULL												// 추가 데이터(일반적으로 null)
		);

		// 윈도우를 화면에 표시하고 포커스를 지정합니다
		ShowWindow(m_hwnd, SW_SHOW);							// 윈도우를 생성하고 넘겨받은 핸들(윈도우객체)를 띄운다
		SetForegroundWindow(m_hwnd);
		SetFocus(m_hwnd);
	}
}

void SystemClass::ShutdownWindows()
{
	// 전체화면 모드였을때 설정을 초기화시킨다
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	// 띄운 창을 제거한다
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	// 프로그램 인스턴스를 제거한다
	UnregisterClass(m_applicationName, m_hInstance);
	m_hInstance = NULL;

	// 정적객체의 포인터 참조를 초기화한다
	ApplicationHandle = NULL;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch(umessage)
	{
		// 윈도우 종료를 확인합니다
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}

		// 윈도우가 닫히는지 확인합니다
		case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		}

		// 그 외의 모든 메시지들은 시스템 클래스의 메시지 처리로 넘깁니다.
		default:
		{
			return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
		}
	}
}
