struct VS_INPUT
{
    float4 pos0 : POSITION0;
    float4 tex0 : TEXCOORD0;
    float4 normal0 : NORMAL0;
};

struct VS_OUTPUT
{
    float4 pos0 : SV_POSITION;
};

cbuffer CB_VP : register(b0)
{
    row_major float4x4 matView;
    row_major float4x4 matProj;
};

cbuffer CB_LightMat : register(b1)
{
    row_major float4x4 matLightView;
    row_major float4x4 matLightProj;
    float4 LightPos;
};

struct STB_BatchMatrix
{
    row_major float4x4 matWorld;
    row_major float4x4 matInvTrans;
};

StructuredBuffer<STB_BatchMatrix> batchMatrix : register(t10); //BatchMatrix

cbuffer CB_BatchIdx : register(b10)
{
    uint batchIdx;
};

VS_OUTPUT vsmain(VS_INPUT input, uint instanceId : SV_InstanceID)
{
    uint matrixID = batchIdx + instanceId;
    VS_OUTPUT output = (VS_OUTPUT) 0;
    input.pos0.w = 1.0f;
    output.pos0 = mul(input.pos0, batchMatrix[matrixID].matWorld);
    output.pos0 = mul(output.pos0, matLightView);
    output.pos0 = mul(output.pos0, matLightProj); //원근나눗셈은 래스터라이저에서 들어온 w값으로 알아서수행된다
    return output;
}