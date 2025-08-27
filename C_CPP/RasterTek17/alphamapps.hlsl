////////////////////////////////////////////////////////////////////////////////
// Filename: Alphamapps.hlsl
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
Texture2D shaderTextures[3]; //멀티텍스쳐를위해서 세장의 Texture2D를 받는다
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


    // 첫 번째 텍스처에서 픽셀 색상을 가져옵니다.
    color1 = shaderTextures[0].Sample(SampleType, input.tex);

    // 두 번째 텍스처에서 픽셀 색상을 가져옵니다.
    color2 = shaderTextures[1].Sample(SampleType, input.tex);

    // 세 번째 텍스처에서 픽셀 색상을 가져옵니다.
    alphaValue = shaderTextures[2].Sample(SampleType, input.tex);

    // 알파텍스쳐의 값을 기반으로 두 텍스쳐를 선형보간해 최종픽셀값을 지정한다
    blendColor = (alphaValue * color1) + (1.0f - alphaValue) * color2;
    
    // 최종 색상을 채 웁니다.
    blendColor = saturate(blendColor); //최종적으로 곱해진 픽셀을 0 ~ 1사이값으로 보간하기위해서 Saturate를 사용한다

    return blendColor;
}
