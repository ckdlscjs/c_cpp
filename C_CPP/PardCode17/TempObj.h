#pragma once
#include "std.h"

class TempObj
{
public:
	TempObj();
	~TempObj();
	void Init();
	void Frame(float deltaTime);
	void Render();
	void Release();
private:
	bool CheckMask(int VK_KEY);
public:
	//Object Variables;
	XMFLOAT3 m_vScale;
	float m_fSpeedScale;
	XMFLOAT3 m_vRotate;
	float m_fSpeedRotate;
	XMFLOAT3 m_vTranslation;
	float m_fSpeedMove;
	std::unordered_map<int, int> VK_MASK;
	size_t m_lVKMask;
	std::unordered_map<InputEventType, std::vector<size_t>> m_IdxCallbacks;
public:
	//DirectX Variables;
	size_t m_IdxVB;
	size_t m_IdxIB;
	size_t m_IdxIL;
	size_t m_IdxVS;
	size_t m_IdxPS;
	size_t m_IdxTX;
	std::vector<size_t> m_IdxCBs;
};

