struct VS_INPUT
{
    float4 pos : POSITION;
    float4 color : COLOR;
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
};

VS_OUTPUT vsmain(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;   if (input.pos.y > 0.0f && input.pos.y < 1.0f)
    {
        input.pos.x += 0.25f;
    }

    if (input.pos.y > -1.0f && input.pos.y < 0.0f && input.pos.x > 0.0f && input.pos.x < 1.0f)
    {
        input.pos.y += 0.25f;
    }

    if (input.pos.y > 0.0f && input.pos.y < 1.0f && input.pos.x > -1.0f && input.pos.x < 0.0f)
    {
        input.pos.y -= 0.25f;
    }
 
    output.pos = input.pos;
    output.color = input.color;
    return output;
}