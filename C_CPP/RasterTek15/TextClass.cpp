#include "stdafx.h"
#include "FontClass.h"
#include "FontShaderClass.h"
#include "TextClass.h"


TextClass::TextClass()
{
}

TextClass::TextClass(const TextClass& other)
{
}

TextClass::~TextClass()
{
}

bool TextClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, HWND hwnd, int screenWidth, int screenHeight, XMMATRIX baseViewMatrix)
{
	// ȭ�� �ʺ�� ���̸� �����մϴ�.
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// �⺻ �� ��Ʈ������ �����մϴ�.
	m_baseViewMatrix = baseViewMatrix;

	// ��Ʈ ��ü�� �����մϴ�.
	m_Font = new FontClass;
	if (!m_Font)
	{
		return false;
	}

	// ��Ʈ ��ü�� �ʱ�ȭ �մϴ�.
	bool result = m_Font->Initialize(device, "../RasterTek15/data/fontdata.txt", L"../RasterTek15/data/font.dds");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the font object.", L"Error", MB_OK);
		return false;
	}

	// ��Ʈ ���̴� ��ü�� �����մϴ�.
	m_FontShader = new FontShaderClass;
	if (!m_FontShader)
	{
		return false;
	}

	// ��Ʈ ���̴� ��ü�� �ʱ�ȭ �մϴ�.
	result = m_FontShader->Initialize(device, hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the font shader object.", L"Error", MB_OK);
		return false;
	}

	// ù ��° ������ �ʱ�ȭ�մϴ�.
	result = InitializeSentence(&m_sentence1, 16, device);
	if (!result)
	{
		return false;
	}

	// ���� ���� ���۸� �� ���ڿ� ������ ������Ʈ�մϴ�.
	result = UpdateSentence(m_sentence1, "Fps: ", 20, 20, 0.0f, 1.0f, 0.0f, deviceContext);
	if (!result)
	{
		return false;
	}

	// ù ��° ������ �ʱ�ȭ�մϴ�.
	result = InitializeSentence(&m_sentence2, 16, device);
	if (!result)
	{
		return false;
	}

	// ���� ���� ���۸� �� ���ڿ� ������ ������Ʈ�մϴ�.
	result = UpdateSentence(m_sentence2, "Cpu: ", 20, 40, 0.0f, 1.0f, 0.0f, deviceContext);
	if (!result)
	{
		return false;
	}


	// ù ��° ������ �ʱ�ȭ�մϴ�.
	result = InitializeSentence(&m_sentence3, 30, device);
	if (!result)
	{
		return false;
	}

	return true;
}


void TextClass::Shutdown()
{
	// ù��° ������ ��ȯ�մϴ�.
	ReleaseSentence(&m_sentence1);

	// �ι�° ������ ��ȯ�մϴ�.
	ReleaseSentence(&m_sentence2);

	// ��Ʈ ���̴� ��ü�� ��ȯ�մϴ�.
	if (m_FontShader)
	{
		m_FontShader->Shutdown();
		delete m_FontShader;
		m_FontShader = 0;
	}

	// ��Ʈ ��ü�� ��ȯ�մϴ�.
	if (m_Font)
	{
		m_Font->Shutdown();
		delete m_Font;
		m_Font = 0;
	}
}


bool TextClass::Render(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX orthoMatrix)
{
	// ù��° ������ �׸��ϴ�.
	if (!RenderSentence(deviceContext, m_sentence1, worldMatrix, orthoMatrix))
	{
		return false;
	}

	// ù��° ������ �׸��ϴ�.
	if (!RenderSentence(deviceContext, m_sentence2, worldMatrix, orthoMatrix))
	{
		return false;
	}

	// �ι�° ������ �׸��ϴ�.
	return RenderSentence(deviceContext, m_sentence3, worldMatrix, orthoMatrix);
}


