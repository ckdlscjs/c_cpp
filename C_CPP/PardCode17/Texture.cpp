#include "Texture.h"
#include "RenderSystem.h"

Texture::Texture(size_t hashID, const std::wstring& szFilePath)
{
	m_lResourceID = hashID;
	m_szFilePath = szFilePath;
}

Texture::~Texture()
{
}

void Texture::SetTXIdx(size_t idxTX)
{
	m_IdxTX = idxTX;
}

size_t Texture::GetTXIdx()
{
	return m_IdxTX;
}




