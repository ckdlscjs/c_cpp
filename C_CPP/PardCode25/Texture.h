#pragma once
#include "Resource.h"

class Texture : public BaseResource<Texture>
{
	friend class BaseResource<Texture>;
public:
	Texture(size_t hash, const std::wstring& szFilePath, DirectX::WIC_FLAGS flag = WIC_FLAGS_NONE);
	~Texture();
	Texture(const Texture&) = delete;
	Texture& operator=(const Texture&) = delete;
	Texture(Texture&&) = delete;
	Texture& operator=(Texture&&) = delete;

	void SetTX(size_t hashTX);
	size_t GetTX() const;
	const ScratchImage* GetImage() const;
private:
	size_t m_lTX;
	ScratchImage m_ScratchImage;
};

