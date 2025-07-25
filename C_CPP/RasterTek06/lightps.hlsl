////////////////////////////////////////////////////////////////////////////////
// Filename: light.ps
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
Texture2D shaderTexture;
SamplerState SampleType;

cbuffer LightBuffer
{
    float4 diffuseColor;
    float3 lightDirection;
	float padding;
};


//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};


////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 LightPixelShader(PixelInputType input) : SV_TARGET
{
	float4 textureColor;
	float3 lightDir;
	float lightIntensity;
	float4 color;


	// �� �ؽ�ó ��ǥ ��ġ���� ���÷��� ����Ͽ� �ؽ�ó���� �ȼ� ������ ���ø��մϴ�.
	textureColor = shaderTexture.Sample(SampleType, input.tex);

	// ����� ���� �� ������ ������ŵ�ϴ�->�������� ������ ���ϰ��Ѵ�
    lightDir = -lightDirection;

    // �� �ȼ��� ���� ���� ����մϴ�.
    /*
    HLSL���� saturate �Լ��� �Է°��� [0, 1] ���� ���� �����մϴ�. 
    �Է°��� 0���� ������ 0����, 1���� ũ�� 1�� Ŭ�����մϴ�. 
    ��, saturate(x) = min(max(x, 0), 1.0f)�Դϴ�. 
    �� �Լ��� �Ϲ������� �÷� ���̳� �Ÿ� ���� ���� �پ��� �׷��� �˰��򿡼� ���˴ϴ�.
    */
    //Kd -> min(0.0f, max(L dot N, 0.0f))
    lightIntensity = saturate(dot(input.normal, lightDir));

    // ���� ������ ���� �� Ȯ�� ���� �������� ���� ������ ���� ������ �����մϴ�. -> Kd * Ld
    color = saturate(diffuseColor * lightIntensity);

    // �ؽ�ó �ȼ��� ���� Ȯ�� ���� ���Ͽ� ���� �ȼ� ���� ����� ����ϴ�. -> Kd * Ld * Md
	color = lightIntensity;

    return color;
}