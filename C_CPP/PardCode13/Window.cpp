#include "Window.h"
#include "InputSystem.h"

Window* g_pWindow = nullptr;
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	/*
	WPARAM wParam (Word Parameter):
	타입: WPARAM (Word Parameter)
	설명: 메시지에 특정한 추가 정보를 담는 매개변수입니다. "Word"는 원래 16비트 데이터를 의미했지만, 현재는 시스템 아키텍처에 따라 32비트 또는 64비트 정수 값을 가질 수 있습니다 (보통 UINT_PTR로 정의됨).
	용도: 어떤 메시지냐에 따라 그 의미가 달라집니다.

	키보드 메시지 (WM_KEYDOWN, WM_KEYUP): 눌리거나 떼어진 키의 가상 키 코드 (Virtual Key Code)를 담습니다 (예: VK_SPACE, VK_A).
	마우스 메시지 (WM_MOUSEMOVE): 현재 눌려 있는 마우스 버튼 및 Shift/Ctrl 키의 상태를 비트 플래그로 담습니다 (예: MK_LBUTTON, MK_SHIFT).
	윈도우 크기 변경 메시지 (WM_SIZE): 윈도우가 어떻게 크기 변경되었는지 (최소화, 최대화, 일반 크기)에 대한 정보를 담습니다.
	마우스 휠 메시지 (WM_MOUSEWHEEL): 마우스 휠의 회전량(델타)을 담습니다.

	LPARAM lParam (Long Parameter):
	타입: LPARAM (Long Parameter)
	설명: wParam과 마찬가지로 메시지에 특정한 추가 정보를 담는 매개변수입니다. "Long"은 원래 32비트 데이터를 의미했지만, 현재는 시스템 아키텍처에 따라 32비트 또는 64비트 정수 값을 가질 수 있습니다 (보통 LONG_PTR로 정의됨).
	용도: wParam과 마찬가지로 메시지에 따라 그 의미가 달라집니다.

	키보드 메시지: 키의 반복 횟수, 스캔 코드, 확장 키 플래그 등 더 상세한 키보드 상태 정보를 비트 필드로 담습니다.
	마우스 메시지: 마우스 커서의 X, Y 좌표를 담습니다. LOWORD(lParam)으로 X좌표, HIWORD(lParam)으로 Y좌표를 얻습니다. (일부 메시지는 클라이언트 좌표, 일부는 화면 좌표).
	윈도우 크기 변경 메시지: 변경된 윈도우의 새로운 너비와 높이를 담습니다.
	*/
	switch (msg)
	{
		//윈도우생성시
		case WM_CREATE:
		{
			//Event When WindowCreated
			//g_pWindow->SetHWND(hWnd);
			//g_pWindow->OnCreate();
		}break;

		//윈도우소멸시
		case WM_DESTROY:
		{
			//Event When WindowDestroyed
			g_pWindow->Release();
			::PostQuitMessage(0);
		}break;

		//키보드관련
		case WM_KEYDOWN:
		{
			_InputSystem.OnKeyDown(wParam);
		}break;

		case WM_KEYUP:
		{
			_InputSystem.OnKeyUp(wParam);
		}break;

		//마우스관련
		case WM_MOUSEMOVE:
		{
			//좌상단0,0~우하단증가
			_InputSystem.OnMouseMove(LOWORD(lParam), HIWORD(lParam));
		}break;

		/*
		case WM_LBUTTONDOWN:
		{
			std::cout << "btn";
		}break;

		case WM_LBUTTONUP:
		{
			std::cout << "btn";
		}break;

		case WM_RBUTTONDOWN:
		{
			std::cout << "btn";
		}break;

		case WM_RBUTTONUP:
		{
			std::cout << "btn";
		}break;
		
		case WM_SETFOCUS:
		{
		}break;
		case WM_KILLFOCUS:
		{
		}break;
		*/
		default:
		{
			return ::DefWindowProc(hWnd, msg, wParam, lParam);
		}
	}
}

Window::Window()
{
	std::cout << "Window" << " Class" << " 생성자 호출" << '\n';
}

Window::~Window()
{
	std::cout << "Window" << " Class" << "소멸자 호출" << '\n';
}

