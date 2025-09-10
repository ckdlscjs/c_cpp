#pragma once
#include "Resource.h"
//추후에 확장시 참고
//https://mvje.tistory.com/83
class Material : public BaseResource<Material>
{
	friend class BaseResource<Material>;
public:
	Material(size_t hash, const std::wstring& szFilePath);
	~Material();
	Material(const Material&) = delete;
	Material& operator=(const Material&) = delete;
	Material(Material&&) = delete;
	Material& operator=(Material&&) = delete;

	void SetVS(size_t hashVS);
	size_t GetVS() const;
	void SetPS(size_t hashPS);
	size_t GetPS() const;
	void SetIL(size_t hashIL);
	size_t GetIL() const;
	/*void SetCB(size_t hashCB);
	size_t GetCB(int idx) const;*/
	void SetTexture(pTX_HASH t_hash);
	const std::vector<size_t>* GetTextures() const;
private:
	size_t m_lVS;
	size_t m_lPS;
	size_t m_lIL;
	//std::vector<size_t> m_lCBs;
	std::vector<size_t> m_lTXs[(UINT)E_Textures::count];
};
