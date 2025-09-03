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

	/*void SetIdx_TX(size_t idxTX);
	size_t GetIdx_TX() const;*/
	const ScratchImage* GetImage() const;
private:
	//size_t m_IdxTX;
	ScratchImage m_ScratchImage;
};

