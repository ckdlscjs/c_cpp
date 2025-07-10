#include <iostream>
#include <functional>
#include <unordered_map>
// =======================================================
// 1. Event Data Structures (InputEventType, InputEvent)
// =======================================================
enum class InputEventType
{
	KEY_DOWN,      // 키가 처음 눌린 순간
	KEY_UP,        // 키가 처음 떼진 순간
	KEY_PRESSED,   // 키가 눌려있는 동안 (Update에서 체크 가능)
	MOUSE_MOVE,
	MOUSE_BUTTON_DOWN, // 마우스 버튼이 처음 눌린 순간
	MOUSE_BUTTON_UP,   // 마우스 버튼이 처음 떼진 순간
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
	// 콜백을 등록할 때 리스너를 식별할 수 있는 ID (예: 메모리 주소, 또는 고유 ID)
    // std::function 람다 캡처 시 this 포인터를 사용하는 경우, 해당 객체의 주소를 ID로 사용할 수 있습니다.
    // 하지만 이 방식은 좀 더 복잡해지므로, 간단히 모든 콜백을 벡터에 저장하는 예제로 시작합니다.
    // 더 견고한 방법: 각 콜백에 고유 ID를 부여하고, RemoveListener 시 ID로 제거
	*/
	std::unordered_map<InputEventType, std::vector<EventCallBack>> m_listeners; // 각 이벤트 타입별로 여러 콜백을 등록 할 수 있도록 딕셔너리 사용
	/*
	//실제 입력 상태 관리 변수들, win32메시지처리함수, event, 변수들등..
	bool m_keyStates[256] = {false};
	bool m_prevKeyStates[256] = {false};
	POINT m_mousePos = {0, 0};
	POINT m_lastMousePos = {0, 0};
	bool m_mouseButtonStates[3] = {false};
	*/
	bool m_keyStates[256] = { false };      // 현재 키 상태 (눌림/떼짐)
	bool m_prevKeyStates[256] = { false };  // 이전 프레임의 키 상태

	//POINT m_mousePos = { 0, 0 };           // 현재 마우스 위치
	//POINT m_lastMousePos = { 0, 0 };       // 이전 프레임의 마우스 위치

	//bool m_mouseButtonStates[3] = { false }; // 마우스 버튼 상태 (0: Left, 1: Right, 2: Middle)
	//bool m_prevMouseButtonStates[3] = { false }; // 이전 프레임의 마우스 버튼 상태
public:
	//싱글턴으로 호출
	static InputManager& Get()
	{
		static InputManager instance;
		return instance;
	}

	//콜백등록(람다, 함수포인터등), 예제의 이해를 위해 고유ID를 단순한 인덱스형태로반환한다
	//실제로는 고유 ID를 생성하여 반환하고, m_listeners에 map<ID, callback> 형태가 더 안정적일 수 있다, 즉 id별로 콜백을 구분시킨다
	size_t AddListener(InputEventType type, EventCallBack callback)
	{
		m_listeners[type].push_back(callback);
		return m_listeners[type].size() - 1;
	}
	
	//의도는 주어진 id를 기반으로 리스너에서 해제시킨다, 단순화를위해 해당타입을 제거하는식으로사용
	void RemoveAllListeners(InputEventType type)
	{
		m_listeners[type].clear();
	}
	// 특정 콜백만 제거하는 더 정교한 방법 (예: 특정 객체의 주소와 바인딩된 콜백 제거)
	  // template<typename T>
	  // void RemoveListener(InputEventType type, T* instance) {
	  //     auto& callbacks = m_listeners[type];
	  //     callbacks.erase(std::remove_if(callbacks.begin(), callbacks.end(),
	  //         [instance](const EventCallback& cb) {
	  //             // std::function이 캡처한 대상을 식별하는 것은 복잡합니다.
	  //             // 보통 AddListener 시 고유 ID를 반환하고 RemoveListener(ID)로 처리합니다.
	  //             // 또는 리스너 객체를 직접 전달하고, std::bind를 사용해 this를 명시적으로 캡처해야 합니다.
	  //             return false; // 이 예제에서는 단순화를 위해 구현하지 않습니다.
	  //         }), callbacks.end());
	  // }

	void OnKeyDown(int VK_KEY)
	{
		if(VK_KEY < 0 || VK_KEY > 255) return;
		if (m_keyStates[VK_KEY]) return; //키가 처음 눌렸을때만 반응한다
		m_keyStates[VK_KEY] = true;
		InputEvent event;
		event.type = InputEventType::KEY_DOWN;
		event.keyCode = VK_KEY;
		Notify(event);
	}

	void OnKeyUp(int VK_KEY)
	{
		if (VK_KEY < 0 || VK_KEY > 255) return;
		if (!m_keyStates[VK_KEY]) return; //키가 눌려있을때만 반응한다
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
		// 마우스 위치만 업데이트하고, 델타는 Update에서 계산하여 전달
		if (m_mousePos.x == x && m_mousePos.y == y) return; // 위치가 변하지 않았으면 이벤트 발생 안함

		m_mousePos = { x, y };
		// MOUSE_MOVE 이벤트는 Update에서 발생시킬 수도 있지만, 실시간 반응을 위해 여기서도 발생 가능
		// 예제에서는 Update에서 KEY_PRESSED와 함께 처리하도록 했습니다.
		// 만약 OnMouseMove에서 바로 이벤트를 발생시키려면, m_lastMousePos를 미리 업데이트하여 델타를 계산해야 합니다.
	}

