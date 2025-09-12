/*
DirectionalLight
L -> �����������ϴº��� (�����ѹ���)
N -> ��������
R -> �ݻ纤��(�����������º����ǹݻ纤��)
V -> ���������ϴº���
shiness -> ǥ���Ǳ��õ�
Ambient = Ma (Material Ambient)
Diffuse = Kd (L dot N, clamp0~1) * Md (MaterialDiffuse)
Specular = Kd(��ġ��üũ) * Ks(pow((R dot V, clamp0~1), shiness) * Ms(Material Specular)

DirectionalLight = Ambient + Diffuse + Specular
*/

cbuffer Light_Directional : register(b0)
{
    float4 ld_ambient;          
    float4 ld_diffuse;
    float4 ld_specular;
    float3 ld_dir;              //����
    float ld_shiness;           //���õ�
};

float4 DirectionalLight(float3 L, float3 N, float3 R, float3 V)
{
    float4 D_A = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 D_D = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 D_S = float4(0.0f, 0.0f, 0.0f, 0.0f);
    //ambient
    D_A = ld_ambient;
    
    //diffuse
    float Kd = max(dot(L, N), 0.0f);                    //max(L dot N, 0)  //��ġ���������
    D_D = Kd * ld_diffuse;

    //specular
    float Ks = pow(max(dot(R, V), 0.0f), ld_shiness);   //�ݻ纤�Ϳ� ���������� ��ġ���� ���ؼ� ǥ���� ���õ��� �������Ͽ� ���Ѵ�
    D_S = Kd * Ks * ld_specular;                        //Kd, �� ǥ�鿡���� ��������� ���� �ùٸ� ������������� �ݿ���Ų��
    return D_A + D_D + D_S;
}

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
    float3 lp_pos;          //��������ġ
    float lp_shiness;       //���õ�
    
    float lp_att_a0;        //����ɼ�
    float lp_att_a1;
    float lp_att_a2;
    float lp_range;         //���޹���
};

float4 PointLight(float3 L, float D, float3 N, float3 R, float3 V)
{
    float4 P_A = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 P_D = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 P_S = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float ratioD = D / lp_range;
    //ambient
    P_A = lp_ambient;
    
    //diffuse,
    float Kd = max(dot(L, N), 0.0f);                    //max(L dot N, 0)
    P_D = Kd * lp_diffuse;
    
    //specular
    float Ks = pow(max(dot(R, V), 0.0f), lp_shiness);
    P_S = Kd * Ks * lp_specular;
    
    float att = 1.0f / dot(float3(lp_att_a0, lp_att_a1, lp_att_a2), float3(1.0f, ratioD, ratioD * ratioD)); //������, a0 + a1d + a2d^2
    return P_A + (P_D + P_S) * att; //Ambient�� �׳� �߻�ȴ�
}

/*
SpotLight
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
Spot = (-L dot ls_dir, clamp0~1)->match ��ġ���� ������ ���������Ѱ��� ������ = pow(match, ls_spot)

SpotLight = Spot(Ambient + (Diffuse + Specular) / attenuation)
*/

cbuffer Light_Spot : register(b2)
{
    float4 ls_ambient;
    float4 ls_diffuse;
    float4 ls_specular;
    
    float3 ls_dir;          //����
    float ls_padd1;         //�е���
    
    float3 ls_pos;          //��������ġ
    float ls_shiness;       //���õ�
    
    float ls_att_a0;        //����ɼ�
    float ls_att_a1;
    float ls_att_a2;
    float ls_range;         //���޹���
        
    float ls_spot;          //�����°�
    float ls_cosOuter;      //�������ִ밢(cosf, rad), ����������� �񱳸� ���� cos�� ���
    float ls_cosInner;      //�������߽ɰ�(cosf, rad), ����������� �񱳸� ���� cos�� ���
    float ls_padd2;         //�е���
};

float4 SpotLight(float3 L, float D, float3 N, float3 R, float3 V)
{
    float4 S_A = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 S_D = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 S_S = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float ratioD = D / ls_range;
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
    /*
    if (match >= ls_cosInner)       // ���� ����, �ִ� ���
        val = 1.0f;
    if (match < ls_cosOuter)        // �ٱ� ���� ��, �� ����
        val = 0.0f;
    else // �ٱ� ���԰� ���� ���� ����, ���� ����
        val = (match - ls_cosOuter) / (ls_cosInner - ls_cosOuter);
    */
    val = saturate((match - ls_cosOuter) / (ls_cosInner - ls_cosOuter)); //saturate, 0~1, same
    float spot = pow(val, ls_spot); //��ġ���� ������� ���̰��� ������ȭ
    float att = 1.0f / dot(float3(ls_att_a0, ls_att_a1, ls_att_a2), float3(1.0f, ratioD, ratioD * ratioD));
    return (S_A + (S_D + S_S) * att) * spot;
}