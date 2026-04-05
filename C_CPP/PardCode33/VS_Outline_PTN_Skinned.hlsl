struct VS_INPUT
{
    float4 pos0 : POSITION0;
    float4 tex0 : TEXCOORD0;
    float4 normal0 : NORMAL0;
    uint4 bones : BONES0;
    float4 weights : WEIGHTS0;
};

struct VS_OUTPUT
{
    float4 pos0 : SV_POSITION;
    float4 color0 : COLOR0;
};

cbuffer CB_WVPIT : register(b0)
{
    row_major float4x4 matWorld;
    row_major float4x4 matView;
    row_major float4x4 matProj;
    row_major float4x4 matInvTrans;
};

cbuffer CB_Bone : register(b2)
{
    row_major float4x4 matBone[256];
};

cbuffer CB_Outline : register(b3) // 슬롯 번호는 엔진 세팅에 맞춰 조절
{
    float3 outline_Color;
    float outline_Thickness; // 외곽선 두께 (예: 0.03f)
};


// ---------------------------------------------------------
// Vertex Shader: 정점을 법선 방향으로 밀어내어 물체를 크게 만듭니다.
// ---------------------------------------------------------
VS_OUTPUT vsmain(VS_INPUT input)
{
    VS_OUTPUT output;
    input.pos0.w = 1.0f;

    // Init array or else we get strange warnings about SV_POSITION.
    float weights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    weights[0] = input.weights.x;
    weights[1] = input.weights.y;
    weights[2] = input.weights.z;
    weights[3] = input.weights.w;

    float3 posAnim = float3(0.0f, 0.0f, 0.0f);
    float3 normAnim = float3(0.0f, 0.0f, 0.0f);
    for (int i = 0; i < 4; i++)
    {
	    // Assume no nonuniform scaling when transforming normals, so 
		// that we do not have to use the inverse-transpose.
        posAnim += weights[i] * mul(input.pos0, matBone[input.bones[i]]).xyz;
        normAnim += weights[i] * mul(float4(input.normal0.xyz, 0.0f), matBone[input.bones[i]]).xyz;
    }
    normAnim = normalize(normAnim); //재정규화
 
    output.pos0 = mul(float4(posAnim, 1.0f), matWorld);
    float3 worldNormal = normalize(mul(normAnim, (float3x3) matInvTrans));
    //노말값의 확장크기를 더함
    output.pos0.xyz += worldNormal * outline_Thickness;
    output.pos0 = mul(output.pos0, matView);
    output.pos0 = mul(output.pos0, matProj);

    output.color0 = float4(outline_Color / 255.0f, 1.0f);
    return output;
    
}