// Hs관련
struct HS_CS_OUTPUT
{
    float Edges[3] : SV_TessFactor;
    float Inside : SV_InsideTessFactor;
};

struct HS_OUT
{
    float3 pos0 : POSITION;
}; 


// 3. Domain Shader: 테셀레이터가 생성한 정점을 실제 구체 좌표로 투영
struct DS_OUT
{
    float4 pos0 : SV_Position;
    float4 color0 : COLOR0;
};

cbuffer CB_WVPIT : register(b0)
{
    row_major float4x4 matWorld;
    row_major float4x4 matView;
    row_major float4x4 matProj;
    row_major float4x4 matInvTrans;
};

cbuffer CB_Tessellation : register(b5)
{
    float3 center;
    float radius;
    float TessFactor; // 테셀레이션 분할 계수 (1.0 ~ 64.0)
};

[domain("tri")]
DS_OUT dsmain(HS_CS_OUTPUT input, float3 uvw : SV_DomainLocation, const OutputPatch<HS_OUT, 3> patch)
{
    DS_OUT output;
    
    // 무게중심 보간을 통한 새로운 정점 위치 계산, 중심을 기준으로 테셀레이션
    float3 pos = patch[0].pos0 * uvw.x + patch[1].pos0 * uvw.y + patch[2].pos0 * uvw.z;
    
    // [중요] 다시 normalize를 수행하여 정점을 단위 구체 표면으로 밀어냄
    pos = normalize(pos) * radius;
    pos += center;
    // 최종 좌표 변환
    output.pos0 = mul(float4(pos, 1.0f), matWorld);
    output.pos0 = mul(output.pos0, matView);
    output.pos0 = mul(output.pos0, matProj);
    
    output.color0 = float4(0.0f, 1.0f, 0.0f, 1.0f);
    
    return output;
}