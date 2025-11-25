#include "CommonHeader.h"

template<typename T>
class BaseView
{
protected:
	BaseView();
	virtual ~BaseView();
	BaseView(const BaseView&) = delete;
	BaseView& operator=(const BaseView&) = delete;
	BaseView(BaseView&&) = delete;
	BaseView& operator=(BaseView&&) = delete;
	virtual void Resize(ID3D11Device* pDevice, ID3D11Texture2D* pBuffer, DXGI_FORMAT format) = 0;
public:
	T* GetView();
protected:
	T* m_pView;
};

template<typename T>
inline BaseView<T>::BaseView()
{
	std::cout << "Initialize : " << "BaseView <" << typeid(T).name() << "> Class" << '\n';
}

template<typename T>
inline BaseView<T>::~BaseView()
{
	std::cout << "Release : " << "BaseView <" << typeid(T).name() << "> Class" << '\n';
	if (m_pView)
		m_pView->Release();
}

template<typename T>
inline T* BaseView<T>::GetView()
{
	return m_pView;
}


class ShaderResourceView : public BaseView<ID3D11ShaderResourceView>
{
public:
	ShaderResourceView(ID3D11Device* pDevice, const ScratchImage* pResource);
	ShaderResourceView(ID3D11Device* pDevice, ID3D11Texture2D* pBuffer, DXGI_FORMAT format);
	ShaderResourceView(const ShaderResourceView&) = delete;
	ShaderResourceView& operator=(const ShaderResourceView&) = delete;
	ShaderResourceView(ShaderResourceView&&) = delete;
	ShaderResourceView& operator=(ShaderResourceView&&) = delete;
	void Resize(ID3D11Device* pDevice, ID3D11Texture2D* pBuffer, DXGI_FORMAT format) override;
};

class RenderTargetView : public BaseView<ID3D11RenderTargetView>
{
public:
	RenderTargetView(ID3D11Device* pDevice, IDXGISwapChain* pSwapChain);
	RenderTargetView(ID3D11Device* pDevice, ID3D11Texture2D* pBuffer);
	RenderTargetView(const RenderTargetView&) = delete;
	RenderTargetView& operator=(const RenderTargetView&) = delete;
	RenderTargetView(RenderTargetView&&) = delete;
	RenderTargetView& operator=(RenderTargetView&&) = delete;
	void Resize(ID3D11Device* pDevice, IDXGISwapChain* pSwapChain);
	void Resize(ID3D11Device* pDevice, ID3D11Texture2D* pBuffer, DXGI_FORMAT format) override;
};


class DepthStencilView : public BaseView<ID3D11DepthStencilView>
{
public:
	DepthStencilView(ID3D11Device* pDevice, ID3D11Texture2D* pBuffer);
	DepthStencilView(const DepthStencilView&) = delete;
	DepthStencilView& operator=(const DepthStencilView&) = delete;
	DepthStencilView(DepthStencilView&&) = delete;
	DepthStencilView& operator=(DepthStencilView&&) = delete;
	void Resize(ID3D11Device* pDevice, ID3D11Texture2D* pBuffer, DXGI_FORMAT format) override;
};
