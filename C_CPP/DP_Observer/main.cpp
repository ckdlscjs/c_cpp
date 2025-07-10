#include <iostream>
#include <functional>
#include <unordered_map>
// =======================================================
// 1. Event Data Structures (InputEventType, InputEvent)
// =======================================================
enum class InputEventType
{
	KEY_DOWN,      // Ű�� ó�� ���� ����
	KEY_UP,        // Ű�� ó�� ���� ����
	KEY_PRESSED,   // Ű�� �����ִ� ���� (Update���� üũ ����)
	MOUSE_MOVE,
	MOUSE_BUTTON_DOWN, // ���콺 ��ư�� ó�� ���� ����
	MOUSE_BUTTON_UP,   // ���콺 ��ư�� ó�� ���� ����
	MOUSE_WHEEL
};

struct InputEvent
{
	InputEventType type;
	int keyCode = 0;
	/*int mouseX = 0;
	int mouseY = 0;
	int mouseDeltaX = 0;
	int mouseDeltaY = 0;
	int wheelDelta = 0;*/
};
using EventCallBack = std::function<void(const InputEvent&)>;

// =======================================================
// 2. Subject, InputManager
// =======================================================

#define _InputManager InputManager::Get()
class InputManager 
{
private:
	InputManager() = default;
	~InputManager() = default;
	InputManager(const InputManager&) = delete;
	InputManager& operator=(const InputManager&) = delete;
	/*
	// �ݹ��� ����� �� �����ʸ� �ĺ��� �� �ִ� ID (��: �޸� �ּ�, �Ǵ� ���� ID)
    // std::function ���� ĸó �� this �����͸� ����ϴ� ���, �ش� ��ü�� �ּҸ� ID�� ����� �� �ֽ��ϴ�.
    // ������ �� ����� �� �� ���������Ƿ�, ������ ��� �ݹ��� ���Ϳ� �����ϴ� ������ �����մϴ�.
    // �� �߰��� ���: �� �ݹ鿡 ���� ID�� �ο��ϰ�, RemoveListener �� ID�� ����
	*/
	std::unordered_map<InputEventType, std::vector<EventCallBack>> m_listeners; // �� �̺�Ʈ Ÿ�Ժ��� ���� �ݹ��� ��� �� �� �ֵ��� ��ųʸ� ���
	/*
	//���� �Է� ���� ���� ������, win32�޽���ó���Լ�, event, �������..
	bool m_keyStates[256] = {false};
	bool m_prevKeyStates[256] = {false};
	POINT m_mousePos = {0, 0};
	POINT m_lastMousePos = {0, 0};
	bool m_mouseButtonStates[3] = {false};
	*/
	bool m_keyStates[256] = { false };      // ���� Ű ���� (����/����)
	bool m_prevKeyStates[256] = { false };  // ���� �������� Ű ����

	//POINT m_mousePos = { 0, 0 };           // ���� ���콺 ��ġ
	//POINT m_lastMousePos = { 0, 0 };       // ���� �������� ���콺 ��ġ

	//bool m_mouseButtonStates[3] = { false }; // ���콺 ��ư ���� (0: Left, 1: Right, 2: Middle)
	//bool m_prevMouseButtonStates[3] = { false }; // ���� �������� ���콺 ��ư ����
public:
	//�̱������� ȣ��
	static InputManager& Get()
	{
		static InputManager instance;
		return instance;
	}

	//�ݹ���(����, �Լ������͵�), ������ ���ظ� ���� ����ID�� �ܼ��� �ε������·ι�ȯ�Ѵ�
	//�����δ� ���� ID�� �����Ͽ� ��ȯ�ϰ�, m_listeners�� map<ID, callback> ���°� �� �������� �� �ִ�, �� id���� �ݹ��� ���н�Ų��
	size_t AddListener(InputEventType type, EventCallBack callback)
	{
		m_listeners[type].push_back(callback);
		return m_listeners[type].size() - 1;
	}
	
