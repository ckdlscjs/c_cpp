struct VS_OUT
{
    uint anyID : ANY_ID;
    uint matrixID : MATRIXID;
};

cbuffer CB_BatchIdx : register(b10)
{
    uint batchIdx;
};

VS_OUT vsmain(uint vID : SV_VertexID, uint instanceId : SV_InstanceID)
{
    VS_OUT output;
    output.anyID = vID;
    output.matrixID = instanceId + batchIdx;
    return output;
}