bool TextClass::InitializeSentence(SentenceType** sentence, int maxLength, ID3D11Device* device)
{
	// ���ο� ���� ��ü�� ����ϴ�.
	*sentence = new SentenceType;
	if (!*sentence)
	{
		return false;
	}

	// ���� ���۸� null�� �ʱ�ȭ�մϴ�.
	(*sentence)->vertexBuffer = 0;
	(*sentence)->indexBuffer = 0;

	// ������ �ִ� ���̸� �����մϴ�.
	(*sentence)->maxLength = maxLength;

	// ���� �迭�� ���� ���� �����մϴ�.
	(*sentence)->vertexCount = 6 * maxLength;

	// �ε��� �迭�� �ε��� ���� �����մϴ�.
	(*sentence)->indexCount = (*sentence)->vertexCount;

	// ���� �迭�� ����ϴ�.
	VertexType* vertices = new VertexType[(*sentence)->vertexCount];
	if (!vertices)
	{
		return false;
	}

	// �ε��� �迭�� ����ϴ�.
	unsigned long* indices = new unsigned long[(*sentence)->indexCount];
	if (!indices)
	{
		return false;
	}

	// ó������ ���� �迭�� 0���� �ʱ�ȭ�մϴ�.
	memset(vertices, 0, (sizeof(VertexType) * (*sentence)->vertexCount));

	// �ε��� �迭�� �ʱ�ȭ�մϴ�.
	for (int i = 0; i<(*sentence)->indexCount; i++)
	{
		indices[i] = i;
	}

	// ���� �� ���� ������ ������ �����Ѵ�.
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * (*sentence)->vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// subresource ������ ���� �����Ϳ� ���� �����͸� �����մϴ�.
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// ���� ���۸� �����.
	if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &(*sentence)->vertexBuffer)))
	{
		return false;
	}

	// ���� �ε��� ������ ������ �����մϴ�.
	D3D11_BUFFER_DESC indexBufferDesc;
	
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * (*sentence)->indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// ���� ���ҽ� ������ �ε��� �����Ϳ� ���� �����͸� �����մϴ�.
	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// �ε��� ���۸� ����ϴ�.
	if (FAILED(device->CreateBuffer(&indexBufferDesc, &indexData, &(*sentence)->indexBuffer)))
	{
		return false;
	}

	// �� �̻� �ʿ����� ���� ���� �迭�� �����մϴ�.
	delete[] vertices;
	vertices = 0;

	// �� �̻� �ʿ����� ���� �ε��� �迭�� �����մϴ�.
	delete[] indices;
	indices = 0;

	return true;
}


bool TextClass::UpdateSentence(SentenceType* sentence, char* text, int positionX, int positionY, float red, float green, float blue, ID3D11DeviceContext* deviceContext)
{
	// ������ ���� �����Ѵ�.
	sentence->red = red;
	sentence->green = green;
	sentence->blue = blue;

	// ������ ���� ���� �÷ο츦 Ȯ���մϴ�.
	if ((int)strlen(text) > sentence->maxLength)
	{
		return false;
	}

	// ���� �迭�� ����ϴ�.
	VertexType* vertices = new VertexType[sentence->vertexCount];
	if (!vertices)
	{
		return false;
	}

	// ó������ ���� �迭�� 0���� �ʱ�ȭ�մϴ�.
	memset(vertices, 0, (sizeof(VertexType) * sentence->vertexCount));

	// �׸��⸦ ������ ȭ�鿡�� X �� Y �ȼ� ��ġ�� ����մϴ�.
	float drawX = (float)(((m_screenWidth / 2) * -1) + positionX);
	float drawY = (float)((m_screenHeight / 2) - positionY);

	// ��Ʈ Ŭ������ ����Ͽ� ���� �ؽ�Ʈ�� ���� �׸��� ��ġ���� ���� �迭�� ����ϴ�.
	m_Font->BuildVertexArray((void*)vertices, text, drawX, drawY);

	// ���ؽ� ���۸� �� �� �ֵ��� ��޴ϴ�.
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if (FAILED(deviceContext->Map(sentence->vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}

	// ���� ������ �����͸� ����Ű�� �����͸� ��´�.
	VertexType* verticesPtr = (VertexType*)mappedResource.pData;

	// �����͸� ���� ���ۿ� �����մϴ�.
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * sentence->vertexCount));

	// ���� ������ ����� �����մϴ�.
	deviceContext->Unmap(sentence->vertexBuffer, 0);

	// �� �̻� �ʿ����� ���� ���� �迭�� �����մϴ�.
	delete[] vertices;
	vertices = 0;

	return true;
}


void TextClass::ReleaseSentence(SentenceType** sentence)
{
	if (*sentence)
	{
		// ���� ���ؽ� ���۸� �����մϴ�.
		if ((*sentence)->vertexBuffer)
		{
			(*sentence)->vertexBuffer->Release();
			(*sentence)->vertexBuffer = 0;
		}

		// ���� �ε��� ���۸� �����մϴ�.
		if ((*sentence)->indexBuffer)
		{
			(*sentence)->indexBuffer->Release();
			(*sentence)->indexBuffer = 0;
		}

		// ������ �����մϴ�.
		delete *sentence;
		*sentence = 0;
	}
}


