#include "stdafx.h"
#include "TextureClass.h"


TextureClass::TextureClass()
{
}


TextureClass::TextureClass(const TextureClass& other)
{
}


TextureClass::~TextureClass()
{
}


bool TextureClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename)
{
	int width = 0;
	int height = 0;
	
	// targa 이미지 데이터를 메모리에 로드합니다.
	if (!LoadTarga(filename, height, width))
	{
		return false;
	}

	//텍스처의 구조체를 설정합니다.
	D3D11_TEXTURE2D_DESC textureDesc;
	//ZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE2D_DESC));
	//textureDesc.Height = height;
	//textureDesc.Width = width;
	//textureDesc.MipLevels = 0;
	//textureDesc.ArraySize = 1;
	//textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	//textureDesc.SampleDesc.Count = 1;
	//textureDesc.SampleDesc.Quality = 0;
	//textureDesc.Usage = D3D11_USAGE_DEFAULT;
	//textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	//textureDesc.CPUAccessFlags = 0;
	//textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	//// 빈 텍스처를 생성합니다.
	//HRESULT hResult = device->CreateTexture2D(&textureDesc, NULL, &m_texture);
	//if (FAILED(hResult))
	//{
	//	return false;
	//}

	////  targa 이미지 데이터의 너비 사이즈를 설정합니다.
	//UINT rowPitch = (width * 4) * sizeof(unsigned char);

	//// targa 이미지 데이터를 텍스처에 복사합니다.
	//deviceContext->UpdateSubresource(m_texture, 0, NULL, m_targaData, rowPitch, 0);
	
	ZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE2D_DESC));
	textureDesc.Height = height;
	textureDesc.Width = width;
	textureDesc.MipLevels = 1;								//1일때는 밉맵지정, 0은 자동생성을의미
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;								//밉맵을 자동생성치 않으므로 0을지정

	D3D11_SUBRESOURCE_DATA resourceDesc;
	ZeroMemory(&resourceDesc, sizeof(D3D11_SUBRESOURCE_DATA));
	resourceDesc.pSysMem = m_targaData;
	resourceDesc.SysMemPitch = (width * 4);
	resourceDesc.SysMemSlicePitch = 0;
	// 빈 텍스처를 생성합니다.
	HRESULT hResult = device->CreateTexture2D(&textureDesc, &resourceDesc, &m_texture);
	if (FAILED(hResult))
	{
		return false;
	}

	// 셰이더 리소스 뷰 구조체를 설정합니다.
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;

	// 텍스처의 셰이더 리소스 뷰를 만듭니다.
	/*
	D3D11_SHADER_RESOURCE_VIEW_DESC 구조체는 Shader Resource View(SRV)를 만들 때 사용되는 설정을 담고 있습니다. 
	즉, Texture2D의 경우 해당 텍스처에 대한 SRV를 만들 때, 이 구조체를 통해 어떤 형식으로 SRV를 생성할지 설정을 지정할 수 있습니다.

	만약 D3D11_SHADER_RESOURCE_VIEW_DESC 구조체를 CreateShaderResourceView() 함수의 parameter로 전달하지 않는다면, DirectX는 텍스처를 SRV로 사용할 수 있는 디폴트 설정으로 생성할 것입니다. 
	이 경우에는 텍스처의 포맷을 사용하고, View Dimension은 TEXTURE2D, MostDetailedMip은 0, MipLevels은 텍스처의 Mip 레벨 수가 지정됩니다.

	그러므로 이 구조체를 명시하지 않아도 ShaderResourceView를 생성할 수 있지만, 구조체의 멤버를 적절히 설정함으로써 SRV를 조금 더 자세하게 설정할 수 있습니다.
	*/
	hResult = device->CreateShaderResourceView(m_texture, &srvDesc, &m_textureView);
	if (FAILED(hResult))
	{
		return false;
	}

	// 이 텍스처에 대해 밉맵을 생성합니다.
	/*
	* GenerateMips는 주어진 리소스에서 밉맵(mipmap)을 자동으로 생성하는 Direct3D 11 기능입니다.

	리소스는 보통 텍스처입니다. 밉맵은 하나의 텍스처에 여러 개의 해상도 버전이 포함된 것을 말합니다. 
	밉맵은 렌더링 성능을 향상시키는 데 사용됩니다. 더 작은 해상도의 밉맵은 더 큰 렌더 대상에 사용될 때 더 나은 성능을 제공합니다.

	GenerateMips를 호출하면 주어진 셰이더 리소스 뷰에서 생성된 밉맵 체인이 갱신됩니다. 이 함수는 텍스처 필터링 중에 밉맵 레벨을 선택할 때 자동으로 호출되는 경우가 많습니다.

	밉맵은 일반적으로 미리 생성되지만, GenerateMips 함수를 사용하여 런타임에서 동적으로 생성할 수도 있습니다. 
	그러나 이 함수는 런타임 성능에 영향을 미칠 수 있으므로 가능한 경우 미리 생성하는 것이 좋습니다.
	*/
	deviceContext->GenerateMips(m_textureView);

	// 이미지 데이터가 텍스처에 로드 되었으므로 targa 이미지 데이터를 해제합니다.
	delete[] m_targaData;
	m_targaData = 0;

	/*
	DirectX에서는 텍스처를 생성하고 이를 SRV로 변환하여 쉐이더에서 사용하므로, 텍스처를 사용할 필요가 없습니다. 
	텍스처를 Release()하더라도 SRV가 쉐이더에서 여전히 사용 가능합니다. 
	따라서 SRV를 제외한 다른 DirectX 개체들은 더 이상 필요하지 않을 때 Release()해주는 것이 좋습니다
	*/

	return true;
}


