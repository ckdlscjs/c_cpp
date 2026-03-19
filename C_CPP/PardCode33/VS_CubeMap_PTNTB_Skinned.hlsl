struct VS_INPUT
{
    float4 pos0 : POSITION0;
    float4 tex0 : TEXCOORD0;
    float4 normal0 : NORMAL0;
    float4 tangent0 : TANGENT0;
    float4 binormal0 : BINORMAL0;
    uint4 bones : BONES0;
    float4 weights : WEIGHTS0;
};

struct VS_OUTPUT
{
    float4 pos0 : SV_POSITION;
    float4 tex0 : TEXCOORD0;
    float4 normal0 : NORMAL0;
    float4 tangent0 : TANGENT0;
    float4 binormal0 : BINORMAL0;
    uint4 bones : BONES0;
    float4 weights : WEIGHTS0;
};

cbuffer CB_Bone : register(b2)
{
    row_major float4x4 matBone[256];
};

VS_OUTPUT vsmain(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    input.pos0.w = 1.0f;
    // Init array or else we get strange warnings about SV_POSITION.
    float weights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    weights[0] = input.weights.x;
    weights[1] = input.weights.y;
    weights[2] = input.weights.z;
    weights[3] = input.weights.w;

    float3 posAnim = float3(0.0f, 0.0f, 0.0f);
    for (int i = 0; i < 4; i++)
    {
	    // Assume no nonuniform scaling when transforming normals, so 
		// that we do not have to use the inverse-transpose.
        posAnim += weights[i] * mul(input.pos0, matBone[input.bones[i]]).xyz;
    }
    output.pos0 = float4(posAnim, 1.0f);
    output.tex0 = input.tex0;
    output.normal0 = input.normal0;
    output.tangent0 = input.tangent0;
    output.binormal0 = input.binormal0;
    output.bones = input.bones;
    output.weights = input.weights;
    return output;
}