	//�ǵ��� �־��� id�� ������� �����ʿ��� ������Ų��, �ܼ�ȭ������ �ش�Ÿ���� �����ϴ½����λ��
	void RemoveAllListeners(InputEventType type)
	{
		m_listeners[type].clear();
	}
	// Ư�� �ݹ鸸 �����ϴ� �� ������ ��� (��: Ư�� ��ü�� �ּҿ� ���ε��� �ݹ� ����)
	  // template<typename T>
	  // void RemoveListener(InputEventType type, T* instance) {
	  //     auto& callbacks = m_listeners[type];
	  //     callbacks.erase(std::remove_if(callbacks.begin(), callbacks.end(),
	  //         [instance](const EventCallback& cb) {
	  //             // std::function�� ĸó�� ����� �ĺ��ϴ� ���� �����մϴ�.
	  //             // ���� AddListener �� ���� ID�� ��ȯ�ϰ� RemoveListener(ID)�� ó���մϴ�.
	  //             // �Ǵ� ������ ��ü�� ���� �����ϰ�, std::bind�� ����� this�� ��������� ĸó�ؾ� �մϴ�.
	  //             return false; // �� ���������� �ܼ�ȭ�� ���� �������� �ʽ��ϴ�.
	  //         }), callbacks.end());
	  // }

	void OnKeyDown(int VK_KEY)
	{
		if(VK_KEY < 0 || VK_KEY > 255) return;
		if (m_keyStates[VK_KEY]) return; //Ű�� ó�� ���������� �����Ѵ�
		m_keyStates[VK_KEY] = true;
		InputEvent event;
		event.type = InputEventType::KEY_DOWN;
		event.keyCode = VK_KEY;
		Notify(event);
	}

	void OnKeyUp(int VK_KEY)
	{
		if (VK_KEY < 0 || VK_KEY > 255) return;
		if (!m_keyStates[VK_KEY]) return; //Ű�� ������������ �����Ѵ�
		m_keyStates[VK_KEY] = false;
		InputEvent event;
		event.type = InputEventType::KEY_UP;
		event.keyCode = VK_KEY;
		Notify(event);
	}

	void Notify(const InputEvent& event)
	{
		if (m_listeners.count(event.type) <= 0) return;
		for (const auto& callback : m_listeners[event.type])
			if (callback)
				callback(event);
	}
	void OnKeyPressed(int VK_KEY)
	{
		InputEvent event;
		event.type = InputEventType::KEY_PRESSED;
		event.keyCode = VK_KEY;
		Notify(event);
	}
	/*
	void OnMouseMove(long x, long y)
	{
		// ���콺 ��ġ�� ������Ʈ�ϰ�, ��Ÿ�� Update���� ����Ͽ� ����
		if (m_mousePos.x == x && m_mousePos.y == y) return; // ��ġ�� ������ �ʾ����� �̺�Ʈ �߻� ����

		m_mousePos = { x, y };
		// MOUSE_MOVE �̺�Ʈ�� Update���� �߻���ų ���� ������, �ǽð� ������ ���� ���⼭�� �߻� ����
		// ���������� Update���� KEY_PRESSED�� �Բ� ó���ϵ��� �߽��ϴ�.
		// ���� OnMouseMove���� �ٷ� �̺�Ʈ�� �߻���Ű����, m_lastMousePos�� �̸� ������Ʈ�Ͽ� ��Ÿ�� ����ؾ� �մϴ�.
	}

	void OnMouseDown(int button, long x, long y) // button: 0=Left, 1=Right, 2=Middle
	{
		if (button >= 0 && button < 3)
		{
			if (!m_mouseButtonStates[button]) // ��ư�� 'ó��' ������ ���� �̺�Ʈ �߻�
			{
				m_mouseButtonStates[button] = true;
				InputEvent event = { InputEventType::MOUSE_BUTTON_DOWN, button, x, y };
				Notify(event);
			}
		}
	}

	void OnMouseUp(int button, long x, long y)
	{
		if (button >= 0 && button < 3)
		{
			if (m_mouseButtonStates[button]) // ��ư�� 'ó��' ������ ���� �̺�Ʈ �߻�
			{
				m_mouseButtonStates[button] = false;
				InputEvent event = { InputEventType::MOUSE_BUTTON_UP, button, x, y };
				Notify(event);
			}
		}
	}

	void OnMouseWheel(int wheelDelta) // wheelDelta�� ���� 120 �Ǵ� -120
	{
		InputEvent event = { InputEventType::MOUSE_WHEEL, 0, 0, 0, 0, 0, wheelDelta };
		Notify(event);
	}
	*/
	// =================================================================
	// �� ������ ȣ��Ǵ� Update �Լ�, �ش� Update�Լ����� üũ�� ���� Ư�� Ű�� �Է��� �Ǿ��ִٸ� Notify�� ������ ��ϵǾ��ִ� �ݹ��Լ��� ȣ���Ѵ�
	// =================================================================
	
