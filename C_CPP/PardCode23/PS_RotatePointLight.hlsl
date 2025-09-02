Texture2D Texture;
sampler TextureSampler : register(s0); //���������ʾƵ� �⺻���÷��� ��������0�� ���õȴ�

struct PS_INPUT
{
    float4 pos0 : SV_POSITION;
    float4 tex0 : TEXCOORD0;
    float4 normal0 : NORMAL0;
    float4 pos1 : WORLDP0;
};

/*
PointLight
L -> �����������ϴº��� (���� - ��ġ �� ����������Ѵ�)
D -> �Ÿ���
N -> ��������
R -> �ݻ纤��(�����������º����ǹݻ纤��)
V -> ���������ϴº���
shiness -> ǥ���Ǳ��õ�
Ambient = Ma (Material Ambient)
Diffuse = Kd (L dot N, clamp0~1) * Md (MaterialDiffuse)
Specular = Kd(��ġ��üũ) * Ks(pow((R dot V, clamp0~1), shiness) * Ms(Material Specular)
Attenuation = att(a0, a1, a2) dot (1.0f, d, d^2)

PointLight = Ambient + (Diffuse + Specular) / attenuation
*/

cbuffer Light_Point : register(b1)
{
    float4 lp_ambient;
    float4 lp_diffuse;
    float4 lp_specular;
    float3 lp_pos; //��������ġ
    float lp_shiness; //���õ�
    
    float lp_att_a0; //����ɼ�
    float lp_att_a1;
    float lp_att_a2;
    float lp_range; //���޹���
};

float4 PointLight(float3 L, float D, float3 N, float3 R, float3 V)
{
    float4 P_A = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 P_D = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 P_S = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float normalizeD = D / lp_range;
    //ambient
    P_A = lp_ambient;
    
    //diffuse,
    float Kd = max(dot(L, N), 0.0f); //max(L dot N, 0)
    P_D = Kd * lp_diffuse;
    
    //specular
    float Ks = pow(max(dot(R, V), 0.0f), lp_shiness);
    P_S = Kd * Ks * lp_specular;
    
    float att = 1.0f / dot(float3(lp_att_a0, lp_att_a1, lp_att_a2), float3(1.0f, normalizeD, normalizeD * normalizeD)); //������, a0 + a1d + a2d^2
    return P_A + (P_D + P_S) * att; //Ambient�� �׳� �߻�ȴ�
}


cbuffer CB_Campos : register(b5)
{
    float4 campos;
};


float4 psmain(PS_INPUT input) : SV_Target
{
    float4 M = Texture.Sample(TextureSampler, input.tex0.xy); //�ؽ��İ�
    float4 P = input.pos1; //��ȯ�� ����������
    
    //PointLight
    float3 L = lp_pos - P.xyz;  //���������ϴº���
    float D = length(L);        //����-���� ������ �Ÿ���
    L /= D; //����ȭ
    float3 N = normalize(input.normal0).xyz;
    float3 R = reflect(-L, N);  //�ݻ纤��(�Ի纤��, ������ ���ڷλ��)
    float3 V = normalize(campos.xyz - P.xyz); //���������ϴº���
    
    return PointLight(L, D, N, R, V);
}

//float4 psmain(PS_INPUT input) : SV_Target
//{
//    float4 M = Texture.Sample(TextureSampler, input.tex0.xy); //�ؽ��İ�
//    float4 P = input.pos1; //��ȯ�� ����������
    
//   //AmbientLight
//    float ka = 1.5f;
//    float3 ia = float3(0.09f, 0.082f, 0.082f);
//    ia *= M.rgb;

//    float3 ambient_light = ka * ia;

//	//DiffuseLight
//    float kd = 0.7f;
//    float3 direction_point_light = normalize(lp_pos.xyz - P.xyz); //������Ʈ�κ��ͺ��ǹ���
//    float distance_light_object = length(lp_pos.xyz - P.xyz); //������Ʈ�κ��ͺ��ǰŸ�
//    float fade_area = max(0.0f, (distance_light_object - lp_range)); //����Ÿ�

//    float constant_func = 1.0f; //�����
//    float linear_func = 2.0f; //������
//    float quadratic_func = 1.0f; //Ⱦ��
//    float attenuation = constant_func + linear_func * fade_area + quadratic_func * fade_area * fade_area; //���谪

//    float amount_diffuse_light = max(0.0f, dot(direction_point_light.xyz, normalize(input.normal0).xyz));
//    float3 id = M.rgb;
//    float3 diffuse_light = kd * amount_diffuse_light * id;
//    diffuse_light /= attenuation;


//	//SpecularLight
//    float ks = 1.0f;
//    float3 is = float3(1.0f, 1.0f, 1.0f);
//    float3 direction_to_camera = -normalize(campos.xyz - P.xyz); //ī�޶����Ѻ��ǹ���
//    float3 reflected_light = reflect(direction_point_light.xyz, normalize(input.normal0).xyz);
//    float shininess = 100.0f;
//    float amount_specular_light = pow(max(0.0f, dot(reflected_light, direction_to_camera)), shininess);

//    float3 specular_light = ks * amount_specular_light * is;
//    specular_light /= attenuation;

//    float3 final_light = ambient_light + diffuse_light + specular_light;

//    return float4(final_light, 1.0f);
//}