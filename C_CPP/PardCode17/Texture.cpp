#include "Texture.h"
#include "RenderSystem.h"

Texture::Texture(size_t hashID, const std::wstring& szFilePath)
{
	/*
	WIC_FLAGS_NONE(0x0) :
	����: �ƹ� �÷��׵� �������� �ʽ��ϴ�.WIC�� ������ ��Ÿ�����͸� ������� �ּ��� �Ǵ��� ���� �ؽ�ó�� �ε��մϴ�.
	Ư���� ��ȯ�̳� ó���� �ʿ� ���� ��κ��� �⺻ �ؽ�ó �ε��� ���˴ϴ�.���Ͽ� sRGB ������ ������ �Ϲ������� sRGB �������� �ε�ǰ�, ������ ���� �������� �ε�˴ϴ�.

	WIC_FLAGS_IGNORE_SRGB(0x20) :
	���� : �̹��� ���Ͽ� sRGB ��Ÿ�����Ͱ� �ִ��� �̸� �����ϰ� ������ ����(Linear) ���� �������� �ؽ�ó�� �ε��մϴ�.��, �ؽ�ó�� DXGI_FORMAT_R8G8B8A8_UNORM(sRGB �ƴ�)�� ���� ���� �������� GPU�� �ö󰩴ϴ�.
	������ �ؽ�ó : ��� ��(Normal Map), �����Ͻ� ��(Roughness Map), ��Ż�Ͻ� ��(Metallic Map), AO(Ambient Occlusion) �� �� ���� ������ �ƴ� ������ �Ӽ��̳� ���� �����͸� ��� �ؽ�ó�� �ε��� �� �ʼ����Դϴ�.
	�̵��� ���� ������ ����Ǹ� �� �Ǳ� �����Դϴ�.
	��ü���� ���� ���� : ���� ���̴����� ��� ���� ������ �������� ó���ϰ� �Ͱų�, ���� Ÿ�� ����(RTV)���� sRGB�� �����Ͽ� ���� ��� �ܰ迡���� ���� ������ �Ϸ��� ��쿡�� ����� �� �ֽ��ϴ�.

	WIC_FLAGS_FORCE_SRGB(0x40) :
	���� : ������ ��Ÿ�����Ϳ� ������� ������ sRGB ���� �������� �ؽ�ó�� �ε��մϴ�.��, �ؽ�ó�� DXGI_FORMAT_R8G8B8A8_UNORM_SRGB�� ���� sRGB �������� GPU�� �ö󰩴ϴ�.
	�˺���(Albedo) �Ǵ� ��ǻ��(Diffuse) �� : ��ü�� �⺻ ������ ��Ÿ���� �ؽ�ó�� �Ϲ������� sRGB ���� ������ �ֽ��ϴ�.
	�� �÷��׸� ����ϸ� GPU�� ���̴����� ���ø��� �� �ڵ����� ���� �������� ��ȯ�� �־� ��Ȯ�� ������ ����� ���������ϴ�.
	��κ��� ColorMap�� �� �÷��׸� ����ϰų�, WIC_FLAGS_DEFAULT_SRGB�� ����մϴ�.
	*/

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




