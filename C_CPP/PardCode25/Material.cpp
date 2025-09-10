#include "Material.h"
#include <tiny_obj_loader.h>

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
Material::Material(size_t hash, const std::wstring& szFilePath)
{
	SetHash(hash);
	SetFilePath(szFilePath);
}

Material::~Material()
{
}

void Material::SetVS(size_t hashVS)
{
	m_lVS = hashVS;
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

//void Material::SetCB(size_t hashCB)
//{
//	m_lCBs.push_back(hashCB);
//}
//
//size_t Material::GetCB(int idx) const
//{
//	return m_lCBs[idx];
//}

void Material::SetTexture(pTX_HASH t_hash)
{
	m_lTXs[(UINT)t_hash.first].push_back(t_hash.second);
}

const std::vector<size_t>* Material::GetTextures() const
{
	return m_lTXs;
}