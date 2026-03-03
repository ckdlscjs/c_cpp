struct VS_OUT
{
    float3 Pos0 : POSITION;
};

cbuffer CB_Tessellation : register(b1)
{
    float TessFactor; // 테셀레이션 분할 계수 (1.0 ~ 64.0)
    float radius;
};

// 2. Hull Shader (Patch Constant & Main)
struct HS_CS_OUTPUT
{
    float Edges[3] : SV_TessFactor;
    float Inside : SV_InsideTessFactor;
};

struct HS_OUT
{
    float3 Pos0 : POSITION;
};

//상수덮개셰이더, 테셀레이션 계수를 출력한다
HS_CS_OUTPUT ConstantHS(InputPatch<VS_OUT, 3> ip, uint patchID : SV_PrimitiveID)
{
    HS_CS_OUTPUT output;
    // 모든 면에 동일한 테셀레이션 계수 적용
    output.Edges[0] = output.Edges[1] = output.Edges[2] = output.Inside = TessFactor;
    return output;
}

[domain("tri")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("ConstantHS")]
[maxtessfactor(64.0f)]
HS_OUT hsmain(InputPatch<VS_OUT, 3> ip, uint i : SV_OutputControlPointID)
{
    HS_OUT output;
    output.Pos0 = ip[i].Pos0;
    return output;
}