	void Update() // ���� ������ ���� ����
	{
		// Ű���� ���� ���� �� KEY_PRESSED �̺�Ʈ �߻�
		for (int i = 0; i < 256; ++i)
		{
			if (m_keyStates[i]) // ���� �����ִ� Ű���
			{
				OnKeyPressed(i);
			}
			m_prevKeyStates[i] = m_keyStates[i]; // ���� ���� ����
		}
		/*
		// ���콺 �̵� ��Ÿ ��� �� MOUSE_MOVE �̺�Ʈ �߻�
		if (m_mousePos.x != m_lastMousePos.x || m_mousePos.y != m_lastMousePos.y)
		{
			InputEvent event = { InputEventType::MOUSE_MOVE, 0,
								 m_mousePos.x, m_mousePos.y,
								 m_mousePos.x - m_lastMousePos.x, m_mousePos.y - m_lastMousePos.y };
			Notify(event);
		}
		m_lastMousePos = m_mousePos; // ���� ���콺 ��ġ ����

		// ���콺 ��ư ���� ���� (���� ����: KEY_PRESSEDó�� MOUSE_BUTTON_PRESSED�� ���� ���� ����)
		memcpy(m_prevMouseButtonStates, m_mouseButtonStates, sizeof(m_mouseButtonStates));
		*/
	}
	
};

//Listener Ŭ����(�������̽� ��Ӿ��� �ݹ��� ����Ͽ� ���)
class Object
{
public:
	/*
	float m_speed = 5.0f;
	float m_posX = 0.0f;
	float m_posY = 0.0f;
	*/
	size_t m_keyDownListenerId = 0;
	size_t m_keyPressListenerId = 0;
	size_t m_mouseMoveListenerId = 0;
	size_t m_mouseDownListenerId = 0;

public:
	Object()
	{

	}
	~Object()
	{
		DisableInput();
	}

	void EnableInput()
	{
		
		_InputManager.AddListener(InputEventType::KEY_DOWN,
			[this](const InputEvent& event) {
				std::cout << "CallbackFunction " << "KeyDown " << event.keyCode << '\n';
				/*
				if (event.keyCode == 'W') m_posY += m_speed;
				if (event.keyCode == 'S') m_posY -= m_speed;
				if (event.keyCode == 'A') m_posX -= m_speed;
				if (event.keyCode == 'D') m_posX += m_speed;
				*/
			});
		_InputManager.AddListener(InputEventType::KEY_PRESSED,
			[this](const InputEvent& event) {
				std::cout << "CallbackFunction " << "KeyPressed " << event.keyCode << '\n';
				/*
				if (event.keyCode == 'W') m_posY += m_speed;
				if (event.keyCode == 'S') m_posY -= m_speed;
				if (event.keyCode == 'A') m_posX -= m_speed;
				if (event.keyCode == 'D') m_posX += m_speed;
				*/
			});
		_InputManager.AddListener(InputEventType::KEY_UP,
			[this](const InputEvent& event) {
				std::cout << "CallbackFunction " << "KeyUp " << event.keyCode << '\n';
				/*
				if (event.keyCode == 'W') m_posY += m_speed;
				if (event.keyCode == 'S') m_posY -= m_speed;
				if (event.keyCode == 'A') m_posX -= m_speed;
				if (event.keyCode == 'D') m_posX += m_speed;
				*/
			});
		/*
		m_keyDownListenerId = _InputManager.AddListener(InputEventType::KEY_DOWN,
			[this](const InputEvent& event) {
				std::cout << "Player Callback: Key " << (char)event.keyCode << " was pressed (initial)." << std::endl;
				if (event.keyCode == 0x20) { // VK_SPACE (�����̽���)
					std::cout << "Player jumps!" << std::endl;
				}
			});

		m_mouseMoveListenerId = _InputManager.AddListener(InputEventType::MOUSE_MOVE,
			[this](const InputEvent& event) {
				// std::cout << "Player Callback: Mouse moved to (" << event.mouseX << ", " << event.mouseY << ") Delta: (" << event.mouseDeltaX << ", " << event.mouseDeltaY << ")" << std::endl;
			});

		m_mouseDownListenerId = _InputManager.AddListener(InputEventType::MOUSE_BUTTON_DOWN,
			[this](const InputEvent& event) {
				std::cout << "Player Callback: Mouse button " << event.keyCode << " down at (" << event.mouseX << ", " << event.mouseY << ")" << std::endl;
			});
		*/
	}
	