	void OnMouseDown(int button, long x, long y) // button: 0=Left, 1=Right, 2=Middle
	{
		if (button >= 0 && button < 3)
		{
			if (!m_mouseButtonStates[button]) // 버튼이 '처음' 눌렸을 때만 이벤트 발생
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
			if (m_mouseButtonStates[button]) // 버튼이 '처음' 떼졌을 때만 이벤트 발생
			{
				m_mouseButtonStates[button] = false;
				InputEvent event = { InputEventType::MOUSE_BUTTON_UP, button, x, y };
				Notify(event);
			}
		}
	}

	void OnMouseWheel(int wheelDelta) // wheelDelta는 보통 120 또는 -120
	{
		InputEvent event = { InputEventType::MOUSE_WHEEL, 0, 0, 0, 0, 0, wheelDelta };
		Notify(event);
	}
	*/
	// =================================================================
	// 매 프레임 호출되는 Update 함수, 해당 Update함수에서 체크를 통해 특정 키가 입력이 되어있다면 Notify를 수행해 등록되어있는 콜백함수를 호출한다
	// =================================================================
	
	void Update() // 이전 프레임 상태 갱신
	{
		// 키보드 상태 갱신 및 KEY_PRESSED 이벤트 발생
		for (int i = 0; i < 256; ++i)
		{
			if (m_keyStates[i]) // 현재 눌려있는 키라면
			{
				OnKeyPressed(i);
			}
			m_prevKeyStates[i] = m_keyStates[i]; // 이전 상태 복사
		}
		/*
		// 마우스 이동 델타 계산 및 MOUSE_MOVE 이벤트 발생
		if (m_mousePos.x != m_lastMousePos.x || m_mousePos.y != m_lastMousePos.y)
		{
			InputEvent event = { InputEventType::MOUSE_MOVE, 0,
								 m_mousePos.x, m_mousePos.y,
								 m_mousePos.x - m_lastMousePos.x, m_mousePos.y - m_lastMousePos.y };
			Notify(event);
		}
		m_lastMousePos = m_mousePos; // 이전 마우스 위치 갱신

		// 마우스 버튼 상태 갱신 (선택 사항: KEY_PRESSED처럼 MOUSE_BUTTON_PRESSED를 만들 수도 있음)
		memcpy(m_prevMouseButtonStates, m_mouseButtonStates, sizeof(m_mouseButtonStates));
		*/
	}
	
};

//Listener 클래스(인터페이스 상속없이 콜백을 등록하여 사용)
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
				if (event.keyCode == 0x20) { // VK_SPACE (스페이스바)
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
		if (m_keyPressListenerId == 0) return; // 이미 해제되어 있으면 스킵

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
	// 프레임 1
	std::cout << "\n=== Frame 1 ===" << std::endl;
	//_InputManager.OnMouseMove(100, 100); // 마우스 위치만 업데이트
	_InputManager.Update(); // Update 내부에서 Notify(KEY_PRESSED)와 Notify(MOUSE_MOVE) 호출
	_InputManager.OnKeyDown(VK_W); // 'W' 키가 눌림 (OnKeyDown 내부에서 Notify(KEY_DOWN) 호출)

	// 프레임 2
	std::cout << "\n=== Frame 2 ===" << std::endl;
	// 'W'는 여전히 눌려있는 상태 -> Update 시 KEY_PRESSED 다시 발생
	//_InputManager.OnMouseMove(105, 110); // 마우스 위치 업데이트
	_InputManager.Update();

	// 프레임 3
	std::cout << "\n=== Frame 3 ===" << std::endl;
	_InputManager.Update(); // 'A'는 눌려있는 상태 -> Update 시 KEY_PRESSED 발생
	_InputManager.OnKeyUp(VK_W); // 'W' 키 떼짐 (OnKeyUp 내부에서 Notify(KEY_UP) 호출)
	_InputManager.OnKeyDown(VK_A); // 'A' 키 눌림 (OnKeyDown 내부에서 Notify(KEY_DOWN) 호출)
	//_InputManager.OnMouseDown(0, 50, 50); // 왼쪽 마우스 버튼 클릭 (OnMouseDown 내부에서 Notify(MOUSE_BUTTON_DOWN) 호출)

	// 프레임 4
	std::cout << "\n=== Frame 4 ===" << std::endl;
	_InputManager.Update();
	//_InputManager.OnMouseUp(0, 50, 50); // 왼쪽 마우스 버튼 떼짐
	//_InputManager.OnMouseWheel(120); // 휠 업
	_InputManager.OnKeyDown(VK_SPACE); // 스페이스바 눌림

	// 프레임 5
	std::cout << "\n=== Frame 5 ===" << std::endl;
	_InputManager.Update();
	_InputManager.OnKeyUp(VK_SPACE); // 스페이스바 떼짐

	return 0;
}