struct VS_INPUT
{
    float4 pos0 : POSITION0;
    float4 tex0 : TEXCOORD0;
    float4 normal0 : NORMAL0;
};

struct VS_OUTPUT
{
    float4 pos0 : SV_POSITION;
    float4 color0 : COLOR0;
};

cbuffer CB_WVPIT : register(b0)
{
    row_major float4x4 matWorld;
    row_major float4x4 matView;
    row_major float4x4 matProj;
    row_major float4x4 matInvTrans;
};

cbuffer CB_Outline : register(b3)   // 슬롯 번호는 엔진 세팅에 맞춰 조절
{
    float3 outline_Color;
    float outline_Thickness;        // 외곽선 두께 (예: 0.03f)
};


// ---------------------------------------------------------
// Vertex Shader: 정점을 법선 방향으로 밀어내어 물체를 크게 만듭니다.
// ---------------------------------------------------------
VS_OUTPUT vsmain(VS_INPUT input)
{
    VS_OUTPUT output;   
    
    //월드변환
    output.pos0 = mul(float4(input.pos0.xyz, 1.0f), matWorld);
    float3 worldNormal = normalize(mul(input.normal0.xyz, (float3x3) matInvTrans));
    //노말값의 확장크기를 더함
    output.pos0.xyz += worldNormal * outline_Thickness;
    output.pos0 = mul(output.pos0, matView);
    output.pos0 = mul(output.pos0, matProj);

    output.color0 = float4(outline_Color / 255.0f, 1.0f);
    return output;
}