bool Window::Init()
{

	//전역변수 gWindow에 현재 객체를 지정한다, 해당 전역변수는 WndProc에서 호출되므로 순서상 가장먼저 초기화시켜준다
	if (!g_pWindow)
		g_pWindow = this;
	HINSTANCE hInstance = GetModuleHandle(NULL);
	//Window를 띄우기위한 WNDCLASSEX구조체의 초기화
	WNDCLASSEX descWnd;
	ZeroMemory(&descWnd, sizeof(WNDCLASSEX));
	descWnd.cbSize = sizeof(WNDCLASSEX);					//해당구조체의 크기
	descWnd.style = NULL;									//클래스스타일
	descWnd.lpfnWndProc = &WndProc;							//창 프로시저에대한 포인터, 해당함수를 통해서 윈도우 창에대한 입력을 받음
	descWnd.cbClsExtra = NULL;								//클래스에 할당할 추가 바이트 기본은 0
	descWnd.cbWndExtra = NULL;								//윈도우에 할당할 추가 바이트 기본은 0
	descWnd.hInstance = hInstance;							//해당 인스턴스에 대한 핸들
	descWnd.hIcon = LoadIcon(NULL, IDI_APPLICATION);		//클래스 아이콘, 1)인스턴스핸들, 2)해당멤버가 NULL이면 시스템에서 기본 아이콘을 제공
	descWnd.hCursor = LoadCursor(NULL, IDC_ARROW);			//마우스 포인터, 1)인스턴스핸들, 2)해당멤버가 NULL이면 시스템에서 기본 아이콘을 제공
	descWnd.hIconSm = LoadIcon(NULL, IDI_APPLICATION);		//스몰 아이콘, 1)인스턴스핸들, 2)해당멤버가 NULL이면 시스템에서 기본 아이콘을 제공
	descWnd.hbrBackground = (HBRUSH)COLOR_WINDOW;			//클래스 배경 브러시에 대한 핸들
	descWnd.lpszMenuName = L"";								//클래스 메뉴의 리소스이름을 지정, NULL일시 해당클래스에는 기본메뉴가없음
	descWnd.lpszClassName = L"PardCode";					//창 클래스이름을 지정
	
	//WNDCLASSEX 구조체 설정이후 윈도우를 등록한다, 패러미터에 윈도우 구조체의 주소를 넘긴다, 해당 함수 호출이후부터 WndProc가 운영체제에의해서 계속 호출된다
	if (!::RegisterClassEx(&descWnd))
		return false;

	//윈도우 창을 만들고 핸들을 멤버변수 mHwnd에 넘긴다
	//CreateWindowExW(0L, lpClassName, lpWindowName, dwStyle, x, y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam)
	m_hWnd = ::CreateWindowEx
	(
		WS_EX_OVERLAPPEDWINDOW,
		L"PardCode",										//위의 창 클래스 이름과 꼭 같아야한다
		L"WIN32API WINDOW",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		1024,
		768,
		NULL,
		NULL,
		hInstance,
		NULL
	);
	if (!m_hWnd)
		return false;
	//윈도우창을띄운다
	::ShowWindow(m_hWnd, SW_SHOW);
	::UpdateWindow(m_hWnd);
	::SetFocus(m_hWnd);
	RECT rect = GetClientWindowRect();
	m_iWidth = rect.right - rect.left;
	m_iHeight = rect.bottom - rect.top;
	g_pWindow->OnCreate();
	return m_bIsRun = true;
}

bool Window::BroadCast()
{
	MSG msg;
	memset(&msg, 0, sizeof(MSG));
	if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	else
	{
		OnUpdate();
		Sleep(0);
	}
	
	return true;
}

bool Window::Release()
{
	g_pWindow->OnDestroy();
	return ::DestroyWindow(m_hWnd);
}

bool Window::IsRun()
{
	return m_bIsRun;
}

void Window::SetHWND(HWND hWnd)
{
	m_hWnd = hWnd;
}

HWND Window::GetHwnd() const
{
	return m_hWnd;
}

RECT Window::GetClientWindowRect()
{
	RECT rt;
	::GetClientRect(m_hWnd, &rt);
	return rt;
}
