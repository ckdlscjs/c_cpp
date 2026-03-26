#pragma once
#include "CommonHeader.h"

template<typename T>
class BaseShader
{
protected:
	BaseShader();
	virtual ~BaseShader();
	BaseShader(const BaseShader&) = delete;
	BaseShader& operator=(const BaseShader&) = delete;
	BaseShader(BaseShader&&) = delete;
	BaseShader& operator=(BaseShader&&) = delete;
public:
	ID3DBlob* GetBlob() const;
	T* GetShader();
protected:
	T* m_pShader;
	ID3DBlob* m_pBlob;
};

template<typename T>
inline BaseShader<T>::BaseShader()
{
	std::cout << "Initialize : " << "BaseShader <" << typeid(T).name() << "> Class" << '\n';
}

template<typename T>
inline BaseShader<T>::~BaseShader()
{
	std::cout << "Release : " << "BaseShader <" << typeid(T).name() << "> Class" << '\n';
	m_pShader->Release();
	m_pBlob->Release();
}

template<typename T>
inline ID3DBlob* BaseShader<T>::GetBlob() const
{
	return m_pBlob;
}

template<typename T>
inline T* BaseShader<T>::GetShader()
{
	return m_pShader;
}

class VertexShader : public BaseShader<ID3D11VertexShader>
{
public:
	VertexShader(ID3D11Device* pDevice, ID3DBlob* pBlob);
	VertexShader(const VertexShader&) = delete;
	VertexShader& operator=(const VertexShader&) = delete;
	VertexShader(VertexShader&&) = delete;
	VertexShader& operator=(VertexShader&&) = delete;
};

class HullShader : public BaseShader<ID3D11HullShader>
{
public:
	HullShader(ID3D11Device* pDevice, ID3DBlob* pBlob);
	HullShader(const HullShader&) = delete;
	HullShader& operator=(const HullShader&) = delete;
	HullShader(HullShader&&) = delete;
	HullShader& operator=(HullShader&&) = delete;
};

class DomainShader : public BaseShader<ID3D11DomainShader>
{
public:
	DomainShader(ID3D11Device* pDevice, ID3DBlob* pBlob);
	DomainShader(const DomainShader&) = delete;
	DomainShader& operator=(const DomainShader&) = delete;
	DomainShader(DomainShader&&) = delete;
	DomainShader& operator=(DomainShader&&) = delete;
};

class GeometryShader : public BaseShader<ID3D11GeometryShader>
{
public:
	GeometryShader(ID3D11Device* pDevice, ID3DBlob* pBlob);
	GeometryShader(const GeometryShader&) = delete;
	GeometryShader& operator=(const GeometryShader&) = delete;
	GeometryShader(GeometryShader&&) = delete;
	GeometryShader& operator=(GeometryShader&&) = delete;
};

class PixelShader : public BaseShader<ID3D11PixelShader>
{
public:
	PixelShader(ID3D11Device* pDevice, ID3DBlob* pBlob);
	PixelShader(const PixelShader&) = delete;
	PixelShader& operator=(const PixelShader&) = delete;
	PixelShader(PixelShader&&) = delete;
	PixelShader& operator=(PixelShader&&) = delete;
};

class ComputeShader : public BaseShader<ID3D11ComputeShader>
{
public:
	ComputeShader(ID3D11Device* pDevice, ID3DBlob* pBlob);
	ComputeShader(const ComputeShader&) = delete;
	ComputeShader& operator=(const ComputeShader&) = delete;
	ComputeShader(ComputeShader&&) = delete;
	ComputeShader& operator=(ComputeShader&&) = delete;
};

