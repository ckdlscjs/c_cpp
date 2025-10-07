#pragma once
#include "Resource.h"

class Texture : public BaseResource<Texture>
{
	friend class BaseResource<Texture>;
public:
	Texture(size_t hash, const std::wstring& szFilePath, DirectX::WIC_FLAGS flag);
	Texture(size_t hash, const std::wstring& szFilePath, DirectX::DDS_FLAGS flag);
	Texture(const Texture&) = delete;
	Texture& operator=(const Texture&) = delete;
	Texture(Texture&&) = delete;
	Texture& operator=(Texture&&) = delete;

	void SetSRV(size_t hashSRV);
	size_t GetSRV() const;
	const ScratchImage* GetImage() const;
private:
	size_t m_lSRV;
	ScratchImage m_ScratchImage;
};

inline Texture::Texture(size_t hash, const std::wstring& szFilePath, DirectX::WIC_FLAGS flag)
{
	SetHash(hash);
	SetFilePath(szFilePath);
	/*
	WIC_FLAGS_NONE(0x0) :
	����: �ƹ� �÷��׵� �������� �ʽ��ϴ�.WIC�� ������ ��Ÿ�����͸� ������� �ּ��� �Ǵ��� ���� �ؽ�ó�� �ε��մϴ�.
	Ư���� ��ȯ�̳� ó���� �ʿ� ���� ��κ��� �⺻ �ؽ�ó �ε��� ���˴ϴ�.���Ͽ� sRGB ������ ������ �Ϲ������� sRGB �������� �ε�ǰ�, ������ ���� �������� �ε�˴ϴ�.

	WIC_FLAGS_IGNORE_SRGB(0x20) :
	���� : �̹��� ���Ͽ� sRGB ��Ÿ�����Ͱ� �ִ��� �̸� �����ϰ� ������ ����(Linear) ���� �������� �ؽ�ó�� �ε��մϴ�.��, �ؽ�ó�� DXGI_FORMAT_R8G8B8A8_UNORM(sRGB �ƴ�)�� ���� ���� �������� GPU�� �ö󰩴ϴ�.
	������ �ؽ�ó : ��� ��(Normal Map), �����Ͻ� ��(Roughness Map), ��Ż�Ͻ� ��(Metallic Map), AO(Ambient Occlusion) �� �� ���� ������ �ƴ� ������ �Ӽ��̳� ���� �����͸� ��� �ؽ�ó�� �ε��� �� �ʼ����Դϴ�.
	�̵��� ���� ������ ����Ǹ� �� �Ǳ� �����Դϴ�.
	��ü���� ���� ���� : ���� ���̴����� ��� ���� ������ �������� ó���ϰ� �Ͱų�, ���� Ÿ�� ����(RTV)���� sRGB�� �����Ͽ� ���� ��� �ܰ迡���� ���� ������ �Ϸ��� ��쿡�� ����� �� �ֽ��ϴ�.

	WIC_FLAGS_FORCE_SRGB(0x40) :
	���� : ������ ��Ÿ�����Ϳ� ������� ������ sRGB ���� �������� �ؽ�ó�� �ε��մϴ�.��, �ؽ�ó�� DXGI_FORMAT_R8G8B8A8_UNORM_SRGB�� ���� sRGB �������� GPU�� �ö󰩴ϴ�.
	�˺���(Albedo) �Ǵ� ��ǻ��(Diffuse) �� : ��ü�� �⺻ ������ ��Ÿ���� �ؽ�ó�� �Ϲ������� sRGB ���� ������ �ֽ��ϴ�.
	�� �÷��׸� ����ϸ� GPU�� ���̴����� ���ø��� �� �ڵ����� ���� �������� ��ȯ�� �־� ��Ȯ�� ������ ����� ���������ϴ�.
	��κ��� ColorMap�� �� �÷��׸� ����ϰų�, WIC_FLAGS_DEFAULT_SRGB�� ����մϴ�.
	*/
	// ���̺귯���� �Լ��� �����ּҸ��� �Ѱ� ��ü�����͸� �޾ƿ� �̸� �̿��Ѵ�
	_ASEERTION_CREATE(DirectX::LoadFromWICFile(szFilePath.c_str(), flag, nullptr, m_ScratchImage), "LoadTexture not successfully");
}
inline Texture::Texture(size_t hash, const std::wstring& szFilePath, DirectX::DDS_FLAGS flag)
{
	SetHash(hash);
	SetFilePath(szFilePath);
	_ASEERTION_CREATE(DirectX::LoadFromDDSFile(szFilePath.c_str(), flag, nullptr, m_ScratchImage), "LoadTexture not successfully");
}

inline void Texture::SetSRV(size_t hashSRV)
{
	m_lSRV = hashSRV;
}

inline size_t Texture::GetSRV() const
{
	return m_lSRV;
}

inline const ScratchImage* Texture::GetImage() const
{
	return &m_ScratchImage;
}


