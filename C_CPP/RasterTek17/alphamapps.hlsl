////////////////////////////////////////////////////////////////////////////////
// Filename: Alphamapps.hlsl
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
Texture2D shaderTextures[3]; //��Ƽ�ؽ��ĸ����ؼ� ������ Texture2D�� �޴´�
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
float4 AlphaMapPixelShader(PixelInputType input) : SV_TARGET
{
    float4 color1;
    float4 color2;
    float4 alphaValue;
    float4 blendColor;


    // ù ��° �ؽ�ó���� �ȼ� ������ �����ɴϴ�.
    color1 = shaderTextures[0].Sample(SampleType, input.tex);

    // �� ��° �ؽ�ó���� �ȼ� ������ �����ɴϴ�.
    color2 = shaderTextures[1].Sample(SampleType, input.tex);

    // �� ��° �ؽ�ó���� �ȼ� ������ �����ɴϴ�.
    alphaValue = shaderTextures[2].Sample(SampleType, input.tex);

    // �����ؽ����� ���� ������� �� �ؽ��ĸ� ���������� �����ȼ����� �����Ѵ�
    blendColor = (alphaValue * color1) + (1.0f - alphaValue) * color2;
    
    // ���� ������ ä ��ϴ�.
    blendColor = saturate(blendColor); //���������� ������ �ȼ��� 0 ~ 1���̰����� �����ϱ����ؼ� Saturate�� ����Ѵ�

    return blendColor;
}
