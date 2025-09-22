#pragma once
#include "Resource.h"

class Texture : public BaseResource<Texture>
{
	friend class BaseResource<Texture>;
public:
	Texture(size_t hash, const std::wstring& szFilePath, DirectX::WIC_FLAGS flag = WIC_FLAGS_NONE);
	Texture(size_t hash, const std::wstring& szFilePath, DirectX::DDS_FLAGS flag);
	~Texture();
	Texture(const Texture&) = delete;
	Texture& operator=(const Texture&) = delete;
	Texture(Texture&&) = delete;
	Texture& operator=(Texture&&) = delete;

	void SetVW(size_t hashVW);
	size_t GetVW() const;
	const ScratchImage* GetImage() const;
private:
	size_t m_lVW;
	ScratchImage m_ScratchImage;
};

