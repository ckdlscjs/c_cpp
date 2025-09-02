////////////////////////////////////////////////////////////////////////////////
// Filename: multitexture.ps
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
//Texture2D shaderTextures[3];  //멀티텍스쳐를위해서 두장의 Texture2D를 받는다
Texture2D shaderTextures0;      //멀티텍스쳐를위해서 두장의 Texture2D를 받는다
Texture2D shaderTextures1;      //멀티텍스쳐를위해서 두장의 Texture2D를 받는다
Texture2D shaderTextures2;      //멀티텍스쳐를위해서 두장의 Texture2D를 받는다
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


    // 첫 번째 텍스처에서 픽셀 색상을 가져옵니다.
    color1 = shaderTextures0.Sample(SampleType, input.tex);

    // 두 번째 텍스처에서 픽셀 색상을 가져옵니다.
    color2 = shaderTextures1.Sample(SampleType, input.tex);

    // 세 번째 텍스처에서 픽셀 색상을 가져옵니다.
    lightColor = shaderTextures2.Sample(SampleType, input.tex);


    // 두 픽셀을 혼합하고 감마 값을 곱합니다.
    blendColor = color1 * color2 * 2.2f; //두픽셀값을 곱하고 감마보정값을 곱한다
    
    // 최종 색상을 채 웁니다.
    blendColor = saturate(blendColor); //최종적으로 곱해진 픽셀을 0 ~ 1사이값으로 보간하기위해서 Saturate를 사용한다

    blendColor *= lightColor;
    return blendColor;
}
