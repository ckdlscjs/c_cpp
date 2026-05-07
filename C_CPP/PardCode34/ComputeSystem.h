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
	void Init();
	void Update();

public:
	void SetCS_Shader(size_t hashCS);
	void SetCS_ShaderResourceView(size_t hashSRV, UINT startIdx = 0);
	void SetCS_UnorderedAccessView(size_t hashUAV, UINT startIdx = 0);
	void SetCS_ConstantBuffer(size_t hashCB, UINT startIdx = 0);
	void Dispatch(UINT x, UINT y, UINT z);
private:
};
#define _ComputeSystem ComputeSystem::GetInstance()