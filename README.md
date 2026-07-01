# 🛠️ DX11 기반 ECS 아키처 캐시 최적화 렌더러 - 소스코드 포함 버전 (c_cpp)

본 프로젝트는 DirectX 11 API와 C++17 표준을 활용하여 캐시 성능 및 렌더링 파이프라인 바인딩을 최적화한 3D 그래픽스 렌더러 엔진 프로젝트임.

학습 및 설계의 기반으로 **이득우의 게임수학**, **DirectX 11 물방울책 (Introduction to 3D Game Programming with Direct3D 11 - Frank D. Luna 저)**, **RasterTek DirectX 11 Tutorial**, **PardCode DirectX 11 Tutorial**의 핵심 그래픽스 및 선형대수 인프라를 바닥부터 심도 있게 학습하여 내재화하였으며, 과거 팀 프로젝트인 **맵툴(MapTool)** 및 **게임프로젝트(LoLShinSouls)** 개발 과정에서 마주했던 기술적 한계(객체지향 설계의 캐시 미스 오버헤드 및 단편적인 렌더링 파이프라인 제어)에 대한 깊은 아쉬움과 갈증을 기반으로, 엔진 전반의 메모리 연속성 확보 및 64비트 정렬 키 기반 다중 렌더패스 등 고도화된 최적화 렌더패스 엔진을 독자적으로 설계 및 구축하며 기술적 도약을 실천함.

---

## 00 [ 프로젝트 개요 ]

* **문제인식 및 개발배경**: 
  과거 객체지향(OOP) 디자인 패턴을 활용해 개발했던 이전 DirectX 프로젝트에서의 경험이 본 엔진 아키텍처의 근본적인 출발점이 되었음. 당시 수천 개의 동적 오브젝트를 배치했을 때 발생했던 메모리 파편화와 드로우콜 병목 한계를 직접 체감하며 단편적인 기능 구현을 넘어 엔진 전반을 관통하는 최적화 설계의 필요성을 절감하였고, 이에 대한 해결책을 설계에 직접 녹여내고자 개발을 가동함.
* **프로젝트 목표**: 
  - 싱글톤 보일러플레이트 제거 및 자원 수명 관리 일원화 (CRTP 및 RAII 설계)
  - 16KB 청크 단위 연속 메모리 배열을 통한 CPU 데이터 캐시 히트율 극대화 (DOD ECS 설계)
  - 64비트 정렬 키 조립 및 XOR 비트 연산을 통한 D3D11 파이프라인 리바인딩 최소화 (렌더패스 렌더 큐 최적화)
  - 다각형 정밀 충돌 교차 및 스키닝 연산 병목의 GPU 이관 (Compute Shader Möller-Trumbore 가속)
* **전체 아키텍처 흐름**: 
  `AppWindow` ➡️ `EngineSystem` (자원 로드/생성) ➡️ `ECSSystem` (엔티티/컴포넌트 생성)의 초기화 단계를 거치며, 매 프레임 각 시스템의 `Frame` (상태 갱신) 및 `Render` (렌더 명령 제출) 단계를 유기적으로 기동하여 연산 파이프라인 오버헤드를 제어함.

---

## 01 [ 엔진 아키텍처 및 기반 시스템 ]

