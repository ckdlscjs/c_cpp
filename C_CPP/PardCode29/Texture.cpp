#include "Texture.h"



Texture::Texture(size_t hash, const std::wstring& szFilePath, DirectX::WIC_FLAGS flag)
{
	SetHash(hash);
	SetFilePath(szFilePath);
	/*
	WIC_FLAGS_NONE(0x0) :
	설명: 아무 플래그도 설정하지 않습니다.WIC가 파일의 메타데이터를 기반으로 최선의 판단을 내려 텍스처를 로드합니다.
	특별한 변환이나 처리가 필요 없는 대부분의 기본 텍스처 로딩에 사용됩니다.파일에 sRGB 정보가 있으면 일반적으로 sRGB 포맷으로 로드되고, 없으면 선형 포맷으로 로드됩니다.

	WIC_FLAGS_IGNORE_SRGB(0x20) :
	설명 : 이미지 파일에 sRGB 메타데이터가 있더라도 이를 무시하고 강제로 선형(Linear) 감마 공간으로 텍스처를 로드합니다.즉, 텍스처는 DXGI_FORMAT_R8G8B8A8_UNORM(sRGB 아님)과 같은 선형 포맷으로 GPU에 올라갑니다.
	데이터 텍스처 : 노멀 맵(Normal Map), 러프니스 맵(Roughness Map), 메탈니스 맵(Metallic Map), AO(Ambient Occlusion) 맵 등 색상 정보가 아닌 물리적 속성이나 벡터 데이터를 담는 텍스처를 로드할 때 필수적입니다.
	이들은 감마 보정이 적용되면 안 되기 때문입니다.
	자체적인 감마 관리 : 만약 셰이더에서 모든 감마 보정을 수동으로 처리하고 싶거나, 렌더 타겟 포맷(RTV)에서 sRGB를 지정하여 최종 출력 단계에서만 감마 보정을 하려는 경우에도 사용할 수 있습니다.

	WIC_FLAGS_FORCE_SRGB(0x40) :
	설명 : 파일의 메타데이터와 관계없이 강제로 sRGB 감마 공간으로 텍스처를 로드합니다.즉, 텍스처는 DXGI_FORMAT_R8G8B8A8_UNORM_SRGB와 같은 sRGB 포맷으로 GPU에 올라갑니다.
	알베도(Albedo) 또는 디퓨즈(Diffuse) 맵 : 물체의 기본 색상을 나타내는 텍스처는 일반적으로 sRGB 감마 공간에 있습니다.
	이 플래그를 사용하면 GPU가 셰이더에서 샘플링할 때 자동으로 선형 공간으로 변환해 주어 정확한 라이팅 계산이 가능해집니다.
	대부분의 ColorMap에 이 플래그를 사용하거나, WIC_FLAGS_DEFAULT_SRGB를 사용합니다.
	*/
	// 라이브러리의 함수에 파일주소만을 넘겨 객체포인터를 받아와 이를 이용한다
	_ASEERTION_CREATE(DirectX::LoadFromWICFile(szFilePath.c_str(), flag, nullptr, m_ScratchImage), "LoadTexture not successfully");
}
Texture::Texture(size_t hash, const std::wstring& szFilePath, DirectX::DDS_FLAGS flag)
{
	SetHash(hash);
	SetFilePath(szFilePath);
	_ASEERTION_CREATE(DirectX::LoadFromDDSFile(szFilePath.c_str(), flag, nullptr, m_ScratchImage), "LoadTexture not successfully");
}

Texture::~Texture()
{
}

void Texture::SetTX(size_t hashTX)
{
	m_lTX = hashTX;
}

size_t Texture::GetTX() const
{
	return m_lTX;
}

const ScratchImage* Texture::GetImage() const
{
	return &m_ScratchImage;
}




