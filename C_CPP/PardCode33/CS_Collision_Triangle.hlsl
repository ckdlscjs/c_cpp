struct Data
{
    float3 v1;
    float2 v2;
};

StructuredBuffer<Data> gInputA;
RWStructuredBuffer<Data> gOutput;

cbuffer CB_Bone : register(b2)
{
    row_major float4x4 matBone[256];
};

[numthreads(32, 1, 1)]
void csmain(int3 dtid : SV_DispatchThreadID)
{
    gOutput[dtid.x].v1 = gInputA[dtid.x].v1;
}
