#pragma once
#include "Resource.h"
//추후에 확장시 참고
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
* # 이 파일은 Wavefront OBJ MTL 파일입니다.

# 새로운 재질 정의: 컨테이너의 금속 재질
newmtl Sci_Fi_Container_Material
# 주변광 색상
Ka 1.000000 1.000000 1.000000
# 확산광 색상
Kd 1.000000 1.000000 1.000000
# 반사광 색상
Ks 0.000000 0.000000 0.000000
# 반사 지수
Ns 0.000000
# 투명도
d 1.000000
# 조명 모델 (PBR에서는 보통 1 또는 2)
illum 2

# PBR 텍스처 맵들

# 베이스 컬러 / 확산광 (Base Color / Diffuse) 맵
# 물체의 기본 색상과 질감 정보를 담고 있습니다.
map_Kd Sci_Fi_Container_BaseColor.png

# 노멀 (Normal) 맵
# 물체의 표면 법선을 변경하여 세부적인 굴곡을 표현합니다.
map_bump Sci_Fi_Container_Normal.png
# 또는 map_Norm Sci_Fi_Container_Normal.png (일부 파서에서 사용)

# 러프니스 (Roughness) 맵
# 표면의 거칠기를 나타내어 빛의 반사 정도를 제어합니다.
map_Pr Sci_Fi_Container_Roughness.png

# 메탈릭 (Metallic) 맵
# 물체의 금속성을 나타냅니다. (0: 비금속, 1: 금속)
map_Pm Sci_Fi_Container_Metallic.png

# 앰비언트 오클루전 (Ambient Occlusion) 맵
# 주변광의 영향을 받는 정도를 나타내어 음영을 더합니다.
map_ao Sci_Fi_Container_AO.png
# 또는 map_A Sci_Fi_Container_AO.png (일부 파서에서 사용)

# 에미시브 (Emissive) 맵
# 물체 자체가 빛을 내는 부분을 표현합니다.
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