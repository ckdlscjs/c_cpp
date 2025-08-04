Texture2D Texture : register(t0); //psset���μ����Ͽ���
sampler TextureSampler : register(s0); //���������ʾƵ� �⺻���÷��� ��������0�� ���õȴ�

struct PS_INPUT
{
    float4 pos0 : SV_POSITION;
    float4 tex0 : TEXCOORD0;
    float4 normal0 : NORMAL0;
    float4 pos1 : WORLDP0;
};

cbuffer Light_Directional : register(b0)
{
    float4 ld_ambient;
    float4 ld_diffuse;
    float4 ld_specular;
    float3 ld_dir;
    float ld_shiness;
};

cbuffer Light_Point : register(b1)
{
    float4 lp_ambient;
    float4 lp_diffuse;
    float4 lp_specular;
    float3 lp_pos;
    float lp_shiness;
    
    float lp_att_a0;
    float lp_att_a1;
    float lp_att_a2;
    float lp_range;
};

cbuffer Light_Spot : register(b2)
{
    float4 ls_ambient;
    float4 ls_diffuse;
    float4 ls_specular;
    
    float3 ls_dir;
    float ls_padd1;
    
    float3 ls_pos;
    float ls_shiness;
    
    float ls_att_a0;
    float ls_att_a1;
    float ls_att_a2;
    float ls_range;
    
    float ls_spot;
    float ls_cosOuter;
    float ls_cosInner;
    float ls_padd2;
};

cbuffer CB_Campos : register(b5)
{
    float4 campos;
};

float4 DirectionalLight(float3 L, float3 N, float3 R, float3 V)
{
    float4 D_A = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 D_D = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 D_S = float4(0.0f, 0.0f, 0.0f, 0.0f);
    //ambient
    D_A = ld_ambient;                            
    
    //diffuse
    float Kd = max(dot(L, N), 0.0f); //max(L dot N, 0)          //��ġ���������
    D_D = Kd * ld_diffuse;

    //specular
    float Ks = pow(max(dot(R, V), 0.0f), ld_shiness);           //�ݻ纤�Ϳ� ���������� ��ġ���� ���ؼ� ǥ���� ���õ��� �������Ͽ� ���Ѵ�
    D_S = Kd * Ks * ld_specular;                                //Kd, �� ǥ�鿡���� ��������� ���� �ùٸ� ������������� �ݿ���Ų��
    return D_A + D_D + D_S;
}

float4 PointLight(float3 L, float D, float3 N, float3 R, float3 V)
{
    float4 P_A = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 P_D = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 P_S = float4(0.0f, 0.0f, 0.0f, 0.0f);
    //ambient
    P_A = lp_ambient;
    
    //diffuse,
    float Kd = max(dot(L, N), 0.0f); //max(L dot N, 0)
    P_D = Kd * lp_diffuse;
    
    //specular
    float Ks = pow(max(dot(R, V), 0.0f), lp_shiness);
    P_S = Kd * Ks * lp_specular;
    
    float att = 1.0f / dot(float3(lp_att_a0, lp_att_a1, lp_att_a2), float3(1.0f, D, D * D));    //������, a0 + a1d + a2d^2
    return P_A + (P_D + P_S) * att;                                                             //Ambient�� �׳� �߻�ȴ�
}

float4 SpotLight(float3 L, float D, float3 N, float3 R, float3 V)
{
    float4 S_A = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 S_D = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 S_S = float4(0.0f, 0.0f, 0.0f, 0.0f);
    //ambient
    S_A = ls_ambient;
    
    //diffuse,
    float Kd = max(dot(L, N), 0.0f); //max(L dot N, 0)
    S_D = Kd * ls_diffuse;
    
    //specular
    float Ks = pow(max(dot(R, V), 0.0f), ls_shiness);
    S_S = Kd * Ks * ls_specular;
    
    //spot
    float match = max(dot(normalize(ls_dir), -L), 0.0f);
    float val = 0.0f;
    if (match >= ls_cosInner)       // ���� ����, �ִ� ���
        val = 0.5f;
    if (match < ls_cosOuter)        // �ٱ� ���� ��, �� ����
        val = 0.0f;
    else                            // �ٱ� ���԰� ���� ���� ����, ���� ����
        val = (match - ls_cosOuter) / (ls_cosInner - ls_cosOuter); 
    float spot = pow(val, ls_spot); //��ġ���� ������� ���̰��� ������ȭ
    float att = 1.0f / dot(float3(ls_att_a0, ls_att_a1, ls_att_a2), float3(1.0f, D, D * D));
    return (S_A + (S_D + S_S) * att) * spot;
}
float4 psmain(PS_INPUT input) : SV_Target
{
    float4 M = Texture.Sample(TextureSampler, input.tex0.xy);   //�ؽ��İ�
    float4 P = input.pos1;                                      //��ȯ�� ����������
    
    /*
    //DirectionalLight
    float3 L = -ld_dir;                                         //���������ϴº���
    float3 N = normalize(input.normal0.xyz);                    //������������ ����ȭ
    float3 R = reflect(ld_dir, N);                              //�ݻ纤��(�Ի纤��, ������ ���ڷλ��)
    float3 V = normalize(campos.xyz - P.xyz);                   //���������ϴº���
    return DirectionalLight(L, N, R, V) * M;                    //�������� �ؽ����÷��� ���ص� �й��Ģ������ ��������� ���´�
    */
    /*
    //PointLight
    float3 L = lp_pos - P.xyz;                                  //����, �������� ���������ϴ¹���
    float D = length(L);                                    
    if (D > lp_range)                                           //���� �������� �ָ��������
        return float4(0.0f, 0.0f, 0.0f, 0.0f);
    L /= D;                                                     //����ȭ
    float3 N = normalize(input.normal0.xyz);
    float3 R = reflect(-L, N);                                  //���� �Ի纤��(-L)
    float3 V = normalize(campos.xyz - P.xyz);
    return PointLight(L, D, N, R, V) * M;
    */
    ///*
    //SpotLight
    float3 L = ls_pos - P.xyz;
    float D = length(L);
    if (D > ls_range)
        return float4(0.0f, 0.0f, 0.0f, 0.0f);
    L /= D;
    float3 N = normalize(input.normal0.xyz);
    float3 R = reflect(-L, N);
    float3 V = normalize(campos.xyz - P.xyz);
    return SpotLight(L, D, N, R, V) * M;
    //*/
}