bool TextClass::RenderSentence(ID3D11DeviceContext* deviceContext, SentenceType* sentence, XMMATRIX worldMatrix, XMMATRIX orthoMatrix)
{
	// ���� ���� ���� �� �������� �����մϴ�.
	unsigned int stride = sizeof(VertexType);
	unsigned int offset = 0;

	// ������ �� �� �ֵ��� �Է� ��������� ���� ���۸� Ȱ������ �����մϴ�.
	deviceContext->IASetVertexBuffers(0, 1, &sentence->vertexBuffer, &stride, &offset);

	// ������ �� �� �ֵ��� �Է� ��������� �ε��� ���۸� Ȱ������ �����մϴ�.
	deviceContext->IASetIndexBuffer(sentence->indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// �� ���� ���ۿ��� ������ �Ǿ�� �ϴ� ������Ƽ�� ������ �����մϴ�.�� ��쿡�� �ﰢ���Դϴ�.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// �Է� �� ���� �������� �ȼ� ���� ���͸� ����ϴ�.
	XMFLOAT4 pixelColor = XMFLOAT4(sentence->red, sentence->green, sentence->blue, 1.0f);

	// ��Ʈ ���̴��� ����Ͽ� �ؽ�Ʈ�� �������մϴ�.
	if (!m_FontShader->Render(deviceContext, sentence->indexCount, worldMatrix, m_baseViewMatrix, orthoMatrix, m_Font->GetTexture(), pixelColor))
	{
		false;
	}

	return true;
}

bool TextClass::SetFps(int fps, ID3D11DeviceContext* deviceContext)
{
	// fps�� 10,000 ���Ϸ� �ڸ��ϴ�.
	if(fps > 9999)
	{
		fps = 9999;
	}

	// fps ������ ���ڿ� �������� ��ȯ�մϴ�.
	char tempString[16] = { 0, };
	_itoa_s(fps, tempString, 10);

	// fps ���ڿ��� �����մϴ�.
	char fpsString[16] = { 0, };
	strcpy_s(fpsString, "Fps: ");
	strcat_s(fpsString, tempString);

	float red = 0;
	float green = 0;
	float blue = 0;

	// fps�� 60 �̻��̸� fps ������ ������� �����մϴ�.
	if(fps >= 60)
	{
		red = 0.0f;
		green = 1.0f;
		blue = 0.0f;
	}

	// fps�� 60���� ���� ��� fps ������ ��������� �����մϴ�.
	if(fps < 60)
	{
		red = 1.0f;
		green = 1.0f;
		blue = 0.0f;
	}

	// fps�� 30 �̸��̸� fps ������ ���������� �����մϴ�.
	if(fps < 30)
	{
		red = 1.0f;
		green = 0.0f;
		blue = 0.0f;
	}

	// ���� ���� ���۸� �� ���ڿ� ������ ������Ʈ�մϴ�.
	return UpdateSentence(m_sentence1, fpsString, 20, 20, red, green, blue, deviceContext);
}


bool TextClass::SetCpu(int cpu, ID3D11DeviceContext* deviceContext)
{
	// cpu ������ ���ڿ� �������� ��ȯ�մϴ�.
	char tempString[16] = { 0, };
	_itoa_s(cpu, tempString, 10);

	// cpu ���ڿ��� �����մϴ�.
	char cpuString[16] = { 0, };
	strcpy_s(cpuString, "Cpu: ");
	strcat_s(cpuString, tempString);
	strcat_s(cpuString, "%");

	// ���� ���� ���۸� �� ���ڿ� ������ ������Ʈ�մϴ�.
	return UpdateSentence(m_sentence2, cpuString, 20, 40, 0.0f, 1.0f, 0.0f, deviceContext);
}
#include <string>
bool TextClass::SetDeltaTime(float deltatime, ID3D11DeviceContext* deviceContext)
{
	// ���� ���� ���۸� �� ���ڿ� ������ ������Ʈ�մϴ�.
	std::string str = "DeltaTime : ";
	str += std::to_string(deltatime).c_str();
	return UpdateSentence(m_sentence3, const_cast<char*>(str.c_str()), 20, 60, 1.0f, 0.0f, 0.0f, deviceContext);
}
