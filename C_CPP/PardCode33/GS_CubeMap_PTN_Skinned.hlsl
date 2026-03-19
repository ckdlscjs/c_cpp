struct VS_OUTPUT
{
    float4 pos0 : SV_POSITION;
    float4 tex0 : TEXCOORD0;
    float4 normal0 : NORMAL0;
    float4 pos1 : WORLDP0;
};

[maxvertexcount(8)]
void gsmain(triangle VS_OUTPUT input[3], inout TriangleStream<VS_OUTPUT> tristream)
{
    VS_OUTPUT m[6];
    m[0] = input[0];
    m[1].pos0 = 0.5f * (input[0].pos0 + input[1].pos0);
    m[3].pos0 = 0.5f * (input[1].pos0 + input[2].pos0);
    m[2].pos0 = 0.5f * (input[2].pos0 + input[0].pos0);
    
    m[1].normal0 = input[0].normal0;
    m[3].normal0 = input[1].normal0;
    m[2].normal0 = input[2].normal0;
    
    m[1].tex0 = 0.5f * (input[0].tex0 + input[1].tex0);
    m[3].tex0 = 0.5f * (input[1].tex0 + input[2].tex0);
    m[2].tex0 = 0.5f * (input[2].tex0 + input[0].tex0);
    m[4] = input[2];
    m[5] = input[1];
    
    [unroll]
    for (int i = 0; i < 5; i++)
    {
        tristream.Append(m[i]);
    }
    
    tristream.RestartStrip();
    tristream.Append(m[1]);
    tristream.Append(m[5]);
    tristream.Append(m[3]);
}