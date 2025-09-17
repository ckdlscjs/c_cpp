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
	//DirectX Variables;
	//size_t m_IdxVB;
	//size_t m_IdxIB;
	//size_t m_IdxIL;
	//size_t m_IdxVS;
	//size_t m_IdxPS;
	//size_t m_IdxTX;
	//std::vector<size_t> m_IdxCBs;
	//std::vector<size_t> m_hashTextures;
	//std::vector<size_t> m_hashMaterials;
	std::vector<Mesh_Material> m_Mesh_Material;
};