void TextureClass::Shutdown()
{
	//텍스처 뷰 리소스를 해제한다.
	if (m_textureView)
	{
		m_textureView->Release();
		m_textureView = 0;
	}

	// 텍스쳐를 해제합니다.
	if (m_texture)
	{
		m_texture->Release();
		m_texture = 0;
	}

	// targa 이미지 데이터를 해제합니다.
	if (m_targaData)
	{
		delete[] m_targaData;
		m_targaData = 0;
	}
}


ID3D11ShaderResourceView* TextureClass::GetTexture()
{
	return m_textureView;
}


bool TextureClass::LoadTarga(char* filename, int& height, int& width)
{
	// targa 파일을 바이너리 모드로 파일을 엽니다.
	FILE* filePtr;
	if (fopen_s(&filePtr, filename, "rb") != 0)
	{
		return false;
	}

	// 파일 헤더를 읽어옵니다.
	TargaHeader targaFileHeader;
	unsigned int count = (unsigned int)fread(&targaFileHeader, sizeof(TargaHeader), 1, filePtr);
	if (count != 1)
	{
		return false;
	}

	// 파일헤더에서 중요 정보를 얻어옵니다.
	height = (int)targaFileHeader.height;
	width = (int)targaFileHeader.width;
	int bpp = (int)targaFileHeader.bpp;

	// 파일이 32bit 인지 24bit인지 체크합니다.
	if (bpp != 32)
	{
		return false;
	}

	// 32 비트 이미지 데이터의 크기를 계산합니다.
	int imageSize = width * height * 4;

	//  targa 이미지 데이터 용 메모리를 할당합니다.
	unsigned char* targaImage = new unsigned char[imageSize];
	if (!targaImage)
	{
		return false;
	}

	// targa 이미지 데이터를 읽습니다.
	count = (unsigned int)fread(targaImage, 1, imageSize, filePtr);
	if (count != imageSize)
	{
		return false;
	}

	// 파일을 닫습니다.
	if (fclose(filePtr) != 0)
	{
		return false;
	}

	// targa 대상 데이터에 대한 메모리를 할당합니다.
	m_targaData = new unsigned char[imageSize];
	if (!m_targaData)
	{
		return false;
	}

	// targa 대상 데이터 배열에 인덱스를 초기화합니다.
	int index = 0;

	// targa 이미지 데이터에 인덱스를 초기화합니다.
	int k = (width * height * 4) - (width * 4);

	// 이제 targa 형식이 거꾸로 저장되었으므로 올바른 순서로 targa 이미지 데이터를 targa 대상 배열에 복사합니다.
	for (int j = 0; j<height; j++)
	{
		for (int i = 0; i<width; i++)
		{
			m_targaData[index + 0] = targaImage[k + 2];  // 빨강
			m_targaData[index + 1] = targaImage[k + 1];  // 녹색
			m_targaData[index + 2] = targaImage[k + 0];  // 파랑
			m_targaData[index + 3] = targaImage[k + 3];  // 알파

														 // 인덱스를 targa 데이터로 증가시킵니다.
			k += 4;
			index += 4;
		}

		// targa 이미지 데이터 인덱스를 역순으로 읽은 후 열의 시작 부분에서 이전 행으로 다시 설정합니다.
		k -= (width * 8);
	}

	// 대상 배열에 복사 된 targa 이미지 데이터를 해제합니다.
	delete[] targaImage;
	targaImage = 0;

	return true;
}