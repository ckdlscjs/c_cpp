////////////////////////////////////////////////////////////////////////////////
// Filename: light.ps
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
Texture2D shaderTexture : register( t0 );
SamplerState SampleType : register( s0 );

cbuffer LightBuffer
{
	float4 ambientColor;
	float4 diffuseColor;
    float3 lightDirection;
	float specularPower;
    float4 specularColor;
};


//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 viewDirection : TEXCOORD1;
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
	float3 reflection;
    float4 specular;


	// �� �ؽ�ó ��ǥ ��ġ���� ���÷��� ����Ͽ� �ؽ�ó���� �ȼ� ������ ���ø��մϴ�.
	textureColor = shaderTexture.Sample(SampleType, input.tex);

	// ��� �ȼ��� �⺻ ��� ������ �ֺ� ���� ������ �����մϴ�.
    color = ambientColor;
	float3 normalizeLightDir = normalize(lightDirection);
	// ����� ���� �� ������ ������ŵ�ϴ�.
	lightDir = -normalizeLightDir;

	// �� �ȼ��� ���� ���� ����մϴ�.
    lightIntensity = saturate(dot(input.normal, lightDir));

	if(lightIntensity > 0.0f)
    {
        // Ȯ�� ���� �� ������ �翡 ���� ���� Ȯ�� ���� �����մϴ�.
        color += (diffuseColor * lightIntensity);
    
    	// ���� ���� ������ ä ��ϴ�.
    	color = saturate(color);

		// ���� ����, ���� ���� �� ���� ���⿡ ���� �ݻ� ���͸� ����մϴ�.
		//dot(input.normal, lightDirection) -> �Ի���⿡���� ũ���� ��Į��谪(�����ǹݴ������ ��Į���)
		//���ǰ�� * inputNormal, �������͸� �ݴ������ �� ���̸�ŭ ��Į����Ͽ� ���̺��͸� ���Ѵ�,
		//�Ի纤�� - 2.0f(���Ǻ���) �ϸ� �ݻ纤�͸� ���� �� �ִ�, �̸� ����ȭ�� ����Ѵ�
		//Reflect = InputDir - 2.0f*(InputDir dot N)*N
		reflection = normalizeLightDir - 2.0f * dot(input.normal, normalizeLightDir) * input.normal;
		reflection = reflect(normalizeLightDir, input.normal);//hlsl�� �ݻ纤���Լ�, ���� ���������� �����ϴ�
		// �ݻ� ����, �ü� ���� �� �ݻ� ����� �������� �ݻ� ������ ���� �����մϴ�, �ݻ纤�Ϳ� ���������ϴ� ������ ��ġ���� �ݻ籤�� ���Ⱑ�ȴ�
		specular = pow(saturate(dot(reflection, input.viewDirection)), specularPower) * lightIntensity;
	}

	// �ؽ�ó �ȼ��� ���� Ȯ�� ���� ���Ͽ� ���� �ȼ� ���� ����� ����ϴ�.
    color = color * textureColor;

	// ��� ������ �������� �ݻ� ������Ʈ�� �߰��մϴ�.
	color = saturate(color + specular);

    return color;
}