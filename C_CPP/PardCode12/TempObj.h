#pragma once
#include "std.h"

class TempObj
{
public:
	TempObj();
	~TempObj();
	void Init();
	void Frame();
	void Render();
	void Release();

public:
	//Object Variables;
	XMFLOAT3 m_vScale;
	XMFLOAT3 m_vRotate;
	XMFLOAT3 m_vTranslation;

public:
	//DirectX Variables;
	size_t m_IdxVB;
	size_t m_IdxIB;
	size_t m_IdxIL;
	size_t m_IdxVS;
	size_t m_IdxPS;
	std::vector<size_t> m_IdxCBs;
};

