struct GS_OUTPUT
{
    float4 pos      : SV_Position;
    float4 color    : COLOR0;
};

cbuffer CB_WVPIT : register(b0)
{
    row_major float4x4 matWorld;
    row_major float4x4 matView;
    row_major float4x4 matProj;
    row_major float4x4 matInvTrans;
};

cbuffer CB_Box : register(b1)
{
    float3 vMin;
    float3 vMax;
};

[maxvertexcount(36)]
void gsmain(point uint input[1] : ANY_ID, inout TriangleStream<GS_OUTPUT> tristream)
{
    //local variable
    float3 pos = float3(0.0f, 0.0f, 0.0f);
    //float3 extent = float3(0.5f, 0.5f, 0.5f);
    float3 center = (vMin + vMax) * 0.5f;
    pos += center;
    float3 extent = (vMax - vMin) * 0.5f;
    float4 color = float4(1.0f, 0.0f, 0.0f, 1.0f);

    float3 vertices[8] =
    {
        pos + float3(-extent.x, +extent.y, -extent.z),  //front lt
        pos + float3(+extent.x, +extent.y, -extent.z),  //front rt
        pos + float3(-extent.x, -extent.y, -extent.z),  //front lb
        pos + float3(+extent.x, -extent.y, -extent.z),  //front rb
        pos + float3(-extent.x, +extent.y, +extent.z),  //rear lt
        pos + float3(+extent.x, +extent.y, +extent.z),  //rear rt
        pos + float3(-extent.x, -extent.y, +extent.z),  //rear lb
        pos + float3(+extent.x, -extent.y, +extent.z),  //rear rb
    };
    
    uint indices[36] =
    {
        0, 1, 2, 2, 1, 3, //front
        1, 5, 3, 3, 5, 7, //right
        5, 4, 7, 7, 4, 6, //rear
        4, 0, 6, 6, 0, 2, //left
        4, 5, 0, 0, 5, 1, //top
        2, 3, 6, 6, 3, 7, //bottom
    };
    
    [unroll]
    for (int i = 0; i < 36; ++i)
    {
        GS_OUTPUT output;
        //float4x4 matTranslation = float4x4(float4(1.0f, 0.0f, 0.0f, 0.0f), float4(0.0f, 1.0f, 0.0f, 0.0f), float4(0.0f, 0.0f, 1.0f, 0.0f), float4(center, 1.0f));
        //float4 transVert = mul(float4(vertices[indices[i]], 1.0f), matTranslation);
        output.pos = mul(float4(vertices[indices[i]], 1.0f), matWorld);
        output.pos = mul(output.pos, matView);
        output.pos = mul(output.pos, matProj);
        output.color = color;
        tristream.Append(output);

        // 삼각형 단위(3개 정점)로 끊어서 처리
        if ((i + 1) % 3 == 0)
        {
            tristream.RestartStrip();
        }
    }
}