	void DisableInput()
	{
		_InputManager.RemoveAllListeners(InputEventType::KEY_PRESSED);
		_InputManager.RemoveAllListeners(InputEventType::KEY_DOWN);
		_InputManager.RemoveAllListeners(InputEventType::MOUSE_MOVE);
		_InputManager.RemoveAllListeners(InputEventType::MOUSE_BUTTON_DOWN);
		/*
		if (m_keyPressListenerId == 0) return; // �̹� �����Ǿ� ������ ��ŵ

		std::cout << "Player: Disabling input listeners." << std::endl;
		

		m_keyPressListenerId = 0;
		m_keyDownListenerId = 0;
		m_mouseMoveListenerId = 0;
		m_mouseDownListenerId = 0;
		*/
	}

};
int main(void)
{
	const int VK_W = 0x57;
	const int VK_A = 0x41;
	const int VK_SPACE = 0x20;
	Object obj;
	obj.EnableInput();
	// ������ 1
	std::cout << "\n=== Frame 1 ===" << std::endl;
	//_InputManager.OnMouseMove(100, 100); // ���콺 ��ġ�� ������Ʈ
	_InputManager.Update(); // Update ���ο��� Notify(KEY_PRESSED)�� Notify(MOUSE_MOVE) ȣ��
	_InputManager.OnKeyDown(VK_W); // 'W' Ű�� ���� (OnKeyDown ���ο��� Notify(KEY_DOWN) ȣ��)

	// ������ 2
	std::cout << "\n=== Frame 2 ===" << std::endl;
	// 'W'�� ������ �����ִ� ���� -> Update �� KEY_PRESSED �ٽ� �߻�
	//_InputManager.OnMouseMove(105, 110); // ���콺 ��ġ ������Ʈ
	_InputManager.Update();

	// ������ 3
	std::cout << "\n=== Frame 3 ===" << std::endl;
	_InputManager.Update(); // 'A'�� �����ִ� ���� -> Update �� KEY_PRESSED �߻�
	_InputManager.OnKeyUp(VK_W); // 'W' Ű ���� (OnKeyUp ���ο��� Notify(KEY_UP) ȣ��)
	_InputManager.OnKeyDown(VK_A); // 'A' Ű ���� (OnKeyDown ���ο��� Notify(KEY_DOWN) ȣ��)
	//_InputManager.OnMouseDown(0, 50, 50); // ���� ���콺 ��ư Ŭ�� (OnMouseDown ���ο��� Notify(MOUSE_BUTTON_DOWN) ȣ��)

	// ������ 4
	std::cout << "\n=== Frame 4 ===" << std::endl;
	_InputManager.Update();
	//_InputManager.OnMouseUp(0, 50, 50); // ���� ���콺 ��ư ����
	//_InputManager.OnMouseWheel(120); // �� ��
	_InputManager.OnKeyDown(VK_SPACE); // �����̽��� ����

	// ������ 5
	std::cout << "\n=== Frame 5 ===" << std::endl;
	_InputManager.Update();
	_InputManager.OnKeyUp(VK_SPACE); // �����̽��� ����

	return 0;
}