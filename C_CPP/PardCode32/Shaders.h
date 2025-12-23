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

class PixelShader : public BaseShader<ID3D11PixelShader>
{
public:
	PixelShader(ID3D11Device* pDevice, ID3DBlob* pBlob);
	PixelShader(const PixelShader&) = delete;
	PixelShader& operator=(const PixelShader&) = delete;
	PixelShader(PixelShader&&) = delete;
	PixelShader& operator=(PixelShader&&) = delete;
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