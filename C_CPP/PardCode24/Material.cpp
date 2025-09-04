#include "Material.h"

Material::Material(size_t hash, const std::wstring& szFilePath)
{
	SetHash(hash);
	SetFilePath(szFilePath);
}

Material::~Material()
{
}
