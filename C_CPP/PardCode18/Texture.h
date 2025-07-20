#pragma once
#include "Resource.h"

class Texture : public BaseResource<Texture>
{
	friend class BaseResource<Texture>;
public:
	Texture(size_t hashID, const std::wstring& szFilePath);
	~Texture();
	Texture(const Texture&) = delete;
	Texture& operator=(const Texture&) = delete;
	Texture(Texture&&) = delete;
	Texture& operator=(Texture&&) = delete;

	void SetTXIdx(size_t idxTX);
	size_t GetTXIdx();
private:
	size_t m_IdxTX;
};

