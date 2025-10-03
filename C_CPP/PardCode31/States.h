#pragma once
#include "CommonHeader.h"

template<typename T>
class BaseState
{
protected:
	BaseState();
	virtual ~BaseState();
	BaseState(const BaseState&) = delete;
	BaseState& operator=(const BaseState&) = delete;
	BaseState(BaseState&&) = delete;
	BaseState& operator=(BaseState&&) = delete;
	T* GetState(UINT idx);
protected:
	std::vector<T*> m_pStates;
};

template<typename T>
inline BaseState<T>::BaseState()
{
	std::cout << "Initialize : " << "BaseState <" << typeid(T).name() << "> Class" << '\n';
}

template<typename T>
inline BaseState<T>::~BaseState()
{
	std::cout << "Release : " << "BaseState <" << typeid(T).name() << "> Class" << '\n';
	for (auto iter = m_pStates.begin(); iter != m_pStates.end();)
	{
		(*iter)->Release();
		iter = m_pStates.erase(iter);
	}
}

template<typename T>
inline T* BaseState<T>::GetState(UINT idx)
{
	return m_pStates[idx];
}

class DepthStencilState : public BaseState<ID3D11DepthStencilState>
{
public:
	DepthStencilState(ID3D11Device* pDevice);
	DepthStencilState(const DepthStencilState&) = delete;
	DepthStencilState& operator=(const DepthStencilState&) = delete;
	DepthStencilState(DepthStencilState&&) = delete;
	DepthStencilState& operator=(DepthStencilState&&) = delete;
	ID3D11DepthStencilState* GetState(E_DSStates stateIdx);
};


class SamplerState : public BaseState<ID3D11SamplerState>
{
public:
	SamplerState(ID3D11Device* pDevice);
	SamplerState(const SamplerState&) = delete;
	SamplerState& operator=(const SamplerState&) = delete;
	SamplerState(SamplerState&&) = delete;
	SamplerState& operator=(SamplerState&&) = delete;
	ID3D11SamplerState* GetState(E_Samplers stateIdx);
};


class RasterizerState : public BaseState<ID3D11RasterizerState>
{
public:
	RasterizerState(ID3D11Device* pDevice);
	RasterizerState(const RasterizerState&) = delete;
	RasterizerState& operator=(const RasterizerState&) = delete;
	RasterizerState(RasterizerState&&) = delete;
	RasterizerState& operator=(RasterizerState&&) = delete;
	ID3D11RasterizerState* GetState(E_RSStates stateIdx);
};


class BlendState : public BaseState<ID3D11BlendState>
{
public:
	BlendState(ID3D11Device* pDevice);
	BlendState(const BlendState&) = delete;
	BlendState& operator=(const BlendState&) = delete;
	BlendState(BlendState&&) = delete;
	BlendState& operator=(BlendState&&) = delete;
	ID3D11BlendState* GetState(E_BSStates stateIdx);
};
