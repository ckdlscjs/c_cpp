////////////////////////////////////////////////////////////////////////////////
// Filename: multitexture.ps
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
//Texture2D shaderTextures[3];  //��Ƽ�ؽ��ĸ����ؼ� ������ Texture2D�� �޴´�
Texture2D shaderTextures0;      //��Ƽ�ؽ��ĸ����ؼ� ������ Texture2D�� �޴´�
Texture2D shaderTextures1;      //��Ƽ�ؽ��ĸ����ؼ� ������ Texture2D�� �޴´�
Texture2D shaderTextures2;      //��Ƽ�ؽ��ĸ����ؼ� ������ Texture2D�� �޴´�
SamplerState SampleType;


//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};


////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 MultiTexturePixelShader(PixelInputType input) : SV_TARGET
{
	float4 color1;
    float4 color2;
    float4 lightColor;
    float4 blendColor;


    // ù ��° �ؽ�ó���� �ȼ� ������ �����ɴϴ�.
    color1 = shaderTextures0.Sample(SampleType, input.tex);

    // �� ��° �ؽ�ó���� �ȼ� ������ �����ɴϴ�.
    color2 = shaderTextures1.Sample(SampleType, input.tex);

    // �� ��° �ؽ�ó���� �ȼ� ������ �����ɴϴ�.
    lightColor = shaderTextures2.Sample(SampleType, input.tex);


    // �� �ȼ��� ȥ���ϰ� ���� ���� ���մϴ�.
    blendColor = color1 * color2 * 2.2f; //���ȼ����� ���ϰ� ������������ ���Ѵ�
    
    // ���� ������ ä ��ϴ�.
    blendColor = saturate(blendColor); //���������� ������ �ȼ��� 0 ~ 1���̰����� �����ϱ����ؼ� Saturate�� ����Ѵ�

    blendColor *= lightColor;
    return blendColor;
}
