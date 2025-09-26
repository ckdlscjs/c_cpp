#pragma once
#include "CommonHeader.h"

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
	Vector3 m_vScale;
	float m_fSpeedScale;
	Vector3 m_vRotate;
	float m_fSpeedRotate;
	Vector3 m_vPosition;
	float m_fSpeedMove;
	bool bRenderable;

	std::unordered_map<int, int> VK_MASK;
	size_t m_lVKMask;
	std::unordered_map<E_InputEventType, std::vector<size_t>> m_IdxCallbacks;
public:
	std::vector<Mesh_Material> m_Mesh_Material;
};

