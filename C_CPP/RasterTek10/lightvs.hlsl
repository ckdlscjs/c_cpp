////////////////////////////////////////////////////////////////////////////////
// Filename: light.vs
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
/*
Shader Model 4.0 이상에서는 상수 버퍼에 대한 레지스터 번호를 명시적으로 지정할 필요가 없으므로, cbuffer CameraBuffer에서 register 구문을 생략해도 됩니다. 
그러나 float3 변수 cameraPosition과 float 변수 padding은 각각 16바이트와 4바이트의 크기를 갖으므로, 상수 버퍼 내에서 적절한 위치에 배치되어야 합니다.
*/
cbuffer MatrixBuffer : register( b0 )
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer CameraBuffer
{
    float3 cameraPosition;
	float padding;
};

//////////////
// TYPEDEFS //
//////////////
struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 viewDirection : TEXCOORD1;
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType LightVertexShader(VertexInputType input)
{
    PixelInputType output = (PixelInputType)0;
    

	// 적절한 행렬 계산을 위해 위치 벡터를 4 단위로 변경합니다.
    input.position.w = 1.0f;

	// 월드, 뷰 및 투영 행렬에 대한 정점의 위치를 ​​계산합니다.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
	// 픽셀 쉐이더의 텍스처 좌표를 저장한다.
	output.tex = input.tex;
    
	// 월드 행렬에 대해서만 법선 벡터를 계산합니다.
    output.normal = mul(input.normal, (float3x3)worldMatrix);
	
    // 법선 벡터를 정규화합니다.
    output.normal = normalize(output.normal);

	// 세계의 정점 위치를 계산합니다.
    float4 worldPosition = mul(input.position, worldMatrix);

    // 카메라의 위치와 세계의 정점 위치를 기준으로 보기 방향을 결정합니다.
    output.viewDirection = cameraPosition.xyz - worldPosition.xyz;
	
    // 뷰 방향 벡터를 표준화합니다.
    output.viewDirection = normalize(output.viewDirection);

    return output;
}