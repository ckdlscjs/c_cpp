#pragma once
#include "Resource.h"

class Texture : public BaseResource<Texture>
{
	friend class BaseResource<Texture>;
public:
	Texture(size_t hash, const std::wstring& szFilePath, ScratchImage&& scratchIamge);
	Texture(size_t hash, const std::wstring& szFilePath);
	Texture(const Texture&) = delete;
	Texture& operator=(const Texture&) = delete;
	Texture(Texture&&) = delete;
	void SetSRV(size_t hashSRV);
	size_t GetSRV() const;
	const ScratchImage* GetImage() const;
private:
	size_t m_lSRV;
	ScratchImage m_ScratchImage;
};

//우측값이동으로 원본객체(scratchImage)를 복사없이 경량비용으로 가져온다
inline Texture::Texture(size_t hash, const std::wstring& szFilePath, ScratchImage&& scratchIamge) : BaseResource(hash, szFilePath)
{
	m_ScratchImage = std::move(scratchIamge);
}
std::wstring g_texture_initpath = L"../Assets/Textures/";
inline Texture::Texture(size_t hash, const std::wstring& szFilePath) : BaseResource(hash, szFilePath)
{
	std::wstring extension = szFilePath.substr(szFilePath.find_last_of('.'));
	HRESULT hr;
	if(extension == L".dds")
		hr = DirectX::LoadFromDDSFile(szFilePath.c_str(), DirectX::DDS_FLAGS::DDS_FLAGS_NONE, nullptr, m_ScratchImage);
	else
		hr = DirectX::LoadFromWICFile(szFilePath.c_str(), DirectX::WIC_FLAGS::WIC_FLAGS_IGNORE_SRGB, nullptr, m_ScratchImage);
	if (hr == S_OK) return;
	
	//파일명에따른 경로추가
	std::wstring filePath = g_texture_initpath;
	auto lastSlash = szFilePath.find_last_of(L"\\/");
	if (lastSlash != std::wstring::npos)
		filePath += szFilePath.substr(lastSlash + 1);
	else
		filePath += szFilePath;
	if (extension == L".dds")
		hr = DirectX::LoadFromDDSFile(filePath.c_str(), DirectX::DDS_FLAGS::DDS_FLAGS_NONE, nullptr, m_ScratchImage);
	else
		hr = DirectX::LoadFromWICFile(filePath.c_str(), DirectX::WIC_FLAGS::WIC_FLAGS_IGNORE_SRGB, nullptr, m_ScratchImage);
	if (hr == S_OK) return;

	hr = DirectX::LoadFromWICFile(L"../Assets/Textures/brick.png", WIC_FLAGS_IGNORE_SRGB, nullptr, m_ScratchImage);
	if (hr == S_OK) return;
	_ASEERTION_CREATE(FAILED(hr), "LoadTexture not successfully");
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


