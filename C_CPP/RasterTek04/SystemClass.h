#pragma once

//����� Ŭ���� ���漱��
class InputClass;
class GraphicsClass;

//SystemClass
class SystemClass
{
public:
	SystemClass();
	SystemClass(const SystemClass&);
	~SystemClass();

	bool Initialize();				//�ʱ�ȭ
	void Shutdown();				//�����
	void Run();						//���α׷��ݺ�����
	
	LRESULT CALLBACK MessageHandler(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam); 

private:
	bool Frame();
	void InitializeWindows(int&, int&);
	void ShutdownWindows();

private:
	LPCWSTR m_applicationName;
	HINSTANCE m_hInstance;			//���μ����ڵ�
	HWND m_hwnd;					//�������ڵ�

	InputClass* m_Input = nullptr;
	GraphicsClass* m_Graphics = nullptr;
};
//�޽����� ó���ϴ� ���������Լ�
static LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam);
static SystemClass* ApplicationHandle = 0;