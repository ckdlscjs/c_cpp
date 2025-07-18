#include "Window.h"
#include "InputSystem.h"

Window* g_pWindow = nullptr;
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
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

		
		case WM_KEYDOWN:
		{
			_InputSystem.OnKeyDown(wParam);
		}break;

		case WM_KEYUP:
		{
			_InputSystem.OnKeyUp(wParam);
		}break;
		
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
