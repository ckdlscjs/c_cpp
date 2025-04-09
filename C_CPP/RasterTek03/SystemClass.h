#pragma once

//사용할 클래스 전방선언
class InputClass;
class GraphicsClass;

//SystemClass
class SystemClass
{
public:
	SystemClass();
	SystemClass(const SystemClass&);
	~SystemClass();

	bool Initialize();				//초기화
	void Shutdown();				//종료시
	void Run();						//프로그램반복수행
	
	LRESULT CALLBACK MessageHandler(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam); 

private:
	bool Frame();
	void InitializeWindows(int&, int&);
	void ShutdownWindows();

private:
	LPCWSTR m_applicationName;
	HINSTANCE m_hInstance;			//프로세스핸들
	HWND m_hwnd;					//윈도우핸들

	InputClass* m_Input = nullptr;
	GraphicsClass* m_Graphics = nullptr;
};
//메시지를 처리하는 정적전역함수
static LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam);
static SystemClass* ApplicationHandle = 0;