### 1. CRTP 기반 시스템 설계
* **파일명**: [C_CPP/PardCode34/BaseSystem.h](https://github.com/ckdlscjs/c_cpp/blob/main/C_CPP/PardCode34/BaseSystem.h)
* **기능 개요**: 엔진 전반에 존재하는 매니저 클래스의 싱글톤 구성을 일관된 템플릿으로 통제하고, D3D11 객체 자원들의 생성/소멸 수명 주기를 명확히 구성하여 누수를 원천 방지함.
* **코드 상세 분석**:
  - C++의 CRTP(Curiously Recurring Template Pattern) 패턴을 구현해 싱글톤 메커니즘의 중복 보일러플레이트를 완전 제거함.
  - 인스턴스를 호출하는 최초 시점에 정적 객체가 할당되는 지연 생성(Lazy Initialization) 구조를 채택해 멀티스레드 환경에서도 안전한 초기화를 수립함.
  - 생성자 및 소멸자 수명 주기를 명확히 일원화하여 RAII 패턴을 준수하도록 설계함.
* **💻 핵심 구현 코드**:
```cpp
template<typename T>
class BaseSystem
{
protected:
    BaseSystem();
    virtual ~BaseSystem();
    BaseSystem(const BaseSystem&) = delete;            // 복사 생성 금지
    BaseSystem& operator=(const BaseSystem&) = delete; // 대입 연산 금지
    BaseSystem(BaseSystem&&) = delete;                 // 이동 생성 금지
public:
    static T& GetInstance();
};

template<typename T>
inline BaseSystem<T>::BaseSystem() {}

template<typename T>
inline BaseSystem<T>::~BaseSystem() {}

template<typename T>
inline T& BaseSystem<T>::GetInstance()
{
    static T instance;
    return instance;
}
```

### 2. 수학 라이브러리 정의 (Vector, Matrix, Quaternion)
* **파일명**: [C_CPP/PardCode34/CommonMath.h](https://github.com/ckdlscjs/c_cpp/blob/main/C_CPP/PardCode34/CommonMath.h)
* **기능 개요**: DirectXMath의 SIMD 계산 레지스터 연산 성능을 확보하면서, 디버깅 첨자 접근 및 짐벌락 회피 회전 연산을 고속 수행하는 수학 래퍼 라이브러리를 구축함.
* **코드 상세 분석**:
  - `__declspec(align(16))` 하드웨어 정렬 지시어를 수립하여 SIMD 메모리 적재 효율을 극한으로 보존함.
  - 공용체(`union`)를 적용해 내부 연산은 가속화하되 CPU 단에서 행렬 성분 및 첨자(`_11 ~ _44`)에 조밀 접근이 가능하도록 구성함.
  - 사원수(`Quaternion`) 클래스를 구현하여 본 애니메이션 키프레임 간의 구면 선형 보간(Slerp) 계산 인프라를 수립함.
* **💻 핵심 구현 코드**:
```cpp
__declspec(align(16)) struct Vector4 {
public:
    inline Vector4() : m_vec(DirectX::XMVectorZero()) {} 
    inline Vector4(float x, float y, float z, float w) : m_vec(DirectX::XMVectorSet(x, y, z, w)) {}
    inline DirectX::XMVECTOR ToXMVECTOR() const { return m_vec; }
private:
    DirectX::XMVECTOR m_vec; // SIMD 레지스터 16바이트 정렬 직접 매핑
};

__declspec(align(16)) struct Matrix4x4 {
public:
    union
    {
        DirectX::XMMATRIX m_mat;
        struct
        {
            float _11, _12, _13, _14;
            float _21, _22, _23, _24;
            float _31, _32, _33, _34;
            float _41, _42, _43, _44;
        };
    };
    inline Matrix4x4() : m_mat(DirectX::XMMatrixIdentity()) {}
    inline DirectX::XMMATRIX ToXMMATRIX() const { return m_mat; }
};
```

### 3. 가상자원 및 D3D Wrapper 정의
* **파일명**: [C_CPP/PardCode34/ResourceSystem.h](https://github.com/ckdlscjs/c_cpp/blob/main/C_CPP/PardCode34/ResourceSystem.h), [C_CPP/PardCode34/Geometry.h](https://github.com/ckdlscjs/c_cpp/blob/main/C_CPP/PardCode34/Geometry.h)
* **기능 개요**: 텍스처, 기하(Assimp), 머티리얼 등의 원시 에셋 데이터를 FNV-1a 해싱 기반 캐싱 구조에 등록하여 에셋 중복 로드를 O(1)에 원천 차단함.
* **코드 상세 분석**:
  - 완벽 포워딩(`std::forward`) 기반의 캐싱 팩토리 메소드(`CreateResource`)를 구현하여 에셋 중복 로드를 사전에 제어함.
  - Assimp SDK를 거쳐 aiScene 노드 계층을 재귀 순회(`ParseNode`)하고 정점 FNV-1a 해싱 룩업을 수행해 중복 정점 제거(인덱싱 테이블 매핑)를 적용함.
* **💻 핵심 구현 코드**:
```cpp
// ResourceSystem.h: 가변 인자 기반 캐싱 팩토리 정의
template<typename T, typename... Types>
T* ResourceSystem::CreateResource(const std::wstring& szFile, Types&&... args)
{
    size_t hash = Hasing_wstring(szFile);
    if (m_Resources.find(hash) != m_Resources.end()) 
        return static_cast<T*>(m_Resources[hash]);

    T* newResource = new T(hash, szFile, std::forward<Types>(args)...);
    m_Resources[hash] = newResource;
    return newResource;
}

// Geometry.cpp: Assimp 노드 재귀 순회 및 중복 정점 해시 룩업 테이블 인덱싱
inline void Geometry::ParseMesh(const aiScene* scene, const aiMesh* mesh, 
                                 std::map<UINT, std::unordered_map<Vertex_PTNTB_Skinned, UINT, Vertex_PTNTB_Skinned_Hash>>& verticesIdentical) {
    if (mesh->mNumVertices <= 0) return;
    UINT matIdx = mesh->mMaterialIndex;
    std::vector<UINT> vertexLookup(mesh->mNumVertices);
    
    for (UINT i = 0; i < mesh->mNumVertices; i++) {
        Vertex_PTNTB_Skinned vertex = {};
        // [정점 물리 데이터 적재]
        
        auto iter = verticesIdentical[matIdx].find(vertex);
        if (iter == verticesIdentical[matIdx].end()) {
            verticesIdentical[matIdx][vertex] = m_verticesByMaterial[matIdx].size();
            m_verticesByMaterial[matIdx].push_back(vertex);
        }
        vertexLookup[i] = verticesIdentical[matIdx][vertex];
    }
    
    for (UINT i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (UINT j = 0; j < face.mNumIndices; j++)
            m_indicesByMaterial[matIdx].push_back(vertexLookup[face.mIndices[j]]);
    }
}
```

---

## 02 [ 데이터지향 설계(DOD) 기반 ECS 아키텍처 ]

### 1. OOP 기반의 한계성과 DOD 기반으로의 전환
* **파일명**: [C_CPP/PardCode34/ECSSystem.h](https://github.com/ckdlscjs/c_cpp/blob/main/C_CPP/PardCode34/ECSSystem.h), [C_CPP/PardCode34/Components.h](https://github.com/ckdlscjs/c_cpp/blob/main/C_CPP/PardCode34/Components.h)
* **기능 개요**: 힙 공간에 분산 배치되어 CPU 캐시 미스를 초래하는 OOP의 개별 객체 구조를 극소화하고, 순수 컴포넌트 데이터의 조밀 선형 메모리 배열 배치를 수립함.
* **코드 상세 분석**:
  - `Entity`는 순수 식별 인덱스 주소만 보유하며, `Component`는 런타임 타입 마스크 지수를 고유 발급받아 비트 마스킹으로 관리됨.
  - 가변인자 템플릿 `CreateArchetype`에서 컴파일 타임 C++17 폴드 표현식을 통해 컴포넌트 청크를 분팩 생성함.
* **💻 핵심 구현 코드**:
```cpp
// ECSSystem.h: C++17 폴드 표현식을 이용한 각 컴포넌트 자동 등록 및 청크 전개
template<typename ...Comps>
inline ArchetypeKey ECSSystem::CreateArchetype()
{
    ArchetypeKey key = GetArchetypeKey<Comps...>();
    if (m_Archetypes.find(key) != m_Archetypes.end()) 
        return key;
    m_Archetypes[key] = new Archetype(GetMaxComponentSize<Comps...>());
    
    (m_Archetypes[key]->RegisterComponent<Comps>(), ...);
    return key;
}
```

### 2. Archetype & ChunkData 구성
* **파일명**: [C_CPP/PardCode34/Archetype.h](https://github.com/ckdlscjs/c_cpp/blob/main/C_CPP/PardCode34/Archetype.h)
* **기능 개요**: 아키타입 내 이종 컴포넌트 배열들을 16KB(16384바이트)의 물리 연속 크기제약 청크 블록으로 구성하여 CPU 데이터 캐시 히트율을 확보함.
* **코드 상세 분석**:
  - `ComponentChunk` 추상 클래스를 기반으로 상속 템플릿 클래스 `ChunkData<T>`를 구성하여 컴포넌트 타입별 고속 복사 및 Swap 연산을 통제함.
  - 아키타입 내에서 최대 컴포넌트 크기로 용량을 역산해 청크당 최대 인덱싱 개수를 일원화함.
* **💻 핵심 구현 코드**:
```cpp
class ComponentChunk
{
public:
    virtual ~ComponentChunk() = default;
    virtual void Swap(size_t srcCol, ComponentChunk* destChunk, size_t destCol) = 0;
    virtual void SwapAndPop(size_t chunkIdx, ComponentChunk* srcChunk) = 0;
    virtual void AddChunk() = 0;
    virtual size_t GetCount() const = 0;
};
```

### 3. Sparse/Dense(희소배열) 기반 구성 및 Swap & Pop
* **파일명**: [C_CPP/PardCode34/Archetype.h](https://github.com/ckdlscjs/c_cpp/blob/main/C_CPP/PardCode34/Archetype.h) (DeleteComponent), [C_CPP/PardCode34/ECSSystem.cpp](https://github.com/ckdlscjs/c_cpp/blob/main/C_CPP/PardCode34/ECSSystem.cpp) (DeleteEntity)
* **기능 개요**: 임의 개체 소멸 시 시프트 복사 연산($O(N)$) 병목을 회피하기 위해 최후방 청크 데이터와의 스왑을 통한 O(1) 완전 삭제를 적용하고 룩업 포인터를 동기화함.
* **코드 상세 분석**:
  - `DeleteComponent` 기동 시 대상 청크 슬롯들을 최후방(back) 청크의 마지막 요소와 맞바꾼 후 제거하여 O(1) 삭제 효율을 유지함.
  - 이동된 실제 엔티티들의 룩업 매핑이 어긋나지 않도록 `m_LookupTable`(Sparse)과 `m_Entitys`(Dense)를 역추적 교정함.
* **💻 핵심 구현 코드**:
```cpp
// Archetype.h: 아키타입 내 컴포넌트들의 일괄 Swap & Pop 지휘
inline size_t Archetype::DeleteComponent(size_t idxRow, size_t idxCol)
{
    auto& chunks = m_Components[*m_ComponentsLookup.begin()];
    
    for (const auto& key : m_ComponentsLookup)
        m_Components[key][idxRow]->SwapAndPop(idxCol, m_Components[key].back());
 
    if (!chunks.empty() && chunks.back()->GetCount() == 0)
    {
        for (const auto& key : m_ComponentsLookup)
        {
            delete m_Components[key].back();
            m_Components[key].pop_back();
        }
    }
    return m_ChunksLookup.back();
}

// ECSSystem.cpp: 삭제로 인해 순간이동한 후방 엔티티의 물리 청크 정보(행/열) 교정 동기화
void ECSSystem::DeleteEntity(size_t lookupIdx)
{
    size_t delEntityIdx = FindEntity(lookupIdx);
    Entity delEntity = m_Entitys[delEntityIdx];
    m_LookupTable[lookupIdx] = _HashNotInitialize; 
 
    size_t movedLookupIdx = m_Archetypes[delEntity.m_Key]->DeleteComponent(delEntity.m_IdxRow, delEntity.m_IdxCol);
 
    if (movedLookupIdx != lookupIdx)
    {
        size_t movEntityIdx = FindEntity(movedLookupIdx);
        m_Entitys[movEntityIdx].m_IdxRow = delEntity.m_IdxRow;
        m_Entitys[movEntityIdx].m_IdxCol = delEntity.m_IdxCol;
    }
 
    size_t lastEntityIdx = m_Entitys.size() - 1;
    if (delEntityIdx != lastEntityIdx)
    {
        m_Entitys[delEntityIdx] = std::move(m_Entitys[lastEntityIdx]);
        m_LookupTable[m_Entitys[delEntityIdx].m_IdxLookup] = delEntityIdx; 
    }
    m_Entitys.pop_back();
}
```

### 4. 비트연산 기반 컴포넌트 데이터 조회 (ArchetypeQuery)
* **파일명**: [C_CPP/PardCode34/ECSSystem.cpp](https://github.com/ckdlscjs/c_cpp/blob/main/C_CPP/PardCode34/ECSSystem.cpp) (QueryArchetypes)
* **기능 개요**: 수십 가지 아키타입 중 특정 시스템이 요구하는 컴포넌트 마스크 키를 비트셋 AND 연산으로 O(1) 필터링하여 순회 속도를 극대화함.
* **코드 상세 분석**:
  - `QueryArchetypes` 내에서 비트 연산(`(iter.first & key) == key`)을 적용해 타겟 컴포넌트 세트를 지닌 아키타입들만 즉시 추출함.
* **💻 핵심 구현 코드**:
```cpp
std::vector<Archetype*> ECSSystem::QueryArchetypes(ArchetypeKey key)
{
    std::vector<Archetype*> archetypes;
    for (auto& iter : m_Archetypes)
    {
        if ((iter.first & key) == key && !iter.second->IsEmpty())
            archetypes.push_back(iter.second);
    }
    return archetypes;
}
```

### 5. 청크 정렬 업데이트
* **파일명**: [C_CPP/PardCode34/ECSSystem.cpp](https://github.com/ckdlscjs/c_cpp/blob/main/C_CPP/PardCode34/ECSSystem.cpp) (UpdateSwapChunk)
* **기능 개요**: 매 프레임 활성 상태 엔티티들을 청크 메모리 최후방 및 최전방 영역으로 밀집 스왑 정렬하여 순회 대상 쿼리 검색량을 극대화함.
* **코드 상세 분석**:
  - 교환 과정 중 `transferIdx`를 아키타입 내부에 셋업하여 정합성이 보정된 밀집 영역 세그먼트만 선별 순회하도록 제어함.
* **💻 핵심 구현 코드**:
```cpp
void ECSSystem::UpdateSwapChunk(const std::vector<std::pair<size_t, size_t>>& RowCols, size_t startIdx, Archetype* archetype)
{
    if (RowCols.empty()) return;
    size_t idx = archetype->GetAllChunkCount() - 1;
    size_t capacity = archetype->GetCapacity_Chunks();
    
    for(auto iter = RowCols.rbegin(); iter != RowCols.rend(); iter++)
    {
        size_t srcRow = iter->first;
        size_t srcCol = iter->second;
        size_t destRow = idx / capacity;
        size_t destCol = idx % capacity;
        
        if (srcRow != destRow || srcCol != destCol)
        {
            auto src_dest = archetype->SwapChunkData(srcRow, srcCol, destRow, destCol);
            size_t srcEntityIdx = FindEntity(src_dest.first);
            size_t destEntityIdx = FindEntity(src_dest.second);
            
            m_Entitys[srcEntityIdx].m_IdxRow = destRow;
            m_Entitys[srcEntityIdx].m_IdxCol = destCol;

            m_Entitys[destEntityIdx].m_IdxRow = srcRow;
            m_Entitys[destEntityIdx].m_IdxCol = srcCol;
        }
        idx--;
    }
    archetype->m_transfer_row = startIdx / capacity;
    archetype->m_transfer_col = startIdx % capacity;
}
```

---

## 03 [ Engine/Render System 및 RenderPass 구성 ]

### 1. EngineSystem 텍스처 해시 캐싱
* **파일명**: [C_CPP/PardCode34/EngineSystem.cpp](https://github.com/ckdlscjs/c_cpp/blob/main/C_CPP/PardCode34/EngineSystem.cpp) (CreateTexture)
* **기능 개요**: 텍스처 자원의 디스크 경로를 FNV-1a 해시 키로 상호 합성하여 중복 생성 및 파일 중복 I/O를 캐싱 테이블 룩업으로 차단함.
* **코드 상세 분석**:
  - 이미 동일 해시 자원이 맵에 로드되어 존재할 경우 추가 생성 없이 즉각 복사본 해시 키를 조기 리턴함.
* **💻 핵심 구현 코드**:
```cpp
size_t EngineSystem::CreateTexture(const std::wstring& szFilePath)
{
    size_t hash_path = HashCombine(szFilePath);
    if (m_pCSVs.find(hash_path) != m_pCSVs.end())
        return hash_path;

    ScratchImage image;
    _ResourceSystem.LoadTexture(szFilePath, image);
    // [D3D11 텍스처 리소스 뷰 생성 및 저장]
    return hash_path;
}
```

### 2. 64비트 정렬 키 생성 및 조립
* **파일명**: [C_CPP/PardCode34/RenderSystem.cpp](https://github.com/ckdlscjs/c_cpp/blob/main/C_CPP/PardCode34/RenderSystem.cpp) (GenerateRenderPassHash)
* **기능 개요**: 상태 전환 오버헤드 가중치에 맞춰 5대 세그먼트를 결합한 단일 64비트 정수 키(`_RPKey`)를 가동하여 GPU 파이프라인의 불필요한 상태 교체를 차단함.
* **코드 상세 분석**:
  - 투명 패스(`E_RenderPass::Transparent`)의 경우 Back-to-Front 정렬을 가동하도록 거리 값 세그먼트 비트를 반전 합성함.
* **💻 핵심 구현 코드**:
```cpp
_RPKey RenderSystem::GenerateRenderPassHash(uint32_t hashPass, uint32_t hashShaders, uint32_t hashStates, uint32_t hashResources, uint32_t hashDist)
{
    _RPKey key = 0;
    key |= (static_cast<uint64_t>(hashPass) & (UINT)0xF) << 60;
    key |= (static_cast<uint64_t>(hashShaders) & (UINT)0xFFFF) << 44;
    key |= (static_cast<uint64_t>(hashStates) & (UINT)0xFF) << 36;
    key |= (static_cast<uint64_t>(hashResources) & (UINT)0xFFFF) << 20;
    
    if (static_cast<uint32_t>(E_RenderPass::Transparent) == hashPass)
        hashDist = 0xFFFFF - hashDist;
        
    key |= (static_cast<uint64_t>(hashDist) & (UINT)0xFFFFF);
    return key;
}
```

### 3. XOR 비트 연산 기반 지연 바인딩 최소화 루프
* **파일명**: [C_CPP/PardCode34/RenderSystem.cpp](https://github.com/ckdlscjs/c_cpp/blob/main/C_CPP/PardCode34/RenderSystem.cpp) (ExecuteRenderPass 루프)
* **기능 개요**: 렌더러 명령 큐 순회 시, 직전 정렬 키와의 비트 XOR 연산(`keyCur ^ keyPrev`)을 수행해 차이가 발생한 D3D11 파이프라인 영역만 리바인딩함.
* **코드 상세 분석**:
  - 시프트 오프셋 분기(60, 44, 36, 20)를 통해 렌더 타겟, 셰이더, States, Buffer 리소스 바인딩을 선택적 갱신하여 드라이버 오버헤드를 극소화함.
* **💻 핵심 구현 코드**:
```cpp
// (ExecuteRenderPass 내부 루프 세그먼트)
_RPKey keyPrev = _HashNotInitialize;
for (const auto& renderItem : m_hRP_CommandQueue)
{
    _RPKey keyCur = renderItem.sortKey;
    size_t diff = keyCur ^ keyPrev;
    uint32_t curPass = (keyCur >> 60) & 0xF;
 
    if (diff >> 60)
    {
        SetRS_Viewport(&_EngineSystem.m_vp_BB);
        switch (curPass)
        {
            case (uint32_t)E_RenderPass::Opaque:
                SetOM_RenderTargets({ _EngineSystem.m_hash_Gbuffer_Position, _EngineSystem.m_hash_Gbuffer_Normal, _EngineSystem.m_hash_Gbuffer_Albedo }, _EngineSystem.m_hash_DSView_Quad);
                break;
        }
    }
    // [diff >> 44 (Shader), diff >> 36 (State), diff >> 20 (Resource) 분기 갱신 및 Draw]
    keyPrev = keyCur;
}
```

---

## 04 [ 가시성/충돌 판정 최적화 및 그래픽스 파이프라인 ]

### 1. 선형 깊이 분포 절두체 평면 생성
* **파일명**: [C_CPP/PardCode34/CollisionSystem.cpp](https://github.com/ckdlscjs/c_cpp/blob/main/C_CPP/PardCode34/CollisionSystem.cpp) (Frustum 생성자)
* **기능 개요**: 원근 투영 변환의 비선형 분포를 선형 깊이로 교정 보정한 투영 행렬을 빌드하고 전치 승산을 수행해 6개 월드 절두체 평면식을 유도함.
* **코드 상세 분석**:
  - 뷰 행렬과 깊이 분포 보정 투영 행렬을 승산하고 전치한 행렬(`Transpose()`)을 기반으로 6개 가시 영역 평면 식(`m_planes`)을 도출함.
* **💻 핵심 구현 코드**:
```cpp
Frustum::Frustum(float screenDepth, const Matrix4x4& matView, const Matrix4x4& matProj)
{
    Matrix4x4 matLinearProj = matProj;
    float zMinimum = -matLinearProj[3].GetZ() / matLinearProj[2].GetZ();
    float r = screenDepth / (screenDepth - zMinimum);
    matLinearProj[2].SetZ(r);             
    matLinearProj[3].SetZ(-r / zMinimum);  
 
    Matrix4x4 matFinalTranspose = (matView * matLinearProj).Transpose();
    m_planes[0] = matFinalTranspose[3] + matFinalTranspose[0]; // Left
    m_planes[1] = matFinalTranspose[3] - matFinalTranspose[0]; // Right
    m_planes[2] = matFinalTranspose[3] - matFinalTranspose[1]; // Up
    m_planes[3] = matFinalTranspose[3] + matFinalTranspose[1]; // Bottom
    m_planes[4] = matFinalTranspose[3] + matFinalTranspose[2]; // Near
}
```

### 2. 계산 셰이더 기반 GPU Möller-Trumbore 가속
* **파일명**: [C_CPP/PardCode34/CS_Collision_Triangle.hlsl](https://github.com/ckdlscjs/c_cpp/blob/main/C_CPP/PardCode34/CS_Collision_Triangle.hlsl)
* **기능 개요**: 다량의 스키닝 정점 계산 및 정밀 삼각형 피킹 검출을 GPU 다중 코어 스레드로 비동기 병렬 연산 처리함.
* **코드 상세 분석**:
  - `ComputeSkinning`: 계산 셰이더 상에서 본 행렬 구조화 버퍼와 연동해 4가중치 스키닝 위치 변형을 가속 처리함.
  - `Intersect`: Möller-Trumbore 알고리즘을 HLSL로 직접 가동하여 삼각형 세 꼭지점(`wv0, wv1, wv2`)과 레이 간 교차 깊이(`fDist`)를 병렬 탐색함.
* **💻 핵심 구현 코드**:
```hlsl
float3 ComputeSkinning(Vertex v)
{
    float4 animPos = float4(0, 0, 0, 0);
    float4 localPos = float4(v.pos0.xyz, 1.0f);
    if (notAnimate > _IsAnimate)
        return mul(localPos, matWorld).xyz;
        
    [unroll]
    for (int i = 0; i < 4; ++i)
        animPos += v.weights[i] * mul(localPos, matBone[v.bones[i]]);
 
    return mul(float4(animPos.xyz, 1.0f), matWorld).xyz;
}

bool Intersect(float3 rayOrigin, float3 rayDir, float3 wv0, float3 wv1, float3 wv2, out float fDist) {
    float3 e1 = wv1 - wv0;
    float3 e2 = wv2 - wv0;
    float3 s = rayOrigin - wv0;
    float3 p = cross(rayDir, e2);
    float3 q = cross(s, e1);
    float det = dot(e1, p);
    bool NoIntersection = false;
    
    if (det > _Epsilon) {
        float u = dot(s, p);
        NoIntersection = (u < 0.0f) || (u > det);
        float v = dot(rayDir, q);
        NoIntersection = NoIntersection || (v < 0.0f) || ((u + v) > det);
        float t = dot(e2, q);
        NoIntersection = NoIntersection || (t < 0.0f);
    } else if (det < -_Epsilon) {
        float u = dot(s, p);
        NoIntersection = (u > 0.0f) || (u < det);
        float v = dot(rayDir, q);
        NoIntersection = NoIntersection || (v > 0.0f) || ((u + v) < det);
        float t = dot(e2, q);
        NoIntersection = NoIntersection || (t > 0.0f);
    } else {
        NoIntersection = true;
    }
    if (NoIntersection) return false;
    fDist = dot(e2, q) * (1.0f / det);
    return true;
}

[numthreads(64, 1, 1)]
void csmain(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    if (dispatchThreadID.x >= triangleCount) return;
    uint tid = dispatchThreadID.x;
    
    float3 wv0 = ComputeSkinning(gInputVertices[tid * 3 + 0]);
    float3 wv1 = ComputeSkinning(gInputVertices[tid * 3 + 1]);
    float3 wv2 = ComputeSkinning(gInputVertices[tid * 3 + 2]);
    
    float fDist = _Infinite;
    bool isHit = Intersect(vRayOrigin, vRayDir, wv0, wv1, wv2, fDist);
 
    gOutputResults[tid].idx = tid * 3;
    gOutputResults[tid].fDist = isHit ? fDist : _Infinite;
}
```

### 3. 세부 렌더패스 D3D11 파이프라인 연동
* **파일명**: [C_CPP/PardCode34/RenderSystem.cpp](https://github.com/ckdlscjs/c_cpp/blob/main/C_CPP/PardCode34/RenderSystem.cpp), [C_CPP/PardCode34/GS_CubeMap_PTNTB.hlsl](https://github.com/ckdlscjs/c_cpp/blob/main/C_CPP/PardCode34/GS_CubeMap_PTNTB.hlsl), [C_CPP/PardCode34/DS_Debug_Sphere.hlsl](https://github.com/ckdlscjs/c_cpp/blob/main/C_CPP/PardCode34/DS_Debug_Sphere.hlsl)
* **기능 개요**: GBuffer MRT, 그림자 깊이 바인딩, 기하 셰이더 환경 큐브맵 라우팅, 스텐실 카툰 아웃라인 및 테셀레이션 구형 투영 디버깅을 기동함.
* **코드 상세 분석**:
  - **Deferred (Opaque)**: 4개의 GBuffer 텍스처 렌더 타겟을 즉각 설정(`SetOM_RenderTargets`)하여 렌더링 후 후처리 라이팅 패스에서 활용함.
  - **Shadow**: 렌더 타겟 RTV 결합을 nullptr로 구성해 깊이 텍스처만 D3D11에 바인딩하고 PCF 3x3 샘플링 계수를 계산함.
  - **CubeMap**: 기하 셰이더 내부 `gsmain`에서 `rtvIdx = idx`로 6개의 타겟 면 인덱스를 비동기 지정 방출함.
  - **Debug**: 도메인 셰이더 `dsmain` 내에서 무게중심 가중치 보간 정점을 반지름(`radius`)으로 Normalize 구형 투영하는 버퍼리스 렌더링을 연동함.
* **💻 핵심 구현 코드**:
```cpp
// [C++] Opaque MRT G-Buffer 타겟 세팅 및 바인딩
SetOM_RenderTargets(
    {
        _EngineSystem.m_hash_Gbuffer_Position, 
        _EngineSystem.m_hash_Gbuffer_Normal,   
        _EngineSystem.m_hash_Gbuffer_Albedo,   
        _EngineSystem.m_hash_Gbuffer_Specular, 
    },
    _EngineSystem.m_hash_DSView_Quad
);

// [C++] 그림자 맵 생성 시 RTV 비우기 설정
SetOM_RenderTargets(std::vector<size_t>(), _EngineSystem.m_hash_DSV_ShadowMap);
```
```hlsl
// GS_CubeMap_PTNTB.hlsl: 기하 셰이더 6개 큐브 뷰포트 인덱스 분기 방출
[maxvertexcount(18)]
void gsmain(triangle VS_OUTPUT input[3], inout TriangleStream<GS_OUTPUT> outStream)
{
    for (int idx = 0; idx < 6; idx++)
    {
        for (int v = 0; v < 3; ++v)
        {
            GS_OUTPUT output;
            output.rtvIdx = idx; // SV_RenderTargetArrayIndex 바인딩
            output.pos = mul(input[v].pos, matViewProj[idx]);
            outStream.Append(output);
        }
        outStream.RestartStrip();
    }
}

// DS_Debug_Sphere.hlsl: 테셀레이션 무게중심 보간 후 반지름(radius) 구형 투영 계산
[domain("tri")]
DS_OUT dsmain(HS_CS_OUTPUT input, float3 uvw : SV_DomainLocation, const OutputPatch<HS_OUT, 3> patch)
{
    DS_OUT output;
    float3 pos = patch[0].Pos0 * uvw.x + patch[1].Pos0 * uvw.y + patch[2].Pos0 * uvw.z;
    pos = normalize(pos) * radius;
    pos += center;
    output.pos = mul(float4(pos, 1.0f), matViewProj);
    return output;
}
```

---

## 🔗 상세개발기록 (References)

* **[1] [Transform 구성 및 정의]**(https://blog.naver.com/syl610/223922729999)
* **[2] [수학 클래스 구성 및 정의]**(https://blog.naver.com/syl610/223948444921)
* **[3] [사원수 클래스 구성 및 정의]**(https://blog.naver.com/syl610/224088711483)
* **[4] [Assimp라이브러리 임포트 및 애니메이션(Geometry 등 가상자원)]**(https://blog.naver.com/syl610/224185955904)
* **[5] [챕터1 오버뷰 및 세부 코드 스니펫]**(https://blog.naver.com/syl610/224300186028)
* **[6] [DOD기반 아키텍쳐 설계 및 Archetype 아키텍쳐 설계 및 도입배경]**(https://blog.naver.com/syl610/224092884791)
* **[7] [ECS기반 시스템 및 세부구성]**(https://blog.naver.com/syl610/224093235113)
* **[8] [ECS시스템 코드스니펫 및 세부사항 오버뷰]**(https://blog.naver.com/syl610/224306323972)
* **[9] [커맨드큐 기반 렌더패스 설계]**(https://blog.naver.com/syl610/224274490970)
* **[10] [드로우콜 인스턴싱 배치 및 최적화]**(https://blog.naver.com/syl610/224298052221)
* **[11] [EngineSys세부 구성 및 오버뷰]**(https://blog.naver.com/syl610/224309508399)
* **[12] [FBX임포트 및 스키닝애니메이션 구조]**(https://blog.naver.com/syl610/224185955904)
* **[13] [그림자매핑(PCF샘플러)]**(https://blog.naver.com/syl610/224155261089)
* **[14] [환경맵(큐브맵)]**(https://blog.naver.com/syl610/224139108832)
* **[15] [조명연산(Directional, Point, Specular)]**(https://blog.naver.com/syl610/223955371094)
* **[16] [노말매핑]**(https://blog.naver.com/syl610/224014017132)
* **[17] [Gbuffer기반 디퍼드렌더링, 포트스프로세싱, 조명연산]**(https://blog.naver.com/syl610/224286380281)
* **[18] [충돌체 및 오프젝트피킹]**(https://blog.naver.com/syl610/224211606661)
* **[19] [GPU(계산셰이더)기반 충돌판정 및 아웃라인/피킹 트라이앵글]**(https://blog.naver.com/syl610/224245464364)
* **[20] [패스별 세부설명 및 오버뷰(작업정리6)]**(https://blog.naver.com/syl610/224314221774)
