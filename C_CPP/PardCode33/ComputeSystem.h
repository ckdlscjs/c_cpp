#pragma once
#include "BaseSystem.h"

class UnorderedAccessView;

class ComputeSystem : public BaseSystem<ComputeSystem>
{
	friend class BaseSystem<ComputeSystem>;	//CRTP패턴을 위한 friend선언
private:
	ComputeSystem();
	ComputeSystem(const ComputeSystem&) = delete;
	ComputeSystem& operator=(const ComputeSystem&) = delete;
	ComputeSystem(ComputeSystem&&) = delete;
	ComputeSystem& operator=(ComputeSystem&&) = delete;

public:
	~ComputeSystem();
	UnorderedAccessView* CreateUnorderedAccessView(ID3D11Device* pDevice, UINT size_buffer, void* data);
	void Init();
	void Frame(float deltatime);

private:
	std::unordered_map<size_t, UnorderedAccessView*>		m_pCUAVs;
};
#define _ComputeSystem ComputeSystem::GetInstance()