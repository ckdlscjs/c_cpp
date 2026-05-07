struct PS_INPUT
{
    float4 pos0 : SV_POSITION;
    float4 tex0 : TEXCOORD0;
    float4 normal0 : NORMAL0;
    //LightResources
    float4 pos1 : WORLDP0;
    float4 lightPos0 : TEXCOORD1;
    float3 lightNormal0 : NORMAL1;
};

float4 psmain(PS_INPUT input) : SV_Target
{
    return float4(1.0f, 1.0f, 0.0f, 1.0f);
}