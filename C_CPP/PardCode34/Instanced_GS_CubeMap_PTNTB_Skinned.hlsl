struct VS_OUTPUT
{
    float4 pos0 : SV_POSITION;
    float4 tex0 : TEXCOORD0;
    float4 normal0 : NORMAL0;
    float4 tangent0 : TANGENT0;
    float4 binormal0 : BINORMAL0;
    uint4 bones : BONES0;
    float4 weights : WEIGHTS0;
    uint matrixID : MATRIXID;
};

struct GS_OUTPUT
{
    float4 pos0 : SV_POSITION; //로컬정점
    float4 tex0 : TEXCOORD0; //텍스쳐좌표
    float4 normal0 : NORMAL0; //노말값
    float4 tangent0 : TANGENT0;
    float4 binormal0 : BINORMAL0;
    uint rtvIdx : SV_RenderTargetArrayIndex; // 어느 큐브맵 면에 그릴지 결정 (0~5)
    //LightResources
    float4 pos1 : WORLDP0; //월드변환된정점
    float4 lightPos0 : TEXCOORD1;
    float3 lightNormal0 : NORMAL1;
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

cbuffer CB_CubeMap : register(b7)
{
    row_major float4x4 matView_Cube[6];
};

struct STB_BatchMatrix
{
    row_major float4x4 matWorld;
    row_major float4x4 matInvTrans;
};

StructuredBuffer<STB_BatchMatrix> batchMatrix : register(t10); //BatchMatrix


[maxvertexcount(18)] //3*6
void gsmain(triangle VS_OUTPUT input[3], inout TriangleStream<GS_OUTPUT> outStream)
{
    float4 worldPos[3];
    float4 worldNormal[3];
    float4 worldTangent[3];
    float4 worldBinormal[3];

    for (int i = 0; i < 3; ++i)
    {
        worldPos[i] = mul(float4(input[i].pos0.xyz, 1.0f), batchMatrix[input[i].matrixID].matWorld);
        worldNormal[i] = float4(normalize(mul(input[i].normal0.xyz, (float3x3) batchMatrix[input[i].matrixID].matInvTrans)), 0.0f);
        worldTangent[i] = float4(normalize(mul(input[i].tangent0.xyz, (float3x3) batchMatrix[input[i].matrixID].matInvTrans)), 0.0f);
        worldBinormal[i] = float4(normalize(mul(input[i].binormal0.xyz, (float3x3) batchMatrix[input[i].matrixID].matInvTrans)), 0.0f);
    }
    
    for (int idx = 0; idx < 6; idx++)
    {
        for (int v = 0; v < 3; ++v)
        {
            GS_OUTPUT output;
            // Render Target Array Index 설정 (0~5)
            output.rtvIdx = idx;
            
            // 각 면의 View 및 Proj 행렬 적용
            output.pos0 = output.pos1 = worldPos[v];
            output.pos0 = mul(output.pos0, matView_Cube[idx]);
            output.pos0 = mul(output.pos0, matProj);
            
            output.normal0 = worldNormal[v];
            output.tangent0 = worldTangent[v];
            output.binormal0 = worldBinormal[v];
            output.tex0 = input[v].tex0;
            
            //그림자맵 계산을 위한 광원 시점 행렬의 변환
            output.lightPos0 = mul(output.pos1, matLightView);
            output.lightPos0 = mul(output.lightPos0, matLightProj);
    
            //빛의방향계산
            output.lightNormal0 = normalize(LightPos.xyz - output.pos1.xyz);
            
            outStream.Append(output);
        }
        
        // 삼각형 하나(면 하나) 완료 후 스트립 재시작
        outStream.RestartStrip();
    }
}