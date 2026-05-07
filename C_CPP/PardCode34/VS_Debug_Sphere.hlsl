
struct VS_OUT
{
    float3 Pos0 : POSITION;
};

//static 구성으로 해당 셰이더 로드시 정적으로 모든스레드가 공유하는 전역읽기 데이터취급을 하도록한다
// --- 정이십면체 데이터 (Bufferless 핵심) ---
static const float PHI = 1.61803398875f; // 황금비

// 12개의 정이십면체 기본 정점
static const float3 IcoVertices[12] =
{
    float3(-1, PHI, 0), float3(1, PHI, 0), float3(-1, -PHI, 0), float3(1, -PHI, 0),
    float3(0, -1, PHI), float3(0, 1, PHI), float3(0, -1, -PHI), float3(0, 1, -PHI),
    float3(PHI, 0, -1), float3(PHI, 0, 1), float3(-PHI, 0, -1), float3(-PHI, 0, 1)
};

// 20개의 삼각형 면을 구성하는 인덱스 (총 60개)
static const uint IcoIndices[60] =
{
    0, 11, 5, 0, 5, 1, 0, 1, 7, 0, 7, 10, 0, 10, 11,
    1, 5, 9, 5, 11, 4, 11, 10, 2, 10, 7, 6, 7, 1, 8,
    3, 9, 4, 3, 4, 2, 3, 2, 6, 3, 6, 8, 3, 8, 9,
    4, 9, 5, 2, 4, 11, 6, 2, 10, 8, 6, 7, 9, 8, 1
};

//ControlPoint(60개, 십이방체구성)을 각 정점별로 제어점을 크기1로 정규화해 덮개셰이더로 보낸다
VS_OUT vsmain(uint vID : SV_VertexID)
{
    VS_OUT output;
    // 초기 정이십면체 정점 (정규화하여 반지름 1인 구체 초기 형태 생성)
    output.Pos0 = normalize(IcoVertices[IcoIndices[vID]]);
    return output;
}