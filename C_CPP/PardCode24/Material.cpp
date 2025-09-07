#include "Material.h"

Material::Material(size_t hash, const std::wstring& szFilePath)
{
	SetHash(hash);
	SetFilePath(szFilePath);
	//추후 파일기재에 따른 머티리얼 초기화등이 필요함
}

Material::~Material()
{
}

void Material::SetVS(size_t hashVB)
{
	m_lVS = hashVB;
}

size_t Material::GetVS() const
{
	return m_lVS;
}

void Material::SetPS(size_t hashPS)
{
	m_lPS = hashPS;
}

size_t Material::GetPS() const
{
	return m_lPS;
}

void Material::SetIL(size_t hashIL)
{
	m_lIL = hashIL;
}

size_t Material::GetIL() const
{
	return m_lIL;
}

void Material::SetCB(size_t hashCB)
{
	m_lCBs.push_back(hashCB);
}

size_t Material::GetCB(int idx) const
{
	return m_lCBs[idx];
}

void Material::SetTX(pTX_HASH t_hash)
{
	m_lTXs[(UINT)t_hash.first].push_back(t_hash.second);
}

const std::vector<size_t>* Material::GetTXs() const
{
	return m_lTXs;
}