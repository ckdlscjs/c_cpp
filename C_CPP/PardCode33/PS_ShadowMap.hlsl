
struct PS_INPUT
{
    float4 pos0 : SV_POSITION;
};

cbuffer CB_Campos : register(b5)
{
    float4 campos;
};

void psmain(PS_INPUT input)
{
    //GPU는 자동으로 SV_POSITION의 Z/W, 즉 원근나누기한 최종 NDC변환된 깊이값을 DSV에 기록한다
    //alphaclip등이 필요할경우만 추가로 처리해준다
    
}