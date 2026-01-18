struct VS_INPUT
{
    float4 pos0 : POSITION0;
    float4 tex0 : TEXCOORD0;
    float4 normal0 : NORMAL0;
};

struct VS_OUTPUT
{
    float4 pos0 : SV_POSITION;
    float4 tex0 : TEXCOORD0;
    float4 normal0 : NORMAL0;
    float4 pos1 : WORLDP0;
    float4 lightPos0 : TEXCOORD1;
    float3 lightNormal0 : NORMAL1;
};

cbuffer CB_WVPIT : register(b0)
{
    row_major float4x4 matWorld;
    row_major float4x4 matView;
    row_major float4x4 matProj;
    row_major float4x4 matInvTrans;
};

cbuffer CB_LightMat : register(b1)
{
    row_major float4x4 matLightView;
    row_major float4x4 matLightProj;
    float4 LightPos;
};

VS_OUTPUT vsmain(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    input.pos0.w = 1.0f;
    output.pos0 = mul(input.pos0, matWorld);
    output.pos1 = output.pos0;
    output.pos0 = mul(output.pos0, matView);
    output.pos0 = mul(output.pos0, matProj); //원근나눗셈은 래스터라이저에서 들어온 w값으로 알아서수행된다
   
    output.tex0 = input.tex0;
    output.normal0 = float4(mul(input.normal0.xyz, (float3x3) matInvTrans), 0.0f);
    
    //그림자맵 계산을 위한 광원 시점 행렬의 변환
    output.lightPos0 = mul(output.pos1, matLightView);
    output.lightPos0 = mul(output.lightPos0, matLightProj);
    
    //빛의방향계산
    output.lightNormal0 = normalize(LightPos.xyz - output.pos1.xyz);
    
    return output;
}