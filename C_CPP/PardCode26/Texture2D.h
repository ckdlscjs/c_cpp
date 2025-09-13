#pragma once
#include "CommonHeader.h"

class Texture2D
{
public:
	Texture2D(ID3D11Device* pDevice, const ScratchImage* resource);
	~Texture2D();
	Texture2D(const Texture2D&) = delete;
	Texture2D& operator=(const Texture2D&) = delete;
	Texture2D(Texture2D&&) = delete;
	Texture2D& operator=(Texture2D&&) = delete;

	void SetVS(ID3D11DeviceContext* pDeviceContext, UINT startIdx = 0);
	void SetPS(ID3D11DeviceContext* pDeviceContext, UINT startIdx = 0);
private:
	ID3D11Resource* m_pTexture;			//실제리소스
	ID3D11ShaderResourceView* m_pSRV;	//리소스를 가리키는 세이더리소스뷰
};
