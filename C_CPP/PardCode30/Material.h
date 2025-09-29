#pragma once
#include "Resource.h"
//���Ŀ� Ȯ��� ����
//https://mvje.tistory.com/83
class Material : public BaseResource<Material>
{
	friend class BaseResource<Material>;
public:
	Material(size_t hash, const std::wstring& szFilePath);
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
	void SetTexture(TX_HASH tx_hash);
	const std::vector<size_t>* GetTextures() const;
	void SetTexturePath(TX_PATH tx_path);
	const std::vector<std::string>* GetTexturesPaths() const;
private:
	size_t m_lVS;
	size_t m_lPS;
	size_t m_lIL;
	std::vector<std::string> m_szTXPaths[(UINT)E_Textures::count];
	std::vector<size_t> m_lTXs[(UINT)E_Textures::count];
};

/*
* # �� ������ Wavefront OBJ MTL �����Դϴ�.

# ���ο� ���� ����: �����̳��� �ݼ� ����
newmtl Sci_Fi_Container_Material
# �ֺ��� ����
Ka 1.000000 1.000000 1.000000
# Ȯ�걤 ����
Kd 1.000000 1.000000 1.000000
# �ݻ籤 ����
Ks 0.000000 0.000000 0.000000
# �ݻ� ����
Ns 0.000000
# ����
d 1.000000
# ���� �� (PBR������ ���� 1 �Ǵ� 2)
illum 2

# PBR �ؽ�ó �ʵ�

# ���̽� �÷� / Ȯ�걤 (Base Color / Diffuse) ��
# ��ü�� �⺻ ����� ���� ������ ��� �ֽ��ϴ�.
map_Kd Sci_Fi_Container_BaseColor.png

# ��� (Normal) ��
# ��ü�� ǥ�� ������ �����Ͽ� �������� ������ ǥ���մϴ�.
map_bump Sci_Fi_Container_Normal.png
# �Ǵ� map_Norm Sci_Fi_Container_Normal.png (�Ϻ� �ļ����� ���)

# �����Ͻ� (Roughness) ��
# ǥ���� ��ĥ�⸦ ��Ÿ���� ���� �ݻ� ������ �����մϴ�.
map_Pr Sci_Fi_Container_Roughness.png

# ��Ż�� (Metallic) ��
# ��ü�� �ݼӼ��� ��Ÿ���ϴ�. (0: ��ݼ�, 1: �ݼ�)
map_Pm Sci_Fi_Container_Metallic.png

# �ں��Ʈ ��Ŭ���� (Ambient Occlusion) ��
# �ֺ����� ������ �޴� ������ ��Ÿ���� ������ ���մϴ�.
map_ao Sci_Fi_Container_AO.png
# �Ǵ� map_A Sci_Fi_Container_AO.png (�Ϻ� �ļ����� ���)

# ���̽ú� (Emissive) ��
# ��ü ��ü�� ���� ���� �κ��� ǥ���մϴ�.
map_Ke Sci_Fi_Container_Emissive.png
*/
inline Material::Material(size_t hash, const std::wstring& szFilePath)
{
	SetHash(hash);
	SetFilePath(szFilePath);
}

inline void Material::SetVS(size_t hashVS)
{
	m_lVS = hashVS;
}

inline size_t Material::GetVS() const
{
	return m_lVS;
}

inline void Material::SetPS(size_t hashPS)
{
	m_lPS = hashPS;
}

inline size_t Material::GetPS() const
{
	return m_lPS;
}

inline void Material::SetIL(size_t hashIL)
{
	m_lIL = hashIL;
}

inline size_t Material::GetIL() const
{
	return m_lIL;
}

//void Material::SetCB(size_t hashCB)
//{
//	m_lCBs.push_back(hashCB);
//}
//
//size_t Material::GetCB(int idx) const
//{
//	return m_lCBs[idx];
//}

inline void Material::SetTexture(TX_HASH tx_hash)
{
	m_lTXs[(UINT)tx_hash.tex].push_back(tx_hash.hash);
}

inline void Material::SetTexturePath(TX_PATH tx_path)
{
	m_szTXPaths[(UINT)tx_path.tex].push_back(tx_path.szPath);
}

inline const std::vector<size_t>* Material::GetTextures() const
{
	return m_lTXs;
}

inline const std::vector<std::string>* Material::GetTexturesPaths() const
{
	return m_szTXPaths;
}