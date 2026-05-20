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
    uint matrixID : MATRIXID;
};

cbuffer CB_BatchIdx : register(b10)
{
    uint batchIdx;
};

VS_OUTPUT vsmain(VS_INPUT input, uint instanceId : SV_InstanceID)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    output.pos0 = input.pos0;
    output.tex0 = input.tex0;
    output.normal0 = input.normal0;
    output.matrixID = batchIdx + instanceId;